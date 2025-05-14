/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:19:25 by chuleung          #+#    #+#             */
/*   Updated: 2025/05/14 17:53:14 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/HTTPResponse.hpp"

bool flag = false;

HTTPResponse::HTTPResponse(const HTTPRequest &inputRequest) : currentRequest(inputRequest) {
	empty_line = "\r\n";
	switch (currentRequest.getMethod()) {
	case GET:
		setGetResponse();
		break;
	case POST:
		flag = true;
		setPostResponse();
		flag = false;
		break;
	case DELETE:
		setDeleteResponse();
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
	std::cout << "status_line: " << status_line << std::endl;
	std::cout << "header: " << header << std::endl;
	std::cout << "response: " << response << std::endl;

	return response;
};

//METHOD
void HTTPResponse::setGetResponse() {
	//1st -- check if the path request by the user exist
	//2nd -- check if the file exist and readable (not already open, with correct permission (not sure if we need to set it up)) 
	int status_code = checkFile();
	if (status_code == 200) {
		//3rdb -- if file is good, prepare response with 200 ok and content of file 
		prepareStatusLine(status_code);
		prepareHeader();
		if (flag)
			prepareBody();
		completeResponse();
	} else {
		//3rda -- if file has error, prepare an error request (404, 400, 403, etc...)
	}

}

void HTTPResponse::setPostResponse() {
	
}

void HTTPResponse::setDeleteResponse() {
	
}

void HTTPResponse::setErrorResponse() {
	//if no GET, POST or DELETE, then it is a not allow method or bad request and send the error 
		// - 405
		// - 400 ?
}

int HTTPResponse::checkFile() {
	if (currentRequest.getPath() == "/") {
		std::fstream index;
		index.open("documents/index.html", std::fstream::in);
		if (index.is_open()) {
			std::stringstream buffer;
			buffer << index.rdbuf();
			body = buffer.str(); 
			return 200;
		}
		else 
			return 500;
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
	status_line += " " + convertStr(status_code);
	status_line += " " + getStatusStr(status_code);
	status_line += "\r\n";
	std::cout << "status_line: " << status_line << std::endl;
}


void HTTPResponse::prepareHeader() {
	//Probably to adapt so it can change for POST and DELETE as the content can be different
	content_type = "Content-Type: text/html; charset=UTF-8\r\n";
	content_length = body.size();
	header = content_type + "Content-Length: " + convertStr(content_length) + "\r\n";
	std::cout << "header: " << header << std::endl;
}

void HTTPResponse::completeResponse() {
	response = status_line + header + empty_line + body;
	std::cout << "response: " << response << std::endl;
}

void HTTPResponse::prepareBody() {
	return;
}