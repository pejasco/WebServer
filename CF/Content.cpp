/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Content.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 15:10:35 by chuleung          #+#    #+#             */
/*   Updated: 2025/05/21 14:19:26 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/Content.hpp"

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

    if ((pos = boundary.find("boundary=----")) != std::string::npos){
        tmp = boundary.substr(pos + 10);
    }
    boundary_ = tmp;
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


void Content::setBody(const std::string& buffer){



}


void Content::setCDs(const std::string& buffer){



}

//getters
const std::pair<std::string, std::string>& Content::getContentType(){
    return content_type_;
}


const std::string& Content::getBoundary(){
    return boundary_;
}


const int Content::getContentLength(){
    return content_length_;
}

const std::map<std::string, std::string> Content::getBody(){
    return body_;
}


std::vector<ContentDisposition_> &Content::getCDs(){
    return CDs_list_;
}
