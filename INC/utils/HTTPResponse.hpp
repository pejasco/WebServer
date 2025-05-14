/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:19:03 by chuleung          #+#    #+#             */
/*   Updated: 2025/05/14 14:34:06 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ServerManager.hpp"
#include "HTTPRequest.hpp"
#include "Content.hpp"


// // Construct response
// std::string response = "HTTP/1.1 200 OK\r\n"; --> status line to send following the header following an empty blank and the reponse Body (content like HTML)
// response += "Content-Type: text/html\r\n";
// response += "Content-Length: " + std::to_string(body.length()) + "\r\n";
// response += "\r\n";  // End of headers
// response += body;    // Response body

// // Send it over the socket
// send(client_fd, response.c_str(), response.length(), 0);

enum Status {
	OK = 200,
	NO_CONTENT = 204,
	BAD_REQUEST = 400,
	UNAUTHORIZE = 401,
	NOT_FOUND = 404,
	MET_NOT_ALLOW = 405,
	UNSUPPORTED_MEDIA = 415,
	INTERNAL_SERVER_ERROR = 500,
	BAD_GATEWAY = 502, // not sure if this code is useful as our server is not a proxy
};

class HTTPResponse : public HTTPRequest {
	public:
		HTTPResponse(); //get information like the path, method and version via HTTPrequest class 
		~HTTPResponse();
		
		//SETTER

		//GETTER
		const std::string &getResponse(); 
			
		//METHOD
		void setGetResponse();
		void setPostResponse();
		void setDeleteResponse();
		void setErrorResponse();
		
	private:
		//PREPARING RESPONSE
	
		//RESPONSE CONTENT
		std::string response;
		std::string status_line;
		std::string content;
		std::string header;
		std::string empty_line;
		
		//STATUS_LINE
		Status code;
		
		//HEADER_FIELDS
		std::string content_type;
		size_t content_length;
		bool keep_alive;
		//Adding time ?
		
		//BODY
		
	
		// static int global_index_; //dun know if it is useful
		// int instance_index_;

};

