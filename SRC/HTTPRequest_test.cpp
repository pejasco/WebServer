/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest_test.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chuleung <chuleung@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:19:15 by chuleung          #+#    #+#             */
/*   Updated: 2025/05/16 19:34:58 by chuleung         ###   ########.fr       */
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
HTTPRequest::HTTPRequest() : instance_index_(global_index_++), content_flag_(false),
    cgi_flag_(false), is_in_the_body_flag_(false){}

HTTPRequest::~HTTPRequest(){}


//Setters

void HTTPRequest::setIsInTheBodyFlag(const bool flag){
    is_in_the_body_flag_ = flag;
}


void HTTPRequest::setCGIFlag(const bool flag){
    cgi_flag_ = flag;
}


void HTTPRequest::setMet(const std::string&  method){
    if (method == "GET")
        method_ = GET;
    else if (method == "POST")
    {
        method_ = POST;
        content_flag_ = true; //indicating there is content
    }
    else if (method == "DELETE")
        method_ = DELETE;
    else
        throw std::invalid_argument("Invalid HTTP method: " + method);
}

void HTTPRequest::setPath(const std::string& path){
    path_ = path;
    if (path_.find("cgi") != std::string::npos)
        setCGIFlag(true);
}

void HTTPRequest::setVersion(const std::string& version){
    version_ = version;
}

void HTTPRequest::setHost(const std::string& host){
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
        else if (token[0] == '('){
            std::string group = token;

            bool group_complete = false;
            // if (token.find(')') != std::string::npos) {
            //     group_complete = true;

            // }
            while (!group_complete && iss >> token){

                group += " " + token;
                if (token.find(')') != std::string::npos) {
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
}

void HTTPRequest::setAcceptEncoding(const std::string& encoding){
    std::istringstream iss(encoding);
    std::string line;

    while (std::getline(iss, line, ',')){
        accept_encoding_.push_back(line);
    }
}

void HTTPRequest::setConnection(const std::string& connection){
    (void)connection;

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

MET HTTPRequest::getMethod() {
    return method_;
};

const std::string& HTTPRequest::getPath(){
    return path_;
}

const std::string& HTTPRequest::getVersion(){
    return version_;
}

const std::string& HTTPRequest::getHost(){
    return host_;
}

const std::map<std::string, std::string>& HTTPRequest::getUserAgent(){
    return user_agent_;
}

const std::vector<Accept>& HTTPRequest::getAccept(){
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

const Content& HTTPRequest::getContent(){
    return content_;
}

const std::map <std::string, std::string>& HTTPRequest::getUnknown(){
    return unknown_;
}

bool HTTPRequest::getCGIFlag() {
    return cgi_flag_;
}

//Parser

void HTTPRequest::parseRequestLine(const std::string& request_line){
    std::string method, path, version;
    
    std::istringstream stream(request_line);
    stream >> method >> path >> version;

    setMet(method);
    // std::cout << "This is the method: " << this->method_ << std::endl;
    setPath(path);
    // std::cout << "This is the path: " << this->path_ << std::endl;
    setVersion(version);
    // std::cout << "This is the version: " << this->version_ << std::endl;
}

void HTTPRequest::parseContent(const std::string& body_line){
    if (body_line == content_.getBoundary()){
        setIsInTheBodyFlag(true);
        return ;
    } else if (cgi_flag_ && content_.getContentLength() >= 0 && !content_.getBoundary().empty()
            && is_in_the_body_flag_){
        if (body_line.rfind(""))

        
    } else if (cgi_flag_) {
        // Handle CGI-specific content parsing here
    } else {
        content_.setBodyWithNoCD(body_line); 
    }
}
//if (!(content_.getContentType()->first).empty() && content_.getContentLength() >= 0

void HTTPRequest::parseRequestHeader(std::istringstream& stream){
    std::string line;
    size_t pos_begin;
    // size_t pos_end;

    while (std::getline(stream, line))
    {
        //GET
        if (line.empty()){
            if (!(content_.getContentType().first).empty() && content_.getContentLength() >= 0 && is_in_the_body_flag_)
                parseContent(line);
            else
                continue;
        } else if (line.find("Host") != std::string::npos){
            if ((pos_begin = line.rfind(":")) != std::string::npos){
                pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
                std::string host = line.substr(pos_begin, std::string::npos);
                setHost(host);}

        } else if (line.find("Connection") != std::string::npos){
            if ((pos_begin = line.rfind(":")) != std::string::npos){
                pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
                std::string connection = line.substr(pos_begin, std::string::npos);
                setConnection(connection);}

        } else if (line.find("User-Agent") != std::string::npos){   
            if ((pos_begin = line.rfind(":")) != std::string::npos){
                pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
                std::string agents = line.substr(pos_begin, std::string::npos);
                setUserAgent(agents);}

        } else if (line.find("Accept") != std::string::npos){
            if ((pos_begin = line.rfind(":")) != std::string::npos){
                pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
                std::string accept = line.substr(pos_begin, std::string::npos);
                setAccept(accept);}


        } else if (line.find("Referer") != std::string::npos){
            if ((pos_begin = line.rfind(":")) != std::string::npos){
                pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
                std::string referer = line.substr(pos_begin, std::string::npos);
                setReferer(referer);}

        } else if (line.find("Accept-Encoding") != std::string::npos){
            if ((pos_begin = line.rfind(":")) != std::string::npos){
                pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
                std::string encoding = line.substr(pos_begin, std::string::npos);
                setAcceptEncoding(encoding);}

        } else if (line.find("Accept-Language") != std::string::npos){
            if ((pos_begin = line.rfind(":")) != std::string::npos){
                pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
                std::string languages = line.substr(pos_begin, std::string::npos);
                setAcceptLanguage(languages);}

        } 
        // content related (only applicable to POST and DEL)
        else if (line.find("Content-Type") != std::string::npos){
            if ((pos_begin = line.rfind(":")) != std::string::npos){
                pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
                std::string cttype = line.substr(pos_begin, std::string::npos);
                content_.setContentType(cttype);}

        } else if (line.find("Content-Length") != std::string::npos){
            if ((pos_begin = line.rfind(":")) != std::string::npos){
                pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
                std::string ctlength = line.substr(pos_begin, std::string::npos);
                content_.setContentLength(ctlength);
                }
            
        } else if (content_flag_ == true)
            HTTPRequest::parseContent(line);
        else
            return;
    }
}

void HTTPRequest::parseRequest(const std::string& request){

    std::istringstream stream(request);
    std::string request_line;

    std::getline(stream, request_line);
    parseRequestLine(request_line);
    parseRequestHeader(stream);

    


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
