/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chuleung <chuleung@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:19:15 by chuleung          #+#    #+#             */
/*   Updated: 2025/05/06 18:50:21 by chuleung         ###   ########.fr       */
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

int HTTPRequest::global_index_ = 0;

HTTPRequest::HTTPRequest() : instance_index_(global_index_++){}

HTTPRequest::~HTTPRequest(){}


//Setters
void HTTPRequest::setMet(const MET met_code)
{
    method_ = met_code;
}

void HTTPRequest::setPath(const std::string& path)
{
    path_ = path;
}

void HTTPRequest::setVersion(const std::string& version)
{
    version_ = version;
}

void HTTPRequest::setHost(const std::string& host)
{
    host_ = host;

}

void HTTPRequest::setUserAgent(const std::string& agent)
{
    user_agent_ = agent;

}

void HTTPRequest::setAccept(const std::string& media_type)
{
    accept_ = med
}


void HTTPRequest::setAcceptLanguage(const std::string& languages)
{



}
void HTTPRequest::setConnection(const std::string& connection)
{


}
// void setCookie(const std::string& version)


void HTTPRequest::setAuthorization(const std::string& version)
{


}


void HTTPRequest::setContent(const std::string& content)
{



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


int main(){
    int server_fd, client_fd;
    struct sockaddr_in address;
    char buffer_cstr[BUFFER_SIZE];
    socklen_t addlen = sizeof(address);

    //Create socket
    // server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int total = 0;

    while (1) {
        //it returns the number of bytes received, or -1 if an error occurred. In the 
        // event of an error, errno is set to indicate the error.
        // when a stream socker peer has per

        int bytes = recv(client_fd, buffer_cstr + total, BUFFER_SIZE - 1 - total, 0);
        if (bytes <= 0) break;
        total += bytes;
        buffer_cstr[total] = '\0';
        if (ft_strstr(buffer_cstr,"\r\n\r\n")) break;
    }
    std::string buffer = buffer_cstr; 
    i



}
