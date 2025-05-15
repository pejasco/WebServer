/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:19:25 by chuleung          #+#    #+#             */
/*   Updated: 2025/05/15 16:42:26 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/HTTPResponse.hpp"

bool cgi_flag = false;

HTTPResponse::HTTPResponse(const HTTPRequest &inputRequest) : currentRequest(inputRequest) {
	empty_line = "\r\n";
	switch (currentRequest.getMethod()) {
	case GET:
		setGetResponse();
		cgi_flag = false;
		break;
	case POST:
		setPostResponse();
		cgi_flag = false;
		break;
	case DELETE:
		setDeleteResponse();
		cgi_flag = false; // is it useful ?
		break;
	default:
		setErrorResponse();
		break;
	}	
};
HTTPResponse::~HTTPResponse() {
	std::cout << "method destructor\n";
};

// SETTER

// GETTER
const std::string &HTTPResponse::getResponse() {
	// std::cout << "status_line: " << status_line << std::endl;
	// std::cout << "header: " << header << std::endl;
	std::cout << "response header that is going to be sent:\n" << response;

	return response;
};

std::string &HTTPResponse::getBodyFilename() {
	return body_filename;
}

//METHOD
void HTTPResponse::setGetResponse() {
	//1st -- check if the path request by the user exist
	//2nd -- check if the file exist and readable (not already open, with correct permission (not sure if we need to set it up)) 
	cgi_flag = currentRequest.getCGIFlag();
	int status_code = checkFile();
	if (status_code == 200) {
		//3rdb -- if file is good, prepare response with 200 ok and content of file 
		prepareStatusLine(status_code);
		prepareHeader();
		if (cgi_flag)
			CGI_Body();
		headerResponse();
	} else {
		//3rda -- if file has error, prepare an error request (404, 400, 403, etc...)
	}

}

void HTTPResponse::setPostResponse() {
	
}

void HTTPResponse::setDeleteResponse() {
	body_filename = "documents" + currentRequest.getPath();
	if (fileExists(body_filename)) {
		if (!std::remove(body_filename.c_str()))
			response = currentRequest.getVersion() + " 204 No Content\r\n\r\n";
		else
			setErrorResponse(500); // file exist but not closable so internal error ?
	} else {
		setErrorResponse(404); // file not found as return false to fileExist()? 
	}
}

void HTTPResponse::setErrorResponse() {
	//if no GET, POST or DELETE, then it is a not allow method or bad request and send the error 
		// - 405
		// - 400 ?
}

//Checking if the file requested exist and if it is possible to read it 
int HTTPResponse::checkFile() {
	if (currentRequest.getPath() == "/") {
		body_filename = "documents/index.html";
		std::ifstream body_file(body_filename.c_str(), std::ios::binary);
		if (body_file.is_open()) {
			body_file.close();
			return 200;
		}
		else {
			body_file.close();
			std::cout << "Error: " << strerror(errno) << std::endl;
			return 500;
		}
	} else {
		body_filename = "documents" + currentRequest.getPath();
		if (fileExists(body_filename)){
			std::ifstream body_file(body_filename.c_str(), std::ios::binary);
			if (body_file.is_open()) {
				body_file.close(); 
				return 200;
			} else {
				body_file.close();
				std::cout << "Error: " << strerror(errno) << std::endl;
				return 500;	
			}
		} else
			return 404;
	}
	return 500;	
}


/*
HTTP/1.1 200 OK\r\n
Content-Type: text/html; charset=UTF-8\r\n
Content-Length: 145\r\n
Date: Wed, 14 May 2025 12:00:00 GMT\r\n ???
Connection: keep-alive\r\n
\r\n
<!DOCTYPE html>\r\n
<html>\r\n
<head>\r\n
    <title>My Page</title>\r\n
</head>\r\n
<body>\r\n
    <h1>Welcome to my server</h1>\r\n
    <p>This is a sample page.</p>\r\n
</body>\r\n
</html>
*/

void HTTPResponse::prepareStatusLine(int status_code) {
	status_line += currentRequest.getVersion();
	status_line += " " + convertToStr(status_code);
	status_line += " " + getStatusStr(status_code);
	status_line += "\r\n";
	
	/****DEBUGGING***/
	// std::cout << "status_line: " << status_line << std::endl;
}


int HTTPResponse::prepareHeader() {
	//Probably to adapt so it can change for POST and DELETE as the content can be different
	std::cout << "INSIDE PREPARE HEARDER\n";
	if (currentRequest.getPath().find(".") != std::string::npos) {
		size_t pos;
		if ((pos = currentRequest.getPath().rfind(".")) != std::string::npos)
			content_type = "Content-Type: " + getContentType(currentRequest.getPath().substr(pos)) + "\r\n";
	}
	content_length = calculateFileSize(body_filename);
	if (content_length < 0) {
		std::cout << "Error: Content-Length is -1\n";
		return 500;
	}
	header = content_type + "Content-Length: " + convertToStr(content_length) + "\r\n";
	/****DEBUGGING***/
	// std::cout << "HEADER = \n" << header << std::endl;
	return 200;
}

void HTTPResponse::headerResponse() {
	response = status_line + header + empty_line;

	/****DEBUGGING***/
	// std::cout << "response: " << response << std::endl;
}

void HTTPResponse::CGI_Body() {
	
	return;
}

bool HTTPResponse::deleteFile() {
	
}