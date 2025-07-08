/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:19:03 by chuleung          #+#    #+#             */
/*   Updated: 2025/07/08 15:23:33 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "HTTPRequest.hpp"
// #include "ServerManager.hpp"

class ServerManager;
class Location;
class Server;

class HTTPResponse {
	public:
		// HTTPResponse();
		HTTPResponse(HTTPRequest &input_request, Server *server_requested, Location *location_requested, Server *master_server, ServerManager *server_manager, int error_flag, int error_code); //get information like the path, method and version via HTTPrequest class 
		~HTTPResponse();
		
		//SETTER
		void setAutoIndex(bool val);

		//GETTER
		const std::string &getResponse();
		std::string &getBodyFilename();
		bool isAutoIndex();
		int getStatusCode();
			
		//METHOD
		void setGetResponse();
		void setPostResponse();
		void setDeleteResponse();
		void setErrorResponse(int error_code);
		int checkFile();
		int checkMethod() ;
		void prepareStatusLine();
		int prepareHeader();
		void headerResponse();
		void CGI_Body();
		void autoIndexRequest();
		int createUploadFile(std::string& upload_dir, Content& content);
		int checkDirectory(std::string& location);
		//void makePostResponse(ContentDisposition_ &cd);
		bool isReady() const;
		bool checkRedirection();
		void draftErrorResponse();
		void draftRedirectResponse();
	
	private:
		//PREPARING RESPONSE
		ServerManager *server_manager_;
		int status_code_;
		HTTPRequest &current_request_;
		Server *server_;
		Location *location_;
		// Server *master_server_;
		Server *default_server_;
		Location *default_location_;
		Server *master_server_;
		//RESPONSE CONTENT
		std::string response_;
		std::string status_line_;
		std::string body_msg_;
		std::string header_;
		std::string empty_line_;
		
		//STATUS_LINE
		bool is_autoindex_; 
		
		//HEADER_FIELDS
		std::string content_type_;
		int content_length_;
		std::string body_filename_;
		// bool keep_alive;
		//Adding time ?
		
		//BODY
		
	
		// static int global_index_; //dun know if it is useful
		// int instance_index_;
		bool _response_ready_;


};

int structureInfo(const std::string &dir_path, const std::string &sub_dir, const std::string &default_folder);

#endif
