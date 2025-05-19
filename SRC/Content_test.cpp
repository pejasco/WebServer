/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Content_test.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chuleung <chuleung@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 15:10:35 by chuleung          #+#    #+#             */
/*   Updated: 2025/05/19 17:12:10 by chuleung         ###   ########.fr       */
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


void Content::setCDs(const std::string& buffer, CD_header header, int index){
    std::stringstream iss(buffer);
    
    switch (header){
        case ContentDisposition: {
            size_t pos_begin;
            size_t pos_end;
            std::string cd_element;
            
            std::string type_part;
            getline(iss, type_part, '; ');
            pos_begin = type_part.find_first_not_of(" \t");
            if (pos_begin != std::string::npos)
                    CDs_list_[index].CD_type_ = type_part.substr(pos_begin);

            while(getline(iss, cd_element, ';')){
                size_t pos_begin = cd_element.find_first_not_of(" \t");
                if (pos_begin != std::string::npos)
                    cd_element = cd_element.substr(pos_begin);
                if (cd_element.find("name=") != std::string::npos){
                    pos_begin = cd_element.find("\"") + 1;
                    pos_end = cd_element.rfind("\"");
                    std::string res = cd_element.substr(pos_begin, pos_end - pos_begin);
                    CDs_list_[index].name_ = res;
                } else if (cd_element.find("filename=") != std::string::npos) {
                    pos_begin = cd_element.find("\"") + 1;
                    pos_end = cd_element.rfind("\"");
                    std::string res = cd_element.substr(pos_begin, pos_end - pos_begin);
                    CDs_list_[index].filename_ = res;
                }
            }
            break;
        }
        case InterContentType: {
            std::string dummy;
            iss >> dummy >> CDs_list_[index].inner_content_type_;

            break;
        }
        case Content: {
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


const std::vector<ContentDisposition_> Content::getCDs(){
    return CDs_list_;
}
