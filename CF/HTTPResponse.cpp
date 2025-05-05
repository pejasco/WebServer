/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chuleung <chuleung@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:19:25 by chuleung          #+#    #+#             */
/*   Updated: 2025/05/05 12:19:26 by chuleung         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 4096


const char* ft_strstr(const char* haystack, const char* needle)
{
    if (!haystack)
        return NULL;
    if (!needle)
        return haystack;
 
    for (const char* p = haystack; *p; ++p)
    {
        const char* h = p;
        const char* n = needle;
        while (*h && *n && *h == *n)
        {
            h++;
            n++;
        }
        if (!*n)
            return p;        
    }
    return NULL;
}


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
    



}
