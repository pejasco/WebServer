/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Content.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chuleung <chuleung@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 15:10:35 by chuleung          #+#    #+#             */
/*   Updated: 2025/05/08 17:09:22 by chuleung         ###   ########.fr       */
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
void Content::setContentType(const std::string& buffer){
    size_t pos;
    std::string type;
    std::string subtype;

    if ((pos = buffer.find('/')) != std::string::npos){
        type = buffer.substr(0, pos);
        subtype = buffer.substr(pos + 1);
    }
    content_type_.first = type;
    content_type_.second = subtype;

}

void Content::setBounday(const std::string& buffer){



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


const std::vector<ContentDisposition_> Content::getCDs(){
    return CDs_list_;
}
