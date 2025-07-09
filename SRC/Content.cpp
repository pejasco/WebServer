/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Content.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 15:10:35 by chuleung          #+#    #+#             */
/*   Updated: 2025/06/18 09:55:00 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/HTTPResponse.hpp"

//<<static variables>>
int ContentDisposition_::global_index_ = 0;
int Content::global_index_ = 0;

//<<ContentDisposition>>
ContentDisposition_::ContentDisposition_() : instance_index_(global_index_++) {}

ContentDisposition_::~ContentDisposition_() {}

//<<Conetent>>
Content::Content() : instance_index_(global_index_++), content_length_(0) {}

Content::~Content() {
	CDs_list_.clear();
	std::string().swap(boundary_);
	std::string().swap(open_boundary_);
	std::string().swap(close_boundary_);
	std::string().swap(body_with_no_cd_);
}

// setters


void Content::setOpenBoundary(const std::string& boundary)
{
	open_boundary_ = boundary;
}


void Content::setCloseBoundary(const std::string& boundary)
{
	close_boundary_ = boundary;
	
}


void Content::setBoundary(const std::string &boundary)
{
	boundary_ = boundary;
}



void Content::setContentType(const std::string &buffer)
{
	size_t pos;
	size_t pos_end;
	std::string type;
	std::string subtype;
	std::string boundary;

	if ((pos = buffer.find('/')) != std::string::npos)
	{
		type = buffer.substr(0, pos);
		if ((pos_end = buffer.find(';', pos)) != std::string::npos)
		{
			subtype = buffer.substr(pos + 1, pos_end - pos - 1);
			pos = pos_end + 1; // Move past ';'
			if ((pos = buffer.find("boundary=", pos)) != std::string::npos)
			{
				boundary = buffer.substr(pos);
				setBoundary(boundary);
			}
		}
		else
			subtype = buffer.substr(pos + 1);
	}
	content_type_.first = type;
	content_type_.second = subtype;
}

void Content::setContentLength(int len)
{
	content_length_ = len;
}

void Content::setBodyWithNoCD(const std::string &line)
{
	body_with_no_cd_ += line;
}

void Content::setCDs(const std::string &buffer, CD_header header, int index)
{ // problem with this function -- unable to parse the content of file and get segfault
	std::stringstream iss(buffer);
	DEBUG_PRINT("setCDs() calling");
	DEBUG_PRINT("Buffer: " << buffer << "Header: " << header << "Index: " << index);
	switch (header)
	{
	case ContentDisposition:
	{
		size_t pos_begin;
		size_t pos_end;
		std::string cd_element;
		std::string type_part;
		// getline(iss, type_part, ';');
		// std::cout << "type-part: " << type_part << std::endl;
		// std::cout << "check2\n";
		// pos_begin = type_part.find_first_not_of(" \t");
		// if (pos_begin != std::string::npos) {
		//     CDs_list_[index].CD_type_ = type_part.substr(pos_begin);
		//     std::cout << "check2bis\n";
		// }
		// if (getline(iss, type_part, ';'))
		//     std::cout << "type-part2: " << type_part << std::endl;
		// else {
		//     // Failure: either EOF was reached or an error occurred
		//     std::cout << "just"
		//     if (iss.eof()) {
		//         // End of file reached
		//         std::cout << "End of file reached" << std::endl;
		//     } else if (iss.bad()) {
		//         // Unrecoverable stream error
		//         std::cerr << "Unrecoverable stream error: " << std::strerror(errno) << std::endl;
		//     } else if (iss.fail()) {
		//         // Recoverable stream error (like format errors)
		//         std::cerr << "Recoverable stream error" << std::endl;
		//         iss.clear(); // Clear the error flags to continue using the stream
		//     }
		// }
		// getline(iss, type_part, ';');
		// std::cout << "type-part3: " << type_part << std::endl;
		while (getline(iss, type_part, ';'))
		{
			pos_begin = type_part.find_first_not_of(" \t");
			DEBUG_PRINT("type-part: " << type_part << "pos_begin: " << pos_begin);
			if (pos_begin != std::string::npos)
			{
				std::string newStr = type_part.substr(pos_begin);
				DEBUG_PRINT("CD_type: " << newStr);
				if (CDs_list_.empty())
				{
					DEBUG_PRINT("CD doesn't exist - vector is empty");
				}
				else
				{
					// Check if index is valid
					if (index < (int)CDs_list_.size())
					{
						CDs_list_[index].CD_type_ = newStr;
					}
					else
					{
						DEBUG_PRINT("Error: Index " << index << " out of bounds for CDs_list_ of size " << CDs_list_.size());
					}
				}

			}
			size_t pos_begin = cd_element.find_first_not_of(" \t");
			if (pos_begin != std::string::npos)
			{
				cd_element = cd_element.substr(pos_begin);
				DEBUG_PRINT("cd_element= " << cd_element);
			}
			if (cd_element.find("name=") != std::string::npos)
			{
				pos_begin = cd_element.find("\"") + 1;
				pos_end = cd_element.rfind("\"");
				std::string res = cd_element.substr(pos_begin, pos_end - pos_begin);
				CDs_list_[index].name_ = res;
				DEBUG_PRINT("cd_name= " << res);
			}
			else if (cd_element.find("filename=") != std::string::npos)
			{
				pos_begin = cd_element.find("\"") + 1;
				pos_end = cd_element.rfind("\"");
				std::string res = cd_element.substr(pos_begin, pos_end - pos_begin);
				CDs_list_[index].filename_ = res;
				DEBUG_PRINT("filename= " << res);
			}
		}
		std::cerr << BOLD RED "Error with the getline function: " << strerror(errno) << RESET << std::endl;
		break;
	}
	case InterContentType:
	{
		std::string dummy;
		iss >> dummy >> CDs_list_[index].inner_content_type_;

		break;
	}
	case Cont:
	{
		CDs_list_[index].content_ += buffer;
		break;
	}

	case FileContent:
	{
		CDs_list_[index].file_content_ += buffer;
		break;
	}
	}
}

// getters
const std::pair<std::string, std::string> &Content::getContentType()
{
	return content_type_;
}

const std::string &Content::getBoundary()
{
	return boundary_;
}

const std::string &Content::getOpenBoundary()
{
	return open_boundary_;
}

const std::string &Content::getCloseBoundary()
{
	return close_boundary_;
}


int Content::getContentLength()
{
	return content_length_;
}

// const std::map<std::string, std::string> Content::getBody()
// {
//     return body_;
// }

std::vector<ContentDisposition_>& Content::getCDs() {
	return CDs_list_;
}

const std::vector<ContentDisposition_> &Content::getCDs() const
{
	return CDs_list_;
}

std::string& Content::getBodyWithNoCD()
{ 
	return body_with_no_cd_;
}

void Content::addContentDisposition()
{
	DEBUG_PRINT("addContentDisposition() called");
	CDs_list_.push_back(ContentDisposition_());
}


void Content::printCDsList() const
{
	DEBUG_PRINT("printCDsList() called");
	for (std::vector<ContentDisposition_>::const_iterator it = CDs_list_.begin(); it != CDs_list_.end(); ++it) {
		DEBUG_PRINT("<<CD>>: " << global_index_ << " <CD>>");
		DEBUG_PRINT("  global_index: " << global_index_);
		DEBUG_PRINT("  index: " << it->instance_index_);
		DEBUG_PRINT("  CD_type: [" << it->CD_type_);
		DEBUG_PRINT("  name: [" << it->name_ << "]");
		DEBUG_PRINT("  filename: [" << it->filename_ << "]");
		DEBUG_PRINT("  inner_content: [" << it->inner_content_type_ << "]");
		DEBUG_PRINT("  content_: [" << it->content_ << "]");
		DEBUG_PRINT("  file_content_: [" << it->file_content_.size() << "]");
	}
	DEBUG_PRINT("printCDsList() exited");
}

// const std::map<std::string, std::string> Content::getBodyconst() const { return body_; }