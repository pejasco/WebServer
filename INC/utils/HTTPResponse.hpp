/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:19:03 by chuleung          #+#    #+#             */
/*   Updated: 2025/05/30 12:17:31 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "HTTPRequest_copy.hpp"
#include "ServerManager.hpp"
// // Construct response
// std::string response = "HTTP/1.1 200 OK\r\n"; --> status line to send following the header following an empty blank and the reponse Body (content like HTML)
// response += "Content-Type: text/html\r\n";
// response += "Content-Length: " + std::to_string(body.length()) + "\r\n";
// response += "\r\n";  // End of headers
// response += body;    // Response body

// // Send it over the socket
// send(client_fd, response.c_str(), response.length(), 0);
class ServerManager;
class Location;
class Server;

class HTTPResponse {
	public:
		HTTPResponse(const HTTPRequest &inputRequest, ServerManager &serverManager, const std::string &serverIP); //get information like the path, method and version via HTTPrequest class 
		~HTTPResponse();
		
		//SETTER

		//GETTER
		const std::string &getResponse();
		std::string &getBodyFilename();
			
		//METHOD
		void setGetResponse();
		// void setPostResponse();
		void setDeleteResponse();
		void setErrorResponse(int errorCode);
		int checkFile();
		int checkMethod() ;
		void prepareStatusLine(int status_code);
		int prepareHeader();
		void headerResponse();
		void CGI_Body();
		void autoIndexRequest();
		// int createUploadFile(std::string& location, Content& content);
		// int checkDirectory(std::string& location);
		// void makePostResponse(ContentDisposition_ &cd);
		
	
	private:
		//PREPARING RESPONSE
		HTTPRequest currentRequest;
		Server *server;
		Location *location;
		Server *defaultServer;
		Location *defaultLocation;
		//RESPONSE CONTENT
		std::string response;
		std::string status_line;
		std::string body;
		std::string header;
		std::string empty_line;
		
		//STATUS_LINE
		
		//HEADER_FIELDS
		std::string content_type;
		int content_length;
		std::string body_filename;
		// bool keep_alive;
		//Adding time ?
		
		//BODY
		
	
		// static int global_index_; //dun know if it is useful
		// int instance_index_;

};

int structureInfo(const std::string &dir_path, const std::string &sub_dir);

#endif
