/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chuleung <chuleung@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:19:15 by chuleung          #+#    #+#             */
/*   Updated: 2025/05/08 18:21:31 by chuleung         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <stdio.h>
// #include <stdlib.h>
// #include <string>
// #include <iostream>
// #include <unistd.h>
// #include <netinet/in.h>
#include "../INC/utils/HTTPRequest.hpp"

// #define PORT 8080
#define BUFFER_SIZE 4096


//<<static variables>>
int Accept::global_index_ = 0;
int HTTPRequest::global_index_ = 0;

//<<Accept>>
Accept::Accept(std::string type, std::string subtype, float piority) : type_(type), subtype_(type), piority_(piority){}
Accept::~Accept(){}



//<<HTTPRequest>>
HTTPRequest::HTTPRequest() : instance_index_(global_index_++){}

HTTPRequest::~HTTPRequest(){}


//Setters
void HTTPRequest::setMet(const std::string  method){
    if (method == "GET")
        method_ = GET;
    else if (method == "POST")
        method_ = POST;
    else if (method == "DELETE")
        method_ = DELETE;
    else
        throw std::invalid_argument("Invalid HTTP method: " + method);
}

void HTTPRequest::setPath(const std::string& path){
    path_ = path;
}

void HTTPRequest::setVersion(const std::string& version){
    version_ = version;
}

void HTTPRequest::setHost(const std::string& host){
    host_ = host;

}

void HTTPRequest::setUserAgent(const std::string& agents){
    


    
}

void HTTPRequest::setAccept(const std::string& media_type){
    
}


void HTTPRequest::setAcceptLanguage(const std::string& languages){



}

void HTTPRequest::setConnection(const std::string& connection){


}

// void setCookie(const std::string& version)


void HTTPRequest::setAuthorization(const std::string& version){


}


void HTTPRequest::setContent(const std::string& content){



}

void HTTPRequest::setUnknown(const std::string& buffer){


    
}

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

const std::map<std::string, int>& HTTPRequest::getAcceptLanguage(){
    return accept_language_;
}

const bool HTTPRequest::getConnection(){
    return connection_;
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

//Parser

void HTTPRequest::parseRequestLine(const std::string& request_line){
    std::string method, path, version;
    
    std::istringstream stream(request_line);
    stream >> method >> path >> version;

    setMet(method);
    setPath(path);
    setVersion(version);
}

void HTTPRequest::parseRequestHeader(std::istringstream& stream){
    std::string line;
    size_t pos_begin;
    size_t pos_end;

    while (std::getline(stream, line))
    {
        //GET
        if (line.find("Host") != std::string::npos){
            if ((pos_begin = line.rfind(":")) != std::string::npos){
                pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
                std::string host = line.substr(pos_begin, std::string::npos);
                setHost(host);}
        } else if (line.find("Connection") != std::string::npos){
            if ((pos_begin = line.rfind(":")) != std::string::npos){
                pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
                std::string host = line.substr(pos_begin, std::string::npos);
                setConnection(host);}

        } else if (line.find("User-Agent") != std::string::npos){   
            if ((pos_begin = line.rfind(":")) != std::string::npos){
                pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
                std::string host = line.substr(pos_begin, std::string::npos);
                setHost(host);}


        } else if (line.find("Accept") != std::string::npos){
            if ((pos_begin = line.rfind(":")) != std::string::npos){
                pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
                std::string host = line.substr(pos_begin, std::string::npos);
                setHost(host);}


        } else if (line.find("Referer") != std::string::npos){
            if ((pos_begin = line.rfind(":")) != std::string::npos){
                pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
                std::string host = line.substr(pos_begin, std::string::npos);
                setHost(host);}


        } else if (line.find("Accept-Encoding") != std::string::npos){
            if ((pos_begin = line.rfind(":")) != std::string::npos){
                pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
                std::string host = line.substr(pos_begin, std::string::npos);
                setHost(host);}



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










// const char* ft_strstr(const char* haystack, const char* needle)
// {
//     if (!haystack)
//         return NULL;
//     if (!needle)
//         return haystack;
 
//     for (const char* p = haystack; *p; ++p)
//     {
//         const char* h = p;
//         const char* n = needle;
//         while (*h && *n && *h == *n)
//         {
//             h++;
//             n++;
//         }
//         if (!*n)
//             return p;        
//     }
//     return NULL;
// }


// int main(){
//     int server_fd, client_fd;
//     struct sockaddr_in address;
//     char buffer_cstr[BUFFER_SIZE];
//     socklen_t addlen = sizeof(address);

//     //Create socket
//     // server_fd = socket(AF_INET, SOCK_STREAM, 0);

//     int total = 0;

//     while (1) {
//         //it returns the number of bytes received, or -1 if an error occurred. In the 
//         // event of an error, errno is set to indicate the error.
//         // when a stream socker peer has per

//         int bytes = recv(client_fd, buffer_cstr + total, BUFFER_SIZE - 1 - total, 0);
//         if (bytes <= 0) break;
//         total += bytes;
//         buffer_cstr[total] = '\0';
//         if (ft_strstr(buffer_cstr,"\r\n\r\n")) break;
//     }
//     std::string buffer = buffer_cstr; 
//     i



// }
