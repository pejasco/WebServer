/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chuleung <chuleung@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:19:25 by chuleung          #+#    #+#             */
/*   Updated: 2025/05/29 16:50:55 by chuleung         ###   ########.fr       */
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
	std::cout << BOLD UNDERLINE GREEN "\n###### ENTERING SERVER//LOCATION DEBUUGING ######\n\n" RESET;
	server = getCurrentServer(inputRequest, serverManager, serverIP);
	location = getCurrentLocation(inputRequest, *server);
	if (location == NULL && (inputRequest.getPath() == "/"))
		location = defaultServer->getLocation().front();
	std::cout << BOLD UNDERLINE GREEN "\n###### LEAVING SERVER//LOCATION DEBUUGING ######\n\n" RESET;
	switch (currentRequest.getMethod()) {
	case GET:
		if (checkMethod() != 200) {
			setErrorResponse(405);
			return ;
		}
		setGetResponse();
		cgi_flag = false;
		break;
	case POST:
		if (checkMethod() != 200) {
			setErrorResponse(405);
			return;
		}
		setPostResponse(); // main > SM::serverMonitoring() > SM::existingClientConnection > HTTPRequest currentRequest > HTTPResponse constructor(currentRequest, *the pointer of serverManger, string serverIP)
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
	// std::cerr << "[debugging] cgi_flag = " << cgi_flag << std::endl;
	//body_filename = "./CGI/cgi-bin/test.py"; //need to figure out how to set the correct root path for the cgi....maybe checkFile?

	//int status_code = checkFile();
	int status_code;
	if (cgi_flag) {
		body_filename = "./CGI/cgi-bin/test.py";  // TEMPORARY override for testing
		status_code = fileExists(body_filename) ? 200 : 404;
	} else {
		status_code = checkFile();
		std::cout << "status_code of checkFIle in GET: " << status_code << std::endl;
	}

	if (status_code == 200) {
		std::cout << "GET requested an existing file --> proceed to response pre\n";
		prepareStatusLine(status_code);
		prepareHeader();
		if (cgi_flag)
			CGI_Body();
		headerResponse();
	} else {
		std::cout << "GET requested a file that doesn't exist --> proceed to error response pre\n";
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

// int HTTPResponse::checkDirectory(std::string& location){
// 	DIR* dir = opendir(location.c_str());
// 	if (dir == NULL)
// 	{
// 		std::cout << "Error: " << strerror(errno) << std::endl;
// 		return 404;
// 	}
// 	closedir(dir);
// 	return 200;
// 	(void)location;
// }


int HTTPResponse::checkDirectory(std::string& location){
	struct stat st;

	 if (stat(location.c_str(), &st) == 0){
		if (S_ISDIR(st.st_mode)){
			return 200;
		} else {
			std::cout << "Error: Path exits but is not a directory" << '\n';
			return 500; //conflict
		}
	 } else {
		if (errno == ENOENT) {
			std::cout << "Error: Directory does not exist" << std::endl;
			return 500; //not found
		} else {
			std::cout << "Error: " << strerror(errno) << std::endl;
			return 500; //internal error
		}
	 }
}


int HTTPResponse::createUploadFile(std::string& location, Content& content){
	// check if the directory exists
	int status_code = checkDirectory(location);
	if (status_code != 200)
		return status_code; //if it doesnt exist
	//std::string filepath = location + "/" + content.filename_;
	std::ofstream file(filepath.c_str(), std::ios::binary);
	if (!file.is_open())
		return 500;
	file.write(cd.file_content_.c_str(), cd.file_content_.length());
	
	if (file.fail()){
		file.close();
		return 500;
	}
	
	file.close();
	return 200;
	
}


// void HTTPResponse::setPostResponse(std::string& location, Content& content) {
// 	// Switch or if statement to see if it is an upload request (CD --> filename)
// 		// IF fielname exist --> create a file with a filenema define in CD and fill it with the file content of cd and save it under upload		
		
// 		int status_code = 400; //default to be bad
// 		if (!(cd.filename_).empty())
// 			status_code = createUploadFile(location, cd);
// 		if (status_code == 200) { 
// 			prepareStatusLine(status_code);
			
// 			body = "<!DOCTYPE html><html><head><title>Success</title><meta http-equiv=\"refresh\" content=\"3;url=/\"></head><body><h1>Upload Successful!!!!!!!</h1></body></html>";
// 		// if (cgi_flag) -- Check with Shally if we need that
// 		// 	CGI_Body();
// 			content_length = body.length();
// 			header = "Content-Type: text/html; charset=UTF-8\r\nContent-Length: " + convertToStr(content_length) + "\r\n";
// 			response = status_line + header + empty_line + body;
	
// 		} else {
// 			setErrorResponse(status_code);
// 	}
// }

void HTTPResponse::setPostResponse() {
	// Switch or if statement to see if it is an upload request (CD --> filename)
		// IF fielname exist --> create a file with a filenema define in CD and fill it with the file content of cd and save it under upload		
		
		int status_code = 400; //default to be bad
		std::string path = currentRequest.getPath();
		Content cd = currentRequest.getContent();
		if (!path.empty()) //for upload only && not cgi 
			status_code = createUploadFile(path, cd);
		if (status_code == 200) { 
			prepareStatusLine(status_code);
			
			body = "<!DOCTYPE html><html><head><title>Success</title><meta http-equiv=\"refresh\" content=\"3;url=/\"></head><body><h1>Upload Successful!!!!!!!</h1></body></html>";
		// if (cgi_flag) -- Check with Shally if we need that
		// 	CGI_Body();
			content_length = body.length();
			header = "Content-Type: text/html; charset=UTF-8\r\nContent-Length: " + convertToStr(content_length) + "\r\n";
			response = status_line + header + empty_line + body;
	
		} else {
			setErrorResponse(status_code);
	}
}



// void HTTPResponse::setPostResponse() {
//     std::string location = "uploads";
    
//     ContentDisposition_ cd = currentRequest.getContentDisposition();
    
//     setPostResponse(location, cd);
// }


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
	if (!server->getErrorList().empty()) {
		std::cout << "Error list exist inside current server\n";
		std::map<int, std::string>::iterator begEr = server->getErrorList().begin();
		std::map<int, std::string>::iterator endEr = server->getErrorList().end();
		for (; begEr != endEr; ++begEr) {
			if (begEr->first == errorCode)
				body_filename = begEr->second;
		}
		if (begEr == endEr)
			body_filename = server->getErrorDir() + convertToStr(errorCode) + ".html";
	} else 
		body_filename = defaultServer->getErrorDir() + convertToStr(errorCode) + ".html";
	std::cout << "body_filename found via map lookup: " << body_filename << std::endl;
	if (fileExists(body_filename)) {
		std::cout << "error_file exist " << body_filename << std::endl;
		prepareStatusLine(errorCode);
		content_length = calculateFileSize(body_filename);
		header = "Content-Type: text/html; charset=UTF-8\r\nContent_Length: " + convertToStr(content_length) + "\r\nConnection: close\r\n";
		response = status_line + header + empty_line;
	} else {
		std::cout << "error: error-file " << body_filename << " doesn't exist\n";
		body = "<!DOCTYPE html><html><head><title>500 Error</title></head><body><h1>500 Internal Server Error</h1><p>The server encountered an error and could not complete your request.</p></body></html>";
		status_line = currentRequest.getVersion() + " 500 Internal server error\r\n";
		header = "Content-Type: text/html; charset=UTF-8\r\nContent_Length: " + convertToStr(body.size()) + "\r\nConnection: close\r\n";
		response = status_line + header + empty_line + body;
		body_filename = "";
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
	std::cout << BOLD UNDERLINE RED "\n###### LEAVING URL REFORMAT DEBUUGING ######\n\n" RESET;
}

int HTTPResponse::checkMethod() {
	std::vector<MET>::iterator begM;
	std::vector<MET>::iterator endM;
	if (location->getMethod().empty()) {
		Location &defaultLocation = *defaultServer->getLocation().front();
		begM = defaultLocation.getMethod().begin();
		endM = defaultLocation.getMethod().end();
			for (; begM != endM; ++begM) {
				if (*begM == currentRequest.getMethod()) {
					std::cout << "return value of checkMethod: 200\n";
					return 200;
				}
			}
			std::cout << "return value of checkMethod: 405\n";
			return 405;
	} else {
		begM = location->getMethod().begin();
		endM = location->getMethod().end();
		for (; begM != endM; ++begM) {
				if (*begM == currentRequest.getMethod()) {
					std::cout << "return value of checkMethod: 200\n";
					return 200;
				}
			}
		std::cout << "return value of checkMethod: 405\n";
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
	const std::string CGI_ROOT = "./CGI/cgi-bin";
	std::string uri = currentRequest.getPath();  // e.G /cgi-bin/test.py
	std::string prefix = "/cgi-bin";
	std::string scriptPath;

	if (uri.find(prefix) == 0) {
		std::string relativePath = uri.substr(prefix.size());     // e.g. "/test.py"
		scriptPath = CGI_ROOT + relativePath;         // e.g. "./CGI/cgi-bin/test.py"
		std::cerr << "[CGI DEBUG] full path to script: " << scriptPath << std::endl;
	} else {
		setErrorResponse(404);  // unexpected URI
		return;
	}

	RequestData data;
	data.setMethod(currentRequest.getMethodAsStr());
	data.setPath(scriptPath); //for SCRIPT_FILENAME and SCRIPT_NAME
	data.setQueryString(currentRequest.getQueryStr());
	data.setHeaders(currentRequest.getHeaders());
	data.setBody(currentRequest.getRawBody());
	std::cerr << "[CGI] method : " << data.getMethod() << std::endl;
	std::cerr << "[CGI] - path saved: " << data.getPath() << std::endl;
	std::cerr << "[CGI] - query str: " << data.getQueryString() << std::endl;
	std::cerr << "[CGI] - body saved: " << data.getBody() << std::endl;

	CgiHandler handler(data, scriptPath); //new object
	std::string cgiOutput = handler.run();

	size_t headerEnd = cgiOutput.find("\r\n\r\n");
	if (headerEnd != std::string::npos) {
		std::string headers = cgiOutput.substr(0, headerEnd);
		std::string body = cgiOutput.substr(headerEnd + 4);

		if (headers.find("Content-Type:") == std::string::npos)
			headers = "Content-Type: text/html\r\n" + headers;

		response = "HTTP/1.1 200 OK\r\n" + headers + "\r\n\r\n" + body;
	} else {
		// fallback (not ideal, but prevents broken response)
		response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + cgiOutput;
	}

	//std::cerr << "[CGI] Raw CGI output:\n" << cgiOutput << "\n";
	//response = cgiOutput;
}

