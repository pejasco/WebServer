/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssottori <ssottori@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:19:25 by chuleung          #+#    #+#             */
/*   Updated: 2025/05/28 01:48:04 by ssottori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/HTTPResponse.hpp"
#include "../CGI/inc/receiveRequest.hpp"
#include "../CGI/inc/CgiHandler.hpp"

bool cgi_flag = false;

HTTPResponse::HTTPResponse(const HTTPRequest &inputRequest, ServerManager &serverManager, const std::string &serverIP) : currentRequest(inputRequest) {
	empty_line = "\r\n";
	defaultServer = serverManager.getServers().front();
	defaultLocation = defaultServer->getLocation().front();
	server = getCurrentServer(inputRequest, serverManager, serverIP);
	location = getCurrentLocation(inputRequest, *server);
	if (location == NULL && (inputRequest.getPath() == "/"))
		location = defaultServer->getLocation().front();
	switch (currentRequest.getMethod()) {
	case GET:
		if (checkMethod() != 200) {
			setErrorResponse(405);
			break;
		}
		setGetResponse();
		cgi_flag = false;
		break;
	case POST:
		if (checkMethod() != 200) {
			setErrorResponse(405);
			return;
		}
		setPostResponse();
		cgi_flag = false;
		break;
	case DELETE:
		if (checkMethod() != 200) {
			setErrorResponse(405);
			return;
		}
		setDeleteResponse();
		cgi_flag = false; // is it useful ?
		break;
	default:
		setErrorResponse(405);
		break;
	}	
};
HTTPResponse::~HTTPResponse() {
	// std::cout << "method destructor\n";
};

// SETTER

// GETTER
const std::string &HTTPResponse::getResponse() {
	// std::cout << "status_line: " << status_line << std::endl;
	// std::cout << "header: " << header << std::endl;
	// std::cout << "response header that is going to be sent:\n" << response;

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
	std::cerr << "[debugging] cgi_flag = " << cgi_flag << std::endl;
	//body_filename = "./CGI/cgi-bin/test.py"; //need to figure out how to set the correct root path for the cgi....maybe checkFile?

	//int status_code = checkFile();
	int status_code;
	if (cgi_flag) {
		body_filename = "./CGI/cgi-bin/test.py";  // TEMPORARY override for testing
		status_code = fileExists(body_filename) ? 200 : 404;
	}
	else
	{
		status_code = checkFile();
	}

	if (status_code == 200) { 
		prepareStatusLine(status_code);
		prepareHeader();
		if (cgi_flag)
			CGI_Body();
		headerResponse();
	} else {
		setErrorResponse(status_code);
	}
	// if (cgi_flag) {
	// 	CGI_Body(); // <- run the actual CGI handler
	// 	return; // <- do NOT check or open the file
	// }

	// int status_code = checkFile(); //fallback for static files
	// if (status_code == 200) {
	// 	prepareStatusLine(status_code);
	// 	prepareHeader();
	// 	headerResponse();
	// } else {
	// 	setErrorResponse(status_code);
	// }


}

void HTTPResponse::setPostResponse() {
	// Switch or if statement to see if it is an upload request (CD --> filename)
		// IF fielname exist --> create a file with a filenema define in CD and fill it with the file content of cd and save it under upload
	// 	int status_code = CreateTheUploadFIle()
	// 	if (status_code == 200) { 
	// 	prepareStatusLine(status_code);
	// 	prepareHeader();
	// 	// if (cgi_flag) -- Check with Shally if we need that
	// 	// 	CGI_Body();
	// 	headerResponse();
	// 	bodyResponse(); // html page that going to show "File successfully uploaded + redirect them to the server homepage"
	// } else {
	// 	setErrorResponse(status_code);
	// }
	;
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

void HTTPResponse::setErrorResponse(int errorCode) {
	body_filename = "documents/errors/" + convertToStr(errorCode) + ".html";
	if (fileExists(body_filename)) {
		prepareStatusLine(errorCode);
		content_length = calculateFileSize(body_filename);
		header = "Content-Type: text/html; charset=UTF-8\r\nContent_Length: " + convertToStr(content_length) + "\r\n";
		response = status_line + header + empty_line;
	} else {
		std::cerr << "error\n";
		status_line = currentRequest.getVersion() + " 500 Internal server error\r\n";
		header = "Content-Type: text/html; charset=UTF-8\r\nContent_Length: 131\r\n";
		body = "<!DOCTYPE html><html><head><title>500 Error</title></head><body><h1>500 Internal Server Error</h1><p>The server encountered an error and could not complete your request.</p></body></html>";
		response = status_line + header + empty_line + body;
	}
}

//Checking if the file requested exist and if it is possible to read it 
int HTTPResponse::checkFile() {
	std::string defaultPath = defaultLocation->getRoot();
	defaultPath.erase(std::remove(defaultPath.begin(), defaultPath.end(), '/'), defaultPath.end());
	if (currentRequest.getPath() == "/") {
		body_filename = defaultPath + "/" + defaultLocation->getIndex();
		std::cout << "body_filename: " << body_filename << std::endl;
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
		body_filename = defaultPath;
		std::cout << BOLD YELLOW "body_filename: " << body_filename << RESET << std::endl;
		if (currentRequest.getPath().find(".") == std::string::npos) {
			body_filename += location->getRoot() + "/" + location->getIndex();
			std::cout << "adding the file from config\n";
		} else {
			body_filename += currentRequest.getPath();
			std::cout << "adding the file from request\n";
		}
		std::cout << BOLD YELLOW "New body_filename: " << body_filename << RESET << std::endl;
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

int HTTPResponse::checkMethod() {
	std::vector<MET>::iterator begM;
	std::vector<MET>::iterator endM;
	if (location->getMethod().empty()) {
		Location &defaultLocation = *defaultServer->getLocation().front();
		begM = defaultLocation.getMethod().begin();
		endM = defaultLocation.getMethod().end();
			for (; begM != endM; ++begM) {
				if (*begM == currentRequest.getMethod())
					return 200;
			}
			return 405;
	} else {
		begM = location->getMethod().begin();
		endM = location->getMethod().end();
		for (; begM != endM; ++begM) {
			if (*begM == currentRequest.getMethod())
				return 200;
			}
		return 405;
	}
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
	// std::cout << "INSIDE PREPARE HEARDER\n";
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

void HTTPResponse::CGI_Body() //getting httpRequest data and sending it to CGI and storing it in RequestData
{
	std::cerr << "----- [CGI] we are in CGI_Body" << std::endl;
	std::string path = currentRequest.getPath();  // e.g. "/cgi-bin/test.py"
	std::string scriptPath = "." + path;

	RequestData data;
	data.setMethod(currentRequest.getMethodAsStr());
	data.setPath(currentRequest.getPath());
	std::cerr << "[CGI] method : " << data.getMethod() << std::endl;
	std::cerr << "[CGI] - path saved: " << data.getPath() << std::endl;

	// data.setQueryString(currentRequest.getQueryStr());
	// data.setHeaders(currentRequest.getHeaders());
	// data.setBody(currentRequest.getRawBody());

	// CgiHandler handler(data, scriptPath); //new object
	// std::string cgiOutput = handler.run();

	// body = cgiOutput;
	// // overwrite response with final headers + body
	// response += body;
}

