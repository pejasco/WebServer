/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Content_test.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 15:10:35 by chuleung          #+#    #+#             */
/*   Updated: 2025/05/21 14:25:32 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/HTTPResponse.hpp"

//<<static variables>>
int ContentDisposition_::global_index_ = 0;
int Content::global_index_ = 0;


//<<ContentDisposition>>
ContentDisposition_::ContentDisposition_() : instance_index_(global_index_++){}

ContentDisposition_::~ContentDisposition_(){}

//<<Conetent>>
Content::Content() : instance_index_(global_index_++){}


Content::~Content(){}

//setters

void Content::setBounday(const std::string& boundary){
    size_t pos;
    std::string tmp;

    if ((pos = boundary.find("boundary=")) != std::string::npos){
        tmp = boundary.substr(pos + 9);
    }
    boundary_ = "--" + tmp;

}

void Content::setContentType(const std::string& buffer){
    size_t pos;
    size_t pos_end;
    std::string type;
    std::string subtype;
    std::string boundary;


    if ((pos = buffer.find('/')) != std::string::npos) {
        type = buffer.substr(0, pos);
        if ((pos_end = buffer.find(';', pos)) != std::string::npos) {
            subtype = buffer.substr(pos + 1, pos_end - pos - 1); 
            pos = pos_end + 1; // Move past ';'
            if ((pos = buffer.find("boundary=", pos)) != std::string::npos) {
                boundary = buffer.substr(pos);
                setBounday(boundary);
            }
        } else
            subtype = buffer.substr(pos + 1);
    }
    content_type_.first = type;
    content_type_.second = subtype;

}

void Content::setContentLength(const std::string& buffer){
    std::stringstream ss(buffer);
    ss >> content_length_;

}


void Content::setBodyWithNoCD(const std::string& line){
    body_with_no_cd_ = line;
}


void Content::setCDs(const std::string& buffer, CD_header header, int index) { // problem with this function -- unable to parse the content of file and get segfault
    std::stringstream iss(buffer);
    std::cout << BOLD GREEN "IS THIS THE PROBLEM\n" RESET;
    std::cout << "buffer: " << buffer << std::endl;
    std::cout << "header: " << header << std::endl;
    std::cout << "index: " << index << std::endl;
    switch (header){
        case ContentDisposition: {
            size_t pos_begin;
            size_t pos_end;
            std::string cd_element;
            std::cout << "check1\n";
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
            while(getline(iss, type_part, ';')){
                std::cout << "type-part: " << type_part << std::endl;
                std::cout << "check2\n";
                pos_begin = type_part.find_first_not_of(" \t");
                std::cout << "pos_begin: " << pos_begin << std::endl;
                if (pos_begin != std::string::npos) {
                    std::cout << "check2bis BEFORE\n";
                    std::string newStr = type_part.substr(pos_begin);
                    std::cout << "CD_type: " << newStr << std::endl;
                    if (CDs_list_.empty()) {
                        std::cout << "CD doesn't exist - vector is empty\n";
                    } else {
                        // Check if index is valid
                        if (index < (int)CDs_list_.size()) {
                            CDs_list_[index].CD_type_ = newStr;
                        } else {
                            std::cout << "Error: Index " << index << " out of bounds for CDs_list_ of size " << CDs_list_.size() << std::endl;
                        }
                    }
                    
                    std::cout << "check2bis AFTER\n";  
                }
                std::cout << "check3\n";
                size_t pos_begin = cd_element.find_first_not_of(" \t");
                if (pos_begin != std::string::npos) {
                    cd_element = cd_element.substr(pos_begin);
                    std::cout << "cd_element= " << cd_element << std::endl;
                } if (cd_element.find("name=") != std::string::npos){
                    pos_begin = cd_element.find("\"") + 1;
                    pos_end = cd_element.rfind("\"");
                    std::string res = cd_element.substr(pos_begin, pos_end - pos_begin);
                    CDs_list_[index].name_ = res;
                    std::cout << "cd_name= " << res << std::endl;
                } else if (cd_element.find("filename=") != std::string::npos) {
                    pos_begin = cd_element.find("\"") + 1;
                    pos_end = cd_element.rfind("\"");
                    std::string res = cd_element.substr(pos_begin, pos_end - pos_begin);
                    CDs_list_[index].filename_ = res;
                    std::cout << "filename= " << res << std::endl;
                }
                std::cout << "check4\n";
            }
            std::cerr << "error with the getline function: " << strerror(errno) << std::endl;
            break;
        }
        case InterContentType: {
            std::string dummy;
            iss >> dummy >> CDs_list_[index].inner_content_type_;

            break;
        }
        case Cont: {
            CDs_list_[index].content_ += buffer;
            break;
        }

        case FileContent: {
            CDs_list_[index].file_content_ += buffer;
            break;
        }

    }

}

//getters
const std::pair<std::string, std::string>& Content::getContentType(){
    return content_type_;
}


const std::string& Content::getBoundary(){
    return boundary_;
}


int Content::getContentLength(){
    return content_length_;
}

const std::map<std::string, std::string> Content::getBody(){
    return body_;
}


std::vector<ContentDisposition_> &Content::getCDs(){
    return CDs_list_;
}
