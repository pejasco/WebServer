/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chuleung <chuleung@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:19:03 by chuleung          #+#    #+#             */
/*   Updated: 2025/05/07 15:53:38 by chuleung         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ServerManager.hpp"
//#include "HTTPRequest.hpp"
#include "Content.hpp"


// // Construct response
// std::string response = "HTTP/1.1 200 OK\r\n";
// response += "Content-Type: text/html\r\n";
// response += "Content-Length: " + std::to_string(body.length()) + "\r\n";
// response += "\r\n";  // End of headers
// response += body;    // Response body

// // Send it over the socket
// send(client_fd, response.c_str(), response.length(), 0);




class HTTPResponse {

        public:
                HTTPResponse();
                ~HTTPResponse();

        private:
                static int global_index_; //dun know if it is useful
		int instance_index_;
                setPath()
                
                
                






};

