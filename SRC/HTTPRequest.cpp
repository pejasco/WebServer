/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:19:15 by chuleung          #+#    #+#             */
/*   Updated: 2025/07/08 13:28:06 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/HTTPResponse.hpp"

// #define PORT 8080
#define BUFFER_SIZE 4096


//<<static variables>>
int Accept::global_index_ = 0;
int HTTPRequest::global_index_ = 0;

//<<Accept>>
Accept::Accept(std::string type, std::string subtype, float piority) : type_(type), subtype_(subtype), piority_(piority){}
Accept::~Accept(){}



//<<HTTPRequest>>
HTTPRequest::HTTPRequest() : instance_index_(global_index_++),
	cgi_flag_(false), content_length_(0), with_file_flag_(false) {
	boundary_ = "";
	open_boundary_ = "";
}

HTTPRequest::~HTTPRequest() {}


//Setters

void HTTPRequest::setContentLength(int length) {
	content_length_ = length;
}

void HTTPRequest::setContentFlag(const bool flag){
	content_flag_ = flag;

}

void HTTPRequest::setBoundaryIndex(int index){
	boundary_index_ = index;
}


void HTTPRequest::setIsInTheBodyFlag(const bool flag){
	is_in_the_body_flag_ = flag;
}


void HTTPRequest::setCGIFlag(const bool flag){
	cgi_flag_ = flag;
}

void HTTPRequest::setCGIType(const std::string cgi_type){
	cgi_type_ = cgi_type;
}


void HTTPRequest::setMet(const std::string&  method){
	if (method == "GET")
		method_ = GET;
	else if (method == "POST")
	{
		method_ = POST;
		setContentFlag(true); //indicating there is content
	}
	else if (method == "DELETE")
		method_ = DELETE;
	else
		std::cerr << BOLD RED "Error: Invalid method --> " << method << RESET << std::endl;
}

void HTTPRequest::setPath(const std::string& path) {
	path_ = path;
	if (path_.find("cgi-bin") != std::string::npos) // not a good flag as it will considere any pathname that include cgi as a cgi request
		setCGIFlag(true);
	path_ = urlDecoder(path_);
}

void HTTPRequest::setVersion(const std::string& version){
	version_ = version;
	// std::cout << "version: " << version << std::endl;
}

void HTTPRequest::setHost(std::string& host) {
	host.erase(host.end() - 1);
	host_ = host;
}

void HTTPRequest::setUserAgent(const std::string& agents){
	std::stringstream iss(agents);
	std::string token;
	std::string last_key;

	while(iss >> token)
	{
		if (token.find('/') != std::string::npos){
			last_key = token;
			user_agent_[last_key] = "";
		} 
		else if (token[0] == '(') {
			std::string group = token;

			bool group_complete = false;
			// if (token.find(')') != std::string::npos) {
			//     group_complete = true;

			// }
			while (!group_complete && iss >> token){

				group += " " + token;
				if (token.find('\'') != std::string::npos) {
					group_complete = true;
				}
			}
			user_agent_[last_key] = group;
		}
	}
}

void HTTPRequest::setAccept(const std::string& media_types){
	std::stringstream iss(media_types);
	std::string media_type;
	size_t pos_begin;
	size_t pos_end;
	std::string type;
	std::string subtype;
	float priority;

	while(getline(iss, media_type, ','))
	{
		priority = 1.0;
		
		if (media_type.find(';') == std::string::npos) {
			pos_end = media_type.find('/');
			pos_begin = 0;
			
			if (pos_begin == pos_end)
				type = "*";
			else
				type = media_type.substr(0, pos_end);
				
			pos_begin = pos_end + 1;
			subtype = media_type.substr(pos_begin, std::string::npos);
		} else {
			// Handle piority
			pos_end = media_type.find('/');
			type = media_type.substr(0, pos_end);
			
			pos_begin = pos_end + 1;
			pos_end = media_type.find(';');
			subtype = media_type.substr(pos_begin, pos_end - pos_begin);
			
			// Extract piority
			if (media_type.find("q=") != std::string::npos) {
				pos_begin = media_type.find("q=") + 2;
				std::string q = media_type.substr(pos_begin);
				std::stringstream ss(q);
				ss >> priority;
			}
		}
		
		//need to handle more type in the future
		if (subtype.empty() && type == "image") {
			subtype = "*";
		}
		
		accept_list_.push_back(Accept(type, subtype, priority));
	}
}

void HTTPRequest::setAcceptLanguage(const std::string& languages){
	std::stringstream iss(languages);
	std::string language;
	size_t pos_begin;
	size_t pos_end;
	std::string lang;
	float priority;

	while(getline(iss, language, ',')){
		priority = 1.0;

		if (language.find(';') == std::string::npos)
			lang = language;
		else {
			pos_end = language.find(';');
			lang = language.substr(0, pos_end);
			if (language.find("q=") != std::string::npos){
				pos_begin = language.find("q=") + 2;
				std::string q = language.substr(pos_begin);
				std::stringstream ss(q);
				ss >> priority;
			}
		}
		accept_language_[lang] = priority;
	}
}

void HTTPRequest::setReferer(const std::string& referer){
	referer_ = referer;
	// std::cout << "referer: " << referer << std::endl;
}

void HTTPRequest::setAcceptEncoding(const std::string& encoding){
	std::istringstream iss(encoding);
	std::string line;

	while (std::getline(iss, line, ',')){
		accept_encoding_.push_back(line);
	}
}

void HTTPRequest::setConnection(const std::string& connection){
	if (connection == "keep-alive")
		connection_ = true;
}

// void setCookie(const std::string& version)


void HTTPRequest::setAuthorization(const std::string& version){
	(void)version;

}


void HTTPRequest::setContent(const std::string& content){
	(void)content;


}

void HTTPRequest::setUnknown(const std::string& buffer){
	(void)buffer;

	
}

void HTTPRequest::setOpenBoundary(const std::string& open_boundary){
	open_boundary_ = open_boundary;

}

void HTTPRequest::setCloseBoundary(const std::string& close_boundary){
	close_boundary_ = close_boundary;
}

void HTTPRequest::setBoundary(const std::string& boundary){
	DEBUG_PRINT("setBoundary() called");
	boundary_ = boundary;
	DEBUG_PRINT("Printing boundary: " << boundary);
	setOpenBoundary(("--" + boundary_));
	setCloseBoundary((open_boundary_ + "--"));
	DEBUG_PRINT("Open_boundary: " << open_boundary_ << ", close_boundary: " << close_boundary_);

	content_.setBoundary(boundary_);
	content_.setOpenBoundary(open_boundary_);
	content_.setCloseBoundary(close_boundary_);

}


void HTTPRequest::setContentType(std::string contentType){
	std::istringstream ss(contentType);
	std::string type;
	std::string value;

	std::getline(ss, type, '/');
	std::getline(ss, value);
	content_type_[type] = value;
	DEBUG_PRINT("setContentType() called\ncontent_type_[" << type << "] = " << value);
	
}

void HTTPRequest::setCDFlag(bool flag){
	with_cd_flag_ = flag;
}


MET HTTPRequest::getMethod() {
	return method_;
};

const std::string& HTTPRequest::getPath() const{
	return path_;
}

const std::string& HTTPRequest::getVersion(){
	return version_;
}

const std::string& HTTPRequest::getHost() const {
	return host_;
}

int HTTPRequest::getContentLength() {
	return content_length_;
};

const std::map<std::string, std::string>& HTTPRequest::getUserAgent(){
	return user_agent_;
}

std::vector<Accept>& HTTPRequest::getAccept(){
	return accept_list_;
}

const std::map<std::string, float>& HTTPRequest::getAcceptLanguage(){
	return accept_language_;
}

bool HTTPRequest::getConnection(){
	return connection_;
}

const std::string& HTTPRequest::getReferer(){
	return referer_;
}


const std::pair<std::string, std::string>& HTTPRequest::getAuthorisation(){
	return authorisation_;
}

Content& HTTPRequest::getContent() {
	return content_;
}

const Content& HTTPRequest::getContent() const{
	return content_;
}

const std::map <std::string, std::string>& HTTPRequest::getUnknown(){
	return unknown_;
}

bool HTTPRequest::getCGIFlag() {
	return cgi_flag_;
}

bool HTTPRequest::getIsInTheBody() {
	return is_in_the_body_flag_;
}

const std::string HTTPRequest::getBoundary() const{
	return boundary_;
}

const std::string HTTPRequest::getOpenBoundary() const{
	return open_boundary_;
}

const std::string HTTPRequest::getCloseBoundary() const{
	return close_boundary_;
}

bool  HTTPRequest::getCDFlag(){
	return with_cd_flag_;
}

std::string HTTPRequest::getRawBody() const {
	return raw_body_;
}

//Parser

// void HTTPRequest::parseContent(const std::string& body_line){
// 	size_t pos_begin;
// 	size_t pos_end;
// 	std::string line;
// 	(void)pos_end;
	
// 	// std::cout << BOLD MAGENTA "I got the body_line, time to parse content\n" RESET;
// 	if (body_line == content_.getBoundary()){
// 		// std::cout << "check1\n";
// 		setIsInTheBodyFlag(true);
// 		if (boundary_index_ == -1)
// 			boundary_index_ = 0;
// 		else
// 			boundary_index_ += 1;
// 		// multipart/form-data
// 	} else if (body_line == content_.getBoundary() + "--"){
// 		// std::cout << "check1bis\n";
// 		setContentFlag(false);
// 		setCGIFlag(false);
// 		setIsInTheBodyFlag(false);
// 		setBoundaryIndex(-1);

// 	} else if (cgi_flag_ == 0 && content_.getContentLength() >= 0 
// 		&& !content_.getBoundary().empty() && is_in_the_body_flag_){
// 		// std::cout << "check2\n";
// 		if (cgi_flag_ == true){
// 			// std::cout << "check2bis\n";
// 			size_t pos_begin = path_.rfind(".");
// 			cgi_type_ = path_.substr(pos_begin + 1);
// 		}
		
// 		if (body_line.find("Content-Disposition:") != std::string::npos){
// 			// std::cout << "check3\n";
// 			content_.addContentDisposition();
// 			// std::cout << "is it working here\n";
// 			if((pos_begin = body_line.find(':')) != std::string::npos){
// 				pos_begin = body_line.find_first_not_of(" \t", pos_begin + 1);
// 				line = body_line.substr(pos_begin);
// 				content_.setCDs(line, ContentDisposition, boundary_index_);
// 			}
// 		} else if (body_line.find("Content-Type:") != std::string::npos){
// 			// std::cout << "check4\n";
// 			if((pos_begin = body_line.find(':')) != std::string::npos){
// 				pos_begin = body_line.find_first_not_of(" \t", pos_begin + 1);
// 				line = body_line.substr(pos_begin);
// 				content_.setCDs(line, InterContentType, boundary_index_);}
		
// 		} else if (!body_line.empty()
// 			&& (content_.getCDs()[boundary_index_].filename_.empty())){
// 				// std::cout << "check4\n";
// 				content_.setCDs(body_line, Cont, boundary_index_);
				
// 		} else if (!body_line.empty()
// 			&& !(content_.getCDs()[boundary_index_].filename_.empty())){
// 				// std::cout << "check5\n";
// 				content_.setCDs(body_line, FileContent, boundary_index_);
	
// 		}
			
// 	} else {
// 		// Handle
// 		// std::cout << "check6\n";
// 		content_.setBodyWithNoCD(body_line); 
		
// 	}
// }

// void HTTPRequest::parseContent(const std::string& body_line) {
//     size_t pos_begin;
//     size_t pos_end;
//     std::string line;

// 	std::cout << "<<sievdebug>>  parseContent called with body_line size: " << body_line.size() << std::endl;
//     std::istringstream stream(body_line);
//     while (std::getline(stream, line)) {
//         // std::cout << BOLD MAGENTA "I got the body_line, time to parse content\n" RESET;
//         // trimString(line);
//         if (line.empty()) {
//             continue;
//         } 
//         else if (line.find(close_boundary_) != std::string::npos) {
//             // std::cout << "check1\n";
//             with_file_flag_ = false;
//             with_cd_flag_ = false;
//             //cgi_flag_ = false; //need to review further?!?!
//             return;
//             // multipart/form-data
//         } 
//         else if (line.find(open_boundary_) != std::string::npos &&
//                  line.find(close_boundary_) == std::string::npos) {
//             // std::cout << "check1bis\n";
//             with_file_flag_ = false;
//         } 
//         else if (line.find("Content-Disposition:") != std::string::npos) {
//             // std::cout << "check3\n";
//             content_.addContentDisposition();
//             with_cd_flag_ = true;
//             // std::cout << "is it working here\n";
//             ContentDisposition_ & last_cd = content_.getCDs().back();
//             //--CD Type
//             // if ((pos_begin = line.find(':')) != std::string::npos) {
//             //     pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
//             //     pos_end = line.find(";", pos_begin);
//             //     line = line.substr(pos_begin);
//             //     std::string type = trimString(line.substr(pos_begin, pos_end - pos_begin));
//             //     last_cd.CD_type_ = type;
//             // }
// 			if ((pos_begin = line.find(':')) != std::string::npos) {
//     			pos_begin++; // move past ':'
//    				 pos_begin = line.find_first_not_of(" \t", pos_begin);
// 				if (pos_begin != std::string::npos) {
// 					pos_end = line.find(";", pos_begin);
// 					std::string type;
// 					if (pos_end != std::string::npos)
// 						type = trimString(line.substr(pos_begin, pos_end - pos_begin));
// 					else
// 						type = trimString(line.substr(pos_begin));
// 					last_cd.CD_type_ = type;
// 				}
// 			}
//             //--name
//             if (line.find("name") != std::string::npos) {
//                 if ((pos_begin = line.find("name")) != std::string::npos) {
//                     pos_begin = (line.find("\"", pos_begin)) + 1;
//                     pos_end = (line.find("\"", pos_begin));
//                     std::string name = trimString(line.substr(pos_begin, pos_end - pos_begin));
//                     last_cd.name_ = name;
//                 }
//             }
//             if (line.find("filename") != std::string::npos) {
//                 if ((pos_begin = line.find("filename")) != std::string::npos) {
//                     pos_begin = (line.find("\"", pos_begin)) + 1;
//                     pos_end = (line.find("\"", pos_begin));
//                     std::string filename = trimString(line.substr(pos_begin, pos_end - pos_begin));
//                     last_cd.filename_ = filename;
//                     with_file_flag_ = true;
//                 }
//             }
//         } 
//         else if (line.find("Content-Type:") != std::string::npos) {
//             // std::cout << "check4\n";
//             if ((pos_begin = line.rfind(":")) != std::string::npos) {
//                 pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
//                 pos_end = line.find(";", pos_begin);
//                 std::string inner_cttype = trimString(line.substr(pos_begin, pos_end - pos_begin));
//                 ContentDisposition_ & last_cd = content_.getCDs().back();
//                 last_cd.inner_content_type_ = inner_cttype;
//             }
//         }
//         else {
//             // Handle
//             // std::cout << "check6\n";
//             if (!with_cd_flag_) {
//                 content_.setBodyWithNoCD(line); 
//             }
//             else if (with_cd_flag_) {
//                 ContentDisposition_ & last_cd = content_.getCDs().back();
//                 if (with_file_flag_)
//                     last_cd.file_content_ += line;
//                 else
//                     last_cd.content_ += line;
//             }
//         }
//     }
// }

void HTTPRequest::parseContent(const std::string& body) {

	DEBUG_PRINT(BOLD UNDERLINE BLACK BG_WHITE "PARSE CONTENT CALLED" RESET);
	
	if (open_boundary_.empty()){
		content_.setBodyWithNoCD(body);
		DEBUG_PRINT("no boundary");
		return;
	}
	
	size_t pos = 0;
	while (true) {
		size_t boundary_start = body.find(open_boundary_, pos);
		if (boundary_start == std::string::npos)
			break;
		boundary_start += open_boundary_.size();
		if (body.substr(boundary_start, 2) == "\r\n")
			boundary_start += 2;
		size_t headers_end = body.find("\r\n\r\n", boundary_start);
		if (headers_end == std::string::npos)
			break;
		std::string headers = body.substr(boundary_start, headers_end - boundary_start);
		std::istringstream header_stream(headers);
		std::string header_line;
		content_.addContentDisposition();
		ContentDisposition_ &last_cd = content_.getCDs().back();
		with_file_flag_ = false;
		while (std::getline(header_stream, header_line)) {
			if (header_line.find("Content-Disposition:") != std::string::npos) {
				size_t pos_begin, pos_end;
				if ((pos_begin = header_line.find(':')) != std::string::npos) {
					pos_begin++;
					pos_begin = header_line.find_first_not_of(" \t", pos_begin);
					if (pos_begin != std::string::npos) {
						pos_end = header_line.find(";", pos_begin);
						std::string type;
						if (pos_end != std::string::npos)
							type = trimString(header_line.substr(pos_begin, pos_end - pos_begin));
						else
							type = trimString(header_line.substr(pos_begin));
						last_cd.CD_type_ = type;
					}
				}
				if (header_line.find("name") != std::string::npos) {
					if ((pos_begin = header_line.find("name")) != std::string::npos) {
						pos_begin = (header_line.find("\"", pos_begin)) + 1;
						pos_end = (header_line.find("\"", pos_begin));
						std::string name = trimString(header_line.substr(pos_begin, pos_end - pos_begin));
						last_cd.name_ = name;
					}
				}
				if (header_line.find("filename") != std::string::npos) {
					if ((pos_begin = header_line.find("filename")) != std::string::npos) {
						pos_begin = (header_line.find("\"", pos_begin)) + 1;
						pos_end = (header_line.find("\"", pos_begin));
						std::string filename = trimString(header_line.substr(pos_begin, pos_end - pos_begin));
						last_cd.filename_ = filename;
						with_file_flag_ = true;
					}
				}
			}
			else if (header_line.find("Content-Type:") != std::string::npos) {
				size_t pos_begin, pos_end;
				if ((pos_begin = header_line.rfind(":")) != std::string::npos) {
					pos_begin = header_line.find_first_not_of(" \t", pos_begin + 1);
					pos_end = header_line.find(";", pos_begin);
					std::string inner_cttype = trimString(header_line.substr(pos_begin, pos_end - pos_begin));
					last_cd.inner_content_type_ = inner_cttype;
				}
			}
		}

		size_t content_start = headers_end + 4;
		size_t next_boundary = body.find(open_boundary_, content_start);
		if (next_boundary == std::string::npos)
			next_boundary = body.size();

		std::string part_content = body.substr(content_start, next_boundary - content_start);
		if (part_content.size() >= 2 && part_content.substr(part_content.size() - 2) == "\r\n")
			part_content.resize(part_content.size() - 2);

		if (with_file_flag_)
			last_cd.file_content_ = part_content;
		else
			last_cd.content_ = part_content;

		pos = next_boundary;
		if (body.substr(pos, close_boundary_.size()) == close_boundary_)
			break; 
	}
	DEBUG_PRINT(BOLD UNDERLINE BLACK BG_WHITE "END PARSE CONTENT" RESET);
}



//if (!(content_.getContentType()->first).empty() && content_.getContentLength() >= 0;

void HTTPRequest::parseRequestHeader(std::istringstream& stream) {
	DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "PARSE REQUEST HEADER CALLED" RESET);
	std::string line;
	size_t pos_begin;
	size_t pos_end;
	// size_t pos_end;
	

	while (std::getline(stream, line)) {
		trimString(line);
		if (line.empty()) {
			return;        
		} 
		else if (line.find("Host") != std::string::npos) {
			if ((pos_begin = line.find(":")) != std::string::npos) {
				pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
				std::string host = line.substr(pos_begin, std::string::npos);
				setHost(host);
				DEBUG_PRINT("Host: " << host);
			}
		} 
		else if (line.find("Connection") != std::string::npos) {
			if ((pos_begin = line.rfind(":")) != std::string::npos) {
				pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
				std::string connection = line.substr(pos_begin, std::string::npos);
				setConnection(connection);
				DEBUG_PRINT("connection: " << connection);
			}
		} 
		else if (line.find("User-Agent") != std::string::npos) {   
			if ((pos_begin = line.rfind(":")) != std::string::npos) {
				pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
				std::string agents = line.substr(pos_begin, std::string::npos);
				setUserAgent(agents);
				DEBUG_PRINT("agents: " << agents);
			}
		} 
		else if (line.find("Accept") != std::string::npos) {
			if ((pos_begin = line.rfind(":")) != std::string::npos) {
				pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
				std::string accept = line.substr(pos_begin, std::string::npos);
				setAccept(accept);
				DEBUG_PRINT("accept: " << accept);
			}
		} 
		else if (line.find("Referer") != std::string::npos) {
			if ((pos_begin = line.rfind(":")) != std::string::npos) {
				pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
				std::string referer = line.substr(pos_begin, std::string::npos);
				setReferer(referer);
				DEBUG_PRINT("referer: " << referer);
			}
		} 
		else if (line.find("Accept-Encoding") != std::string::npos) {
			if ((pos_begin = line.rfind(":")) != std::string::npos) {
				pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
				std::string encoding = line.substr(pos_begin, std::string::npos);
				setAcceptEncoding(encoding);
				DEBUG_PRINT("encoding: " << encoding);
			}
		} 
		else if (line.find("Accept-Language") != std::string::npos) {
			if ((pos_begin = line.rfind(":")) != std::string::npos) {
				pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
				std::string languages = line.substr(pos_begin, std::string::npos);
				setAcceptLanguage(languages);
				DEBUG_PRINT("languages: " << languages);
			}
		} 
		// content related (only applicable to POST and DEL)
		else if (line.find("Content-Type") != std::string::npos) {
			if ((pos_begin = line.rfind(":")) != std::string::npos) {
				pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
				pos_end = line.find(";");
				std::string cttype = trimString(line.substr(pos_begin, pos_end - pos_begin));
				setContentType(cttype);
				if (line.find("boundary") != std::string::npos) {
					pos_begin = line.find("boundary") + 8;
					pos_begin = line.find_first_of("=", pos_begin);
					if (pos_begin != std::string::npos) {
						pos_begin++;
						pos_begin = line.find_first_not_of(" \t", pos_begin); 
						std::string boundary = trimString(line.substr(pos_begin, std::string::npos));
						if (!boundary.empty() && (boundary[0] == '"' || boundary[0] == '\'')) {
							size_t quote_end = boundary.find_first_of("\"'", 1);
							if (quote_end != std::string::npos) {
								boundary = trimString(boundary.substr(1, quote_end - 1));
							} else {
								boundary = trimString(boundary.substr(1));
							}
						}
						
						size_t end_pos = boundary.find_last_not_of(" \t;");
						if (end_pos != std::string::npos) {
							boundary = trimString(boundary.substr(0, end_pos + 1));
						}
						setBoundary(boundary);
						setCDFlag(true);
					}
				}
			}
		} 
		else if (line.find("Content-Length") != std::string::npos) {
			if ((pos_begin = line.rfind(":")) != std::string::npos) {
				pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
				std::string length = trimString(line.substr(pos_begin));
				int len = atoi(length.c_str());
				setContentLength(len);
				DEBUG_PRINT("content_length: " << len);
				content_.setContentLength(len);
			}
		} 
	}
	DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "PARSE REQUEST HEADER EXITED" RESET);
}

// Ensure the function is defined outside the class scope
void HTTPRequest::parseRequestLine(const std::string& request_line) {
	DEBUG_PRINT(BOLD UNDERLINE BG_YELLOW BLACK "PARSE REQUEST LINE CALLED" RESET);
	std::string method, request_uri, version;
	std::istringstream stream(request_line);
	stream >> method >> request_uri >> version;
	DEBUG_PRINT("Raw request line: " << request_line);

	// Split path and query
	size_t pos = request_uri.find('?');
	if (pos != std::string::npos) {
		std::string path = request_uri.substr(0, pos);
		std::string query = request_uri.substr(pos + 1);
		setPath(path);
		setQueryStr(query);
		DEBUG_PRINT("Path: " << path << ", Query: " << query);
	} else {
		setPath(request_uri);
		setQueryStr("");
		DEBUG_PRINT("Path: " << path_ << ", Query: empty");
	}
	setMet(method);
	setVersion(version);
	DEBUG_PRINT("Method: " << method << ", version: " << version);
	DEBUG_PRINT(BOLD UNDERLINE BG_YELLOW BLACK "PARSE REQUEST LINE EXITED" RESET);
}




void HTTPRequest::parseRequest(const std::string& request){
	std::istringstream stream(request);
	std::string request_line;
	DEBUG_PRINT(BOLD WHITE "\n\n---------------------\n---------------------\nPARSE REQUEST STARTED\n---------------------\n---------------------\n" RESET);
	std::getline(stream, request_line);
	parseRequestLine(request_line); //dry method, uri, ver
	parseRequestHeader(stream);
	DEBUG_PRINT(BOLD WHITE "\n\n---------------------\n---------------------\nPARSE REQUEST EXITED\n---------------------\n---------------------\n" RESET);
	// should modify it 
	//-note: the ServerManger::readRequestBody already 
	//calling parseContent, but it do again in parseRequestHeader

	


	// size_t pos = request.find(" ");
	// if (pos != std::string::npos)
	// {
	//     std::string method = request.substr(0, pos);
	//     try
	//     {
	//         setMet("INVALID_METHOD");}
	//     catch (const std::invalid_argument& e)
	//     {
	//         std::cerr << "Error: " << e.what() << std::endl;}
	// }
}

//cgi
std::string HTTPRequest::getMethodAsStr() const //cgi needs it as a string
{
	switch (method_) {
		case GET: return "GET";
		case POST: return "POST";
		case DELETE: return "DELETE";
		default: return "UNKNOWN";
	}
}

std::string HTTPRequest::getQueryStr() const { return query_string_; }

std::map<std::string, std::string> HTTPRequest::getHeaders() const { return headers_; }



void HTTPRequest::setQueryStr(const std::string& query) { query_string_ = query; }
//std::string HTTPRequest::getcgiPath() { return path_; }