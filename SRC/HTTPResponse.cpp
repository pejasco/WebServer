/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:19:25 by chuleung          #+#    #+#             */
/*   Updated: 2025/06/17 18:26:51 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/HTTPResponse.hpp"
#include "../CGI/inc/receiveRequest.hpp"
#include "../CGI/inc/CgiHandler.hpp"

bool cgi_flag = false;

HTTPResponse::HTTPResponse(const HTTPRequest &input_request, Server *default_server, Server *server_requested, Location *location_requested, int error_flag) : 
current_request_(input_request), server_(server_requested), location_(location_requested), default_server_(default_server), default_location_(default_server->getLocationsList().front()), empty_line_("\r\n"), is_autoindex_(false), _response_ready_(false) {
	DEBUG_PRINT(BOLD WHITE "\n\n---------------------\n---------------------\nPARSE RESPONSE STARTED\n---------------------\n---------------------\n" RESET);
	if (error_flag > 0) {
		setErrorResponse(error_flag);
		return ;
	}
	if (checkRedirection()) {
		DEBUG_PRINT("checkRedirection() returned true");
		return ;
	}
	switch (current_request_.getMethod()) {
	case GET:
		DEBUG_PRINT("Method called: GET");
		if (checkMethod() != 200) {
			setErrorResponse(405);
			DEBUG_PRINT(BOLD WHITE "\n\n---------------------\n---------------------\nPARSE REQUEST EXITED\n---------------------\n---------------------\n" RESET);
			return ;
		}
		setGetResponse();
		cgi_flag = false;
		break;
	case POST:
		DEBUG_PRINT("Method called: POST");
		if (checkMethod() != 200) {
			setErrorResponse(405);
			DEBUG_PRINT(BOLD WHITE "\n\n---------------------\n---------------------\nPARSE REQUEST EXITED\n---------------------\n---------------------\n" RESET);
			return;
		}
		setPostResponse();
		// SM got a array of server > setPostResponse(); // main > SM::serverMonitoring() > SM::existingClientConnection > HTTPRequest current_request_ > HTTPResponse constructor(current_request_, *the pointer of serverManger, string serverIP)
		cgi_flag = false;
		break;
	case DELETE:
		DEBUG_PRINT("Method called: DELETE");
		if (checkMethod() != 200) {
			setErrorResponse(405);
			DEBUG_PRINT(BOLD WHITE "\n\n---------------------\n---------------------\nPARSE REQUEST EXITED\n---------------------\n---------------------\n" RESET);
			return;
		}
		setDeleteResponse();
		cgi_flag = false; // is it useful ?
		break;
	default:
		DEBUG_PRINT("Method called: UNKNOWN");
		setErrorResponse(405);
		DEBUG_PRINT(BOLD WHITE "\n\n---------------------\n---------------------\nPARSE REQUEST EXITED\n---------------------\n---------------------\n" RESET);
		break;
	}
};

HTTPResponse::~HTTPResponse() {
	// std::cout << "method destructor\n";
};

// SETTER
void HTTPResponse::setAutoIndex(bool newValue) {
	is_autoindex_ = newValue;
};


// GETTER
const std::string &HTTPResponse::getResponse() {
	return response_;
};

bool HTTPResponse::isReady() const {
	return _response_ready_;
}


std::string &HTTPResponse::getBodyFilename() {
	return body_filename_;
}

bool HTTPResponse::isAutoIndex() {
	return is_autoindex_;
}

//METHOD

bool HTTPResponse::checkRedirection() {
	if (!location_->getRedirectURL().empty()) {
		DEBUG_PRINT("location: " << location_ << " has been identified as redirection");
		setErrorResponse(location_->getRedirectCode());
		return true;
	}
	return false;
}

void HTTPResponse::setGetResponse() {
	DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET GET RESPONSE CALLED" RESET);
	//1st -- check if the path request by the user exist
	//2nd -- check if the file exist and readable (not already open, with correct permission (not sure if we need to set it up)) 
	cgi_flag = current_request_.getCGIFlag();
	DEBUG_PRINT("is current request a CGI: " << cgi_flag);

	int status_code = checkFile();
	cgi_flag = location_->isCGI();
	DEBUG_PRINT("is location allowing CGI: " << cgi_flag);
	if (status_code == 200) {
		if (cgi_flag) {
			DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET GET RESPONSE EXITED" RESET);
			CGI_Body(); // Run CGI handler, which builds full HTTP response
			_response_ready_ = true;
			return ;
		} else {
			prepareStatusLine(status_code);
			prepareHeader();
			headerResponse();
		}
	}
	else
	{
		DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET GET RESPONSE EXITED" RESET);
		setErrorResponse(status_code);
	}
	DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET GET RESPONSE EXITED" RESET);
}

int HTTPResponse::checkDirectory(std::string& location){
	struct stat st;
	DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK DIRECTORY CALLED" RESET);
	DEBUG_PRINT("location in checkDirectory: " << location);
	if (stat(location.c_str(), &st) == 0){
		if (S_ISDIR(st.st_mode)){
			DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK DIRECTORY EXITED" RESET);
			return 200;
		} else {
			DEBUG_PRINT("Path exist but not a directory, return 404");
			DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK DIRECTORY EXITED" RESET);
			return 404; //conflict
		}
	} else {
		if (errno == ENOENT) {
			DEBUG_PRINT("Directory doesn't exist, return 404");
			DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK DIRECTORY EXITED" RESET);
			return 404; //not found
		} else {
			DEBUG_PRINT("Internal error: " << strerror(errno) << ", return 500");
			DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK DIRECTORY EXITED" RESET);
			return 500; //internal error
		}
	}
}


int HTTPResponse::createUploadFile(std::string& upload_dir, Content& content){
	DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CREATE UPLOAD FILE CALLED" RESET);
	std::vector<ContentDisposition_>& allCDs = content.getCDs();
	std::vector<ContentDisposition_>::const_iterator it = allCDs.begin();
	
	DEBUG_PRINT("CD status: " << (allCDs.empty() ? "empty" : "not empty"));

	
	content.printCDsList();
	for(; it != allCDs.end(); ++it){
		if (!(it->filename_).empty())
			break;
	}
	
	if (it == allCDs.end()){
		DEBUG_PRINT("No filename found in CDs, return 500");
		DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CREATE UPLOAD FILE EXITED" RESET);
		return 500;
	}
	
	std::string filename = it->filename_;
	std::string file_content = it->file_content_;
	std::string filepath = upload_dir + "/" + filename;
	DEBUG_PRINT("new filename from CDs: " << filename << ", file_content: " << file_content);
	DEBUG_PRINT("File upload created at: " << filepath);
	
	std::ofstream file(filepath.c_str(), std::ios::binary);
	if (!file.is_open()) {
		DEBUG_PRINT("file can't open in writting mode, return 500");
		DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CREATE UPLOAD FILE EXITED" RESET);
		return 500;
	}
	
	file.write(file_content.c_str(), file_content.length());
	
	if (file.fail()){
		DEBUG_PRINT("File failed to write content, return 500");
		DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CREATE UPLOAD FILE EXITED" RESET);
		file.close();
		return 500;
	}
	
	file.close();
	DEBUG_PRINT("File created successfully, return 200");
	DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CREATE UPLOAD FILE EXITED" RESET);
	return 200;
}

void HTTPResponse::setPostResponse() {
	DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET POST RESPONSE CALLED" RESET);
	std::string default_path = default_location_->getRoot();
	default_path.erase(std::remove(default_path.begin(), default_path.end(), '/'), default_path.end());
	// Switch or if statement to see if it is an upload request (CD --> filename)
		// IF fielname exist --> create a file with a filenema define in CD and fill it with the file content of cd and save it under upload
	int status_code = checkFile();
	// 2 OPTIONS --> either a POST CGI request or a REQUEST for upload file
	cgi_flag = current_request_.getCGIFlag();
	DEBUG_PRINT("is CGI POst request? " << cgi_flag);
	if (cgi_flag)
	{
		DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET POST RESPONSE EXITED" RESET);
		CGI_Body();
		return;
	}
	
	DEBUG_PRINT("location: " << location_);
	DEBUG_PRINT("is a upload request: " << current_request_.getPath().find("upload"));
	std::string upload_dir;
	if (location_ && current_request_.getPath().find("upload") != std::string::npos)
		upload_dir = default_path + location_->getUploadDir();		  // upload_dir
	Content &content = current_request_.getContent(); // content
	DEBUG_PRINT("upload_dir: " << upload_dir);
	status_code = checkDirectory(upload_dir);
	if (status_code != 200)
	{
		DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET POST RESPONSE EXITED" RESET);
		setErrorResponse(status_code);
		return;
	}
	status_code = createUploadFile(upload_dir, content);
	if (status_code == 200)
	{
		prepareStatusLine(status_code);
		body_msg_ = "<!DOCTYPE html><html><head><title>Success</title><meta http-equiv=\"refresh\" content=\"3;url=/\"></head><body><h1>Upload Successful!!!!!!!</h1></body></html>";
		content_length_ = body_msg_.length();
		header_ = "Content-Type: text/html; charset=UTF-8\r\nContent-Length: " + convertToStr(content_length_) + "\r\n";
		response_ = status_line_ + header_ + empty_line_ + body_msg_;
	}
	else
	{
		DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET POST RESPONSE EXITED" RESET);
		setErrorResponse(status_code);
	}
	DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET POST RESPONSE EXITED" RESET);
}

void HTTPResponse::setDeleteResponse() { // NOT good as rely only on hardcoding path instead of using the parsing // config file 
	DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET DELETE RESPONSE CALLED" RESET);
    std::string reqPath = current_request_.getPath(); //  "/upload/rose.jpg"
	std::string default_path = default_location_->getRoot(); // "documents/"
	DEBUG_PRINT("reqPath: " << reqPath << ", default_path: " << default_path);
	if (checkFile() != 200) {
		DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET DELETE RESPONSE EXITED" RESET);
        setErrorResponse(404);
	}
	DEBUG_PRINT("body_filename_: " << body_filename_);
    if (fileExists(body_filename_)) {
		// ADD A CHECK TO ENSURE THE FILE IS IN A FOLDER THAT ALLOWED DELETE --> example when error_file is call 
        if (!std::remove(body_filename_.c_str())) {
            response_ = current_request_.getVersion() + " 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nFile deleted";
            _response_ready_ = true;
        } else {
			DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET DELETE RESPONSE EXITED" RESET);
            setErrorResponse(500);
        }
    } else {
		DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET DELETE RESPONSE EXITED" RESET);
        setErrorResponse(404);
    }
	DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET DELETE RESPONSE EXITED" RESET);
}

void HTTPResponse::setErrorResponse(int error_code) {
	DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "SET ERROR RESPONSE CALLED" RESET);
	if (!server_->getErrorList().empty()) {
		DEBUG_PRINT("Error code exist inside server");
		std::map<int, std::string>::iterator begEr = server_->getErrorList().begin();
		std::map<int, std::string>::iterator endEr = server_->getErrorList().end();
		for (; begEr != endEr; ++begEr) {
			if (begEr->first == error_code)
				body_filename_ = begEr->second;
		}
		if (begEr == endEr)
			body_filename_ = server_->getErrorDirectory() + convertToStr(error_code) + ".html";
	} else 
		body_filename_ = default_server_->getErrorDirectory() + convertToStr(error_code) + ".html";
	// std::cout << "body_filename_ found via map lookup: " << body_filename_ << std::endl;
	// std::cerr << "[DEBUG hellooo] setErrorResponse: using body_filename_ = " << body_filename_ << std::endl;
	if (fileExists(body_filename_)) {
		// std::cout << "error_file exist " << body_filename_ << std::endl;
		prepareStatusLine(error_code);
		content_length_ = calculateFileSize(body_filename_);
		header_ = "Content-Type: text/html; charset=UTF-8\r\nContent-Length: " + convertToStr(content_length_) + "\r\nConnection: close\r\n";
		if (!location_->getRedirectURL().empty())
			header_ += "Location: " + location_->getRedirectURL() + "\r\n";
		response_ = status_line_ + header_ + empty_line_;
	} else {
		if (!location_->getRedirectURL().empty())
			draftRedirectResponse();
		else
			draftErrorResponse();
	}
	DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "SET ERROR RESPONSE EXITED" RESET);
}

void HTTPResponse::draftRedirectResponse() {
	std::cout << "error: error-file " << body_filename_ << " doesn't exist\n";
	status_line_ = current_request_.getVersion() + " 301 Moved Permanently\r\n";
	header_ = "Content-Type: text/html; charset=UTF-8\r\nContent-Length: 0\r\nConnection: close\r\nLocation: " + location_->getRedirectURL() + "\r\n";
	response_ = status_line_ + header_ + empty_line_;
	body_filename_ = "";
	DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "SET ERROR RESPONSE EXITED" RESET);
}

void HTTPResponse::draftErrorResponse() {
	std::cout << "error: error-file " << body_filename_ << " doesn't exist\n";
	body_msg_ = "<!DOCTYPE html><html><head><title>500 Error</title></head><body><h1>500 Internal Server Error</h1><p>The server encountered an error and could not complete your request.</p></body></html>";
	status_line_ = current_request_.getVersion() + " 500 Internal server error\r\n";
	header_ = "Content-Type: text/html; charset=UTF-8\r\nContent_Length: " + convertToStr(body_msg_.size()) + "\r\nConnection: close\r\n";
	response_ = status_line_ + header_ + empty_line_ + body_msg_;
	body_filename_ = "";
	DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "SET ERROR RESPONSE EXITED" RESET);
}

//Checking if the file requested exist and if it is possible to read it 
int HTTPResponse::checkFile() {
	DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK FILE CALLED" RESET);
	std::string default_path = default_location_->getRoot();
	default_path.erase(std::remove(default_path.begin(), default_path.end(), '/'), default_path.end());
	DEBUG_PRINT("default path: " << default_path);
	if (current_request_.getPath() == "/") {
		body_filename_ = default_path + "/" + default_location_->getIndex();
		DEBUG_PRINT("body_filename path: " << body_filename_);
		std::ifstream body_file(body_filename_.c_str(), std::ios::binary);
		if (body_file.is_open()) {
			body_file.close();
			DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK FILE EXITED" RESET);
			return 200;
		} else {
			body_file.close();
			std::cerr << BOLD RED "Error in checkFile() with default filename: " << strerror(errno) << RESET << std::endl;
			DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK FILE EXITED" RESET);
			return 500;
		}
	} else {
		body_filename_ = default_path;
		if (current_request_.getPath().find(".") == std::string::npos) {
			if (location_->getIndex().empty() && location_->isAutoIndex() == true) {
				autoIndexRequest();
				DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK FILE EXITED" RESET);
				return 200;
			}
			body_filename_ += location_->getRoot() + "/" + location_->getIndex();
			DEBUG_PRINT("body_filename path with file from config: " << body_filename_);
		} else {
			body_filename_ += current_request_.getPath();
			DEBUG_PRINT("body_filename path with file from HTTP request: " << body_filename_);
		}
		if (fileExists(body_filename_)){
			std::ifstream body_file(body_filename_.c_str(), std::ios::binary);
			if (body_file.is_open()) {
				body_file.close();
				DEBUG_PRINT("File available, return 200");
				DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK FILE EXITED" RESET);
				return 200;
			} else {
				body_file.close();
				std::cerr << BOLD RED "Error in checkFile() with custom filename: " << strerror(errno) << RESET << std::endl;
				DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK FILE EXITED" RESET);
				return 500;	
			}
		} else {
			DEBUG_PRINT("File doesn't exist, return 404");
			DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK FILE EXITED" RESET);
			return 404;
		}
			
	}
	DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK FILE EXITED" RESET);
	return 500;	
}

int HTTPResponse::checkMethod() {
	DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK METHOD CALLED" RESET);
	std::vector<MET>::iterator begM;
	std::vector<MET>::iterator endM;
	if (location_->getMethod().empty()) {
		Location &default_location_ = *default_server_->getLocationsList().front();
		begM = default_location_.getMethod().begin();
		endM = default_location_.getMethod().end();
			for (; begM != endM; ++begM) {
				if (*begM == current_request_.getMethod()) {
					DEBUG_PRINT("Return value of checkMethod for default location: 200");
					DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK METHOD EXITED" RESET);
					return 200;
				}
			}
			DEBUG_PRINT("Return value of checkMethod for default location: 405");
			DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK METHOD EXITED" RESET);
			return 405;
	} else {
		begM = location_->getMethod().begin();
		endM = location_->getMethod().end();
		for (; begM != endM; ++begM) {
				if (*begM == current_request_.getMethod()) {
					DEBUG_PRINT("Return value of checkMethod for location: 200");
					DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK METHOD EXITED" RESET);
					return 200;
				}
			}
		DEBUG_PRINT("Return value of checkMethod for location: 405");
		DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK METHOD EXITED" RESET);
		return 405;
	}
}

void HTTPResponse::prepareStatusLine(int status_code) {
	status_line_ += current_request_.getVersion();
	status_line_ += " " + convertToStr(status_code);
	status_line_ += " " + getStatusStr(status_code);
	status_line_ += "\r\n";
	
	/****DEBUGGING***/
	DEBUG_PRINT("Response status_line: " << status_line_);
}


int HTTPResponse::prepareHeader() {
	//Probably to adapt so it can change for POST and DELETE as the content can be different
	// std::cout << "INSIDE PREPARE HEARDER\n";
	
	if (current_request_.getPath().find(".") != std::string::npos) {
		size_t pos;
		if ((pos = current_request_.getPath().rfind(".")) != std::string::npos)
			content_type_ = "Content-Type: " + getContentType(current_request_.getPath().substr(pos)) + "\r\n";
	}
	// std::cout << "body_filename_: " << body_filename_ << std::endl;;
	content_length_ = calculateFileSize(body_filename_);
	if (content_length_ < 0) {
		std::cerr << BOLD RED "Error: Content-Length is -1\n" RESET;
		return 500;
	}
	header_ = content_type_ + "Content-Length: " + convertToStr(content_length_) + "\r\n";
	
	/****DEBUGGING***/
	DEBUG_PRINT("Response header: " << header_);
	return 200;
}

void HTTPResponse::headerResponse() {
	response_ = status_line_ + header_ + empty_line_;

	/****DEBUGGING***/
	// std::cout << "response: " << response << std::endl;
}

void HTTPResponse::CGI_Body() {
	DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "CGI_BODY CALLED" RESET);
	std::string scriptPath;
	if (!body_filename_.empty())
		scriptPath = body_filename_;
	else
		scriptPath = current_request_.getPath();
	std::string script_name = getFilenameFromPath(body_filename_);
	if (script_name == scriptPath) {
		DEBUG_PRINT("Unexpected URI, return 404");
		DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "CGI_BODY EXITED" RESET);
		setErrorResponse(404);  // unexpected URI
		return;
	}

	std::cout << "uri: " << scriptPath << ", script name: " << script_name <<std::endl;
	// Strip query string from URI to get actual script file path
	size_t qmark = scriptPath.find('?');
	if (qmark != std::string::npos) {
		scriptPath = scriptPath.substr(0, qmark);  // now uri = "/cgi-bin/birthday.py"
		DEBUG_PRINT("URI substring: " << scriptPath);
	}
	if (access(scriptPath.c_str(), F_OK) != 0) {
		DEBUG_PRINT("CGI script not found, return 404");
		DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "CGI_BODY EXITED" RESET);
		setErrorResponse(404);
		return;
	}
	if (access(scriptPath.c_str(), X_OK) != 0) {
		DEBUG_PRINT("CGI script not executable, return 403");
		DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "CGI_BODY EXITED" RESET);
		setErrorResponse(403);
		return;
	}

	// Build the RequestData object for CGI
	RequestData data;
	data.setMethod(current_request_.getMethodAsStr());
	data.setPath(scriptPath);  // Used for SCRIPT_FILENAME and SCRIPT_NAME
	data.setQueryString(current_request_.getQueryStr());
	data.setHeaders(current_request_.getHeaders());
	data.setBody(current_request_.getContent().getBodyWithNoCD());

	DEBUG_PRINT("[CGI] method : " << data.getMethod());
	DEBUG_PRINT("[CGI] - path saved: " << data.getPath());
	DEBUG_PRINT("[CGI] - query str: " << data.getQueryString());
	DEBUG_PRINT("[CGI] - body saved: " << data.getBody());

	if (data.getMethod() == "POST" && data.getBody().empty()) {
		DEBUG_PRINT("[CGI WARNING] POST method but body is empty!");
	}

	CgiHandler handler(data, scriptPath); // pass to CGI engine
	std::string cgiOutput = handler.run();
	// std::cout << "cgiOutput: " << cgiOutput << std::endl;

	// Try to parse headers from CGI output
	size_t headerEnd = cgiOutput.find("\n\n");
	if (headerEnd != std::string::npos) {
		std::string headers = cgiOutput.substr(0, headerEnd);
		std::string body = cgiOutput.substr(headerEnd + 1);

		if (headers.find("Content-Type:") == std::string::npos)
			headers = "Content-Type: text/html\r\n" + headers;

		if (headers.find("Connection:") == std::string::npos)
			headers += "\r\nConnection: close";

		response_ = "HTTP/1.1 200 OK\r\n" + headers + "\r\n\r\n" + body;
		body_filename_.clear();
	} else {
		// fallback response, in case headers were missing
		std::cout << "[CGI] headers not available\n";
		response_ = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + cgiOutput;
	}
	_response_ready_ = true;
	DEBUG_PRINT("[CGI] printing the response:\n" << response_ << "\n");
	DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "CGI_BODY EXITED" RESET);
}

/* ORIGINAL VERSION
void HTTPResponse::CGI_Body() {
	const std::string CGI_ROOT = "./CGI/cgi-bin";
	std::string uri = current_request_.getPath();  // e.g. /cgi-bin/birthday.py?day=13&month=11
	const std::string prefix = "/cgi-bin";
	std::string scriptPath;

	// Strip query string from URI to get actual script file path
	size_t qmark = uri.find('?');
	if (qmark != std::string::npos)
		uri = uri.substr(0, qmark);  // now uri = "/cgi-bin/birthday.py"

	if (uri.find(prefix) == 0) {
		std::string relativePath = uri.substr(prefix.size());     // e.g. "/birthday.py"
		scriptPath = CGI_ROOT + relativePath;                     // e.g. "./CGI/cgi-bin/birthday.py"
		std::cout << "[CGI DEBUG] full path to script: " << scriptPath << std::endl;
	} else {
		setErrorResponse(404);  // unexpected URI
		return;
	}

	if (access(scriptPath.c_str(), F_OK) != 0) {
		std::cerr << "[CGI ERROR] Script not found?? whyyyyy?: " << scriptPath << std::endl;
		setErrorResponse(404);
		return;
	}
	if (access(scriptPath.c_str(), X_OK) != 0) {
		std::cerr << "[CGI ERROR] Script not executable :((( : " << scriptPath << std::endl;
		setErrorResponse(403);
		return;
	}

	std::cerr << "[CGI DEBUG] Raw POST body (from HTTPRequest): [" << current_request_.getRawBody() << "]" << std::endl;

	// Build the RequestData object for CGI
	RequestData data;
	data.setMethod(current_request_.getMethodAsStr());
	data.setPath(scriptPath);  // Used for SCRIPT_FILENAME and SCRIPT_NAME
	data.setQueryString(current_request_.getQueryStr());
	data.setHeaders(current_request_.getHeaders());
	data.setBody(current_request_.getRawBody());

	std::cerr << "[CGI] method : " << data.getMethod() << std::endl;
	std::cerr << "[CGI] - path saved: " << data.getPath() << std::endl;
	std::cerr << "[CGI] - query str: " << data.getQueryString() << std::endl;
	std::cerr << "[CGI] - body saved: " << data.getBody() << std::endl;

	if (data.getMethod() == "POST" && data.getBody().empty()) {
		std::cerr << "[CGI WARNING] POST method but body is empty!" << std::endl;
	}

	CgiHandler handler(data, scriptPath); // pass to CGI engine
	std::string cgiOutput = handler.run();

	// Try to parse headers from CGI output
	size_t headerEnd = cgiOutput.find("\r\n\r\n");
	if (headerEnd != std::string::npos) {
		std::string headers = cgiOutput.substr(0, headerEnd);
		std::string body = cgiOutput.substr(headerEnd + 4);

		if (headers.find("Content-Type:") == std::string::npos)
			headers = "Content-Type: text/html\r\n" + headers;

		if (headers.find("Connection:") == std::string::npos)
			headers += "\r\nConnection: close";

		response_ = "HTTP/1.1 200 OK\r\n" + headers + "\r\n\r\n" + body;
		body_filename_.clear();
	} else {
		// fallback response, in case headers were missing
		response_ = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + cgiOutput;
	}
	_response_ready_ = true;
}
*/



void HTTPResponse::autoIndexRequest() {
	DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "AUTO INDEX REQUEST CALLED" RESET);
	is_autoindex_ = true;
	std::string dir_path = body_filename_ + current_request_.getPath();
	DEBUG_PRINT("Generate auto index from: " << dir_path);
	// needed to save the auto_index file in the base directory
	std::string default_folder = default_location_->getRoot();
	size_t pos = 0;
	if ((pos = default_folder.find("/")) != std::string::npos)
		default_folder = default_folder.substr(pos + 1);
	int status_code = 0;
	status_code = structureInfo(dir_path, current_request_.getPath(), default_folder);
	body_filename_ = default_folder + "/auto_index.html";
	if (status_code != 200) {
		if (!std::remove(body_filename_.c_str())) {
			this->setAutoIndex(false);
		}
		DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "AUTO INDEX REQUEST EXITED" RESET);
		setErrorResponse(status_code);
		return ;
	}
	DEBUG_PRINT("autoIndex body_filename path: " << body_filename_);
	DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "AUTO INDEX REQUEST EXITED" RESET);
}

