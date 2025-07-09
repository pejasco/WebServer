/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:19:25 by chuleung          #+#    #+#             */
/*   Updated: 2025/07/08 22:32:44 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/HTTPResponse.hpp"
#include "../CGI/inc/receiveRequest.hpp"
#include "../CGI/inc/CgiHandler.hpp"

bool cgi_flag = false;

// HTTPResponse:: HTTPResponse() {}

HTTPResponse::HTTPResponse(HTTPRequest *input_request, Server *server_requested, Location *location_requested, Server *master_server, ServerManager *server_manager, Client &client, int error_flag, int error_code) : server_manager_(server_manager), status_code_(0),
current_request_(input_request), server_(server_requested), location_(location_requested), default_server_(server_requested), default_location_(server_requested->getLocationsList().front()), master_server_(master_server), client_(client), empty_line_("\r\n"), is_autoindex_(false), _response_ready_(false) {
	DEBUG_PRINT(BOLD WHITE "\n\n---------------------\n---------------------\nPARSE RESPONSE STARTED\n---------------------\n---------------------\n" RESET);
	if (error_flag > 0) {
		status_code_ = error_code;
		setErrorResponse(status_code_);
		return ;
	}
	if (URILength(current_request_->getPath()) == 414) {
		status_code_ = 414;
		setErrorResponse(status_code_);
		return ;
	}
	if (checkRedirection()) {
		DEBUG_PRINT("checkRedirection() returned true");
		return ;
	}
	switch (current_request_->getMethod()) {
	case GET:
		DEBUG_PRINT("Method called: GET");
		status_code_ = checkMethod();
		if (status_code_ != 200) {
			setErrorResponse(status_code_);
			DEBUG_PRINT(BOLD WHITE "\n\n---------------------\n---------------------\nPARSE REQUEST EXITED\n---------------------\n---------------------\n" RESET);
			return ;
		}
		setGetResponse();
		status_code_ = 200;
		cgi_flag = false;
		break;
	case POST:
		DEBUG_PRINT("Method called: POST");
		status_code_ = checkMethod();
		if (status_code_ != 200) {
			setErrorResponse(status_code_);
			DEBUG_PRINT(BOLD WHITE "\n\n---------------------\n---------------------\nPARSE REQUEST EXITED\n---------------------\n---------------------\n" RESET);
			return;
		}
		setPostResponse();
		DEBUG_PRINT("status code after post request handling: " << status_code_);
		// SM got a array of server > setPostResponse(); // main > SM::serverMonitoring() > SM::existingClientConnection > HTTPRequest current_request_ > HTTPResponse constructor(current_request_, *the pointer of serverManger, string serverIP)
		cgi_flag = false;
		break;
	case DELETE:
		DEBUG_PRINT("Method called: DELETE");
		status_code_ = checkMethod();
		if (status_code_ != 200) {
			setErrorResponse(status_code_);
			DEBUG_PRINT(BOLD WHITE "\n\n---------------------\n---------------------\nPARSE REQUEST EXITED\n---------------------\n---------------------\n" RESET);
			return;
		}
		setDeleteResponse();
		DEBUG_PRINT("DELETE response: " << response_);
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

void HTTPResponse::setBodyFilename(std::string filename) {
	body_filename_ = filename;
}

// GETTER
const std::string &HTTPResponse::getResponse() {
	return response_;
	DEBUG_PRINT("Response that's going to be sent: " << response_);
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

int HTTPResponse::getStatusCode() {
	return status_code_;
}
HTTPRequest *HTTPResponse::getCurrentRequest() {
	return current_request_;
};

Client &HTTPResponse::getClient() {
	return client_;
}
//METHOD

void HTTPResponse::clearBodyFilename() {
	body_filename_.clear();
}

void HTTPResponse::cleanCurrentRequest() {
	if (current_request_) {
		delete current_request_;
		current_request_ = NULL;
	}
}

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
	cgi_flag = current_request_->getCGIFlag();
	DEBUG_PRINT("is current request a CGI: " << cgi_flag);

	int status_code = checkFile();
	cgi_flag = location_->isCGI();
	DEBUG_PRINT("is location allowing CGI: " << cgi_flag);
	if (status_code == 200) {
		if (cgi_flag) {
			DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET GET RESPONSE EXITED" RESET);
			CGI_Body(); // Run CGI handler, which builds full HTTP response
			if (status_code_ == 200)
				_response_ready_ = true;
			return ;
		} else {
			prepareStatusLine();
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
		} else if (errno == EACCES) {
			DEBUG_PRINT("Path exist but permission denied, return 403");
			DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK DIRECTORY EXITED" RESET);
			return 403;
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
		DEBUG_PRINT("No filename found in CDs, return 400");
		DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CREATE UPLOAD FILE EXITED" RESET);
		return 400;
	}
	
	std::string filename = it->filename_;
	std::string file_content = it->file_content_;
	std::string filepath = upload_dir + "/" + filename;
	DEBUG_PRINT("new filename from CDs: " << filename << ", file_content: " << file_content.size());
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
	// Switch or if statement to see if it is an upload request (CD --> filename)
		// IF fielname exist --> create a file with a filenema define in CD and fill it with the file content of cd and save it under upload
	status_code_ = checkFile();
	// 2 OPTIONS --> either a POST CGI request or a REQUEST for upload file
	cgi_flag = current_request_->getCGIFlag();
	DEBUG_PRINT("is CGI POst request? " << cgi_flag);
	if (cgi_flag)
	{
		DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET POST RESPONSE EXITED" RESET);
		CGI_Body();
		return;
	}
	
	DEBUG_PRINT("location: " << location_->getName());
	DEBUG_PRINT("is a upload request: " << location_->isUpload());
	DEBUG_PRINT("upload directory: " << location_->getUploadDir());
	std::string upload_dir;
	if (location_ && location_->isUpload()) {
		if (location_->getUploadDir().find(default_location_->getRoot()) != std::string::npos)
			upload_dir = location_->getUploadDir();	
		else
			upload_dir = default_path + location_->getUploadDir();
	} 	  // upload_dir
	Content &content = current_request_->getContent(); // content
	DEBUG_PRINT("upload_dir: " << upload_dir);
	status_code_ = checkDirectory(upload_dir);
	if (status_code_ != 200)
	{
		DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET POST RESPONSE EXITED" RESET);
		setErrorResponse(status_code_);
		return;
	}
	status_code_ = createUploadFile(upload_dir, content);
	if (status_code_ == 200)
	{
		prepareStatusLine();
		body_msg_ = "<!DOCTYPE html><html><head><title>Success</title><meta http-equiv=\"refresh\" content=\"3;url=/\"></head><body><h1>Upload Successful!!!!!!!</h1></body></html>";
		content_length_ = body_msg_.size();
		header_ = "Content-Type: text/html; charset=UTF-8\r\nContent-Length: " + convertToStr(content_length_) + "\r\nConnection: close\r\n";
		response_ = status_line_ + header_ + empty_line_ + body_msg_;
		body_filename_ = "";
		_response_ready_ = true;
	}
	else
	{
		DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET POST RESPONSE EXITED" RESET);
		setErrorResponse(status_code_);
	}
	DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET POST RESPONSE EXITED" RESET);
}

void HTTPResponse::setDeleteResponse() { // NOT good as rely only on hardcoding path instead of using the parsing // config file 
	DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET DELETE RESPONSE CALLED" RESET);
	std::string reqPath = current_request_->getPath(); //
	std::string default_path = default_location_->getRoot(); // "documents/"
	DEBUG_PRINT("reqPath: " << reqPath << ", default_path: " << default_path);
	status_code_ = checkFile();
	if (status_code_ != 200) {
		DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET DELETE RESPONSE EXITED" RESET);
		setErrorResponse(status_code_);
		return ;
	}
	// if (body_filename_.find(location_->getUploadDir()) == std::string::npos) {
	// 	DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET DELETE RESPONSE EXITED" RESET);
	//     setErrorResponse(status_code_);
	// 	return ;
	// }
	DEBUG_PRINT("body_filename_: " << body_filename_);
	int authorise_code = fileDeletable(body_filename_, location_, default_location_);
	if (authorise_code == 200) {
	   	if (!std::remove(body_filename_.c_str())) {
	   		response_ = current_request_->getVersion() + " 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 12\r\n\r\nFile deleted";
	   		_response_ready_ = true;
			status_code_ = 200;
	   	} else if (errno == EACCES || errno == EPERM){
			DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET DELETE RESPONSE EXITED" RESET);
			status_code_ = 403;
	   		setErrorResponse(status_code_);
			return ;
	   	} else {
			DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET DELETE RESPONSE EXITED" RESET);
			status_code_ = 500;
	   		setErrorResponse(status_code_);
			return;
		}
	} else if (authorise_code == 403){
		DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET DELETE RESPONSE EXITED" RESET);
		status_code_ = 403;
	   	setErrorResponse(status_code_);
		return ;
	} else if (authorise_code == 405) {
		DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET DELETE RESPONSE EXITED" RESET);
		status_code_ = 405;
	   	setErrorResponse(status_code_);
		return ;
	}
	DEBUG_PRINT(BOLD UNDERLINE BG_CYAN BLACK "SET DELETE RESPONSE EXITED" RESET);
}

void HTTPResponse::setErrorResponse(int error_code) {
	status_code_ = error_code;
	DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "SET ERROR RESPONSE CALLED" RESET);
	if (!server_->getErrorList().empty()) {
		DEBUG_PRINT("Error code exist inside server");
		std::map<int, std::string>::iterator begEr = server_->getErrorList().begin();
		std::map<int, std::string>::iterator endEr = server_->getErrorList().end();
		for (; begEr != endEr; ++begEr) {
			if (begEr->first == status_code_)
				body_filename_ = begEr->second;
		}
		if (begEr == endEr)
			body_filename_ = server_->getErrorDirectory() + convertToStr(status_code_) + ".html";
		DEBUG_PRINT("body_filename_ found via map lookup: " << body_filename_);
	} else if (!default_server_->getErrorList().empty()) {
		DEBUG_PRINT("Error code exist inside server");
		std::map<int, std::string>::iterator begEr = default_server_->getErrorList().begin();
		std::map<int, std::string>::iterator endEr = default_server_->getErrorList().end();
		for (; begEr != endEr; ++begEr) {
			if (begEr->first == status_code_)
				body_filename_ = begEr->second;
		}
		if (begEr == endEr)
			body_filename_ = default_server_->getErrorDirectory() + convertToStr(status_code_) + ".html";
		DEBUG_PRINT("body_filename_ found via map lookup: " << body_filename_);		
	} else if (!master_server_->getErrorDirectory().empty()) {
		DEBUG_PRINT("Error code exist inside server");
		std::map<int, std::string>::iterator begEr = master_server_->getErrorList().begin();
		std::map<int, std::string>::iterator endEr = master_server_->getErrorList().end();
		for (; begEr != endEr; ++begEr) {
			if (begEr->first == status_code_)
				body_filename_ = begEr->second;
		}
		if (begEr == endEr)
			body_filename_ = master_server_->getErrorDirectory() + convertToStr(status_code_) + ".html";
		DEBUG_PRINT("body_filename_ found via map lookup: " << body_filename_);		
	} else
		body_filename_ = master_server_->getErrorDirectory() + convertToStr(status_code_) + ".html";
	DEBUG_PRINT("body_filename_ found via default: " << body_filename_);
	// std::cerr << "[DEBUG hellooo] setErrorResponse: using body_filename_ = " << body_filename_ << std::endl;
	if (fileExists(body_filename_)) {
		DEBUG_PRINT("File error found: " << body_filename_);
		prepareStatusLine();
		DEBUG_PRINT("Status code: " << status_code_);
		DEBUG_PRINT("Status line with error code info: " << status_line_);
		content_length_ = calculateFileSize(body_filename_);
		header_ = "Content-Type: text/html; charset=UTF-8\r\nContent-Length: " + convertToStr(content_length_) + "\r\nConnection: close\r\n";
		if (!location_->getRedirectURL().empty())
			header_ += "Location: " + location_->getRedirectURL();
		response_ = status_line_ + header_ + empty_line_;
		DEBUG_PRINT("Error response: " << response_);
	} else {
		if (!location_->getRedirectURL().empty())
			draftRedirectResponse();
		else
			draftErrorResponse();
	}
	// _response_ready_ = true;
	DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "SET ERROR RESPONSE EXITED" RESET);
}

void HTTPResponse::draftRedirectResponse() {
	DEBUG_PRINT("error: error-file " << body_filename_ << " doesn't exist");
	status_line_ = current_request_->getVersion() + " 301 Moved Permanently\r\n";
	header_ = "Content-Type: text/html; charset=UTF-8\r\nContent-Length: 0\r\nConnection: close\r\nLocation: " + location_->getRedirectURL() + "\r\n";
	response_ = status_line_ + header_ + empty_line_;
	body_filename_ = "";
	status_code_ = 301;
	_response_ready_ = true;
	DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "SET ERROR RESPONSE EXITED" RESET);
}

void HTTPResponse::draftErrorResponse() {
	DEBUG_PRINT("error: error-file " << body_filename_ << " doesn't exist");
	body_msg_ = getDefautlErrorPage(status_code_);
	if (body_msg_.empty()) {
		status_line_ = "HTTP/1.1 500 Internal Server Error\r\n";
		header_ = "Content-Type: text/html; charset=UTF-8\r\nContent-Length: 167\r\nConnection: close\r\n";
		body_msg_ = "<!DOCTYPE html><html><head><title>500 Error</title></head><body><h1>500 Internal Server Error\
					</h1><p>The server encountered an error and could not complete your request.</p></body></html>";
		response_ = status_line_ + header_ + empty_line_ + body_msg_;
		body_filename_ = "";
		status_code_ = 500;
	} else
		response_ = body_msg_;
	_response_ready_ = true;
	DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "SET ERROR RESPONSE EXITED" RESET);
}

int HTTPResponse::checkFile() {
	DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK FILE CALLED" RESET);
	std::string default_path = default_location_->getRoot();
	DEBUG_PRINT("default path: " << default_path);
	DEBUG_PRINT("location: " << location_->getRoot() << ", default_location: " << default_location_->getRoot());
	if (current_request_->getPath() == "/") {
		if (location_->getIndex().empty() && location_->isAutoIndex() == true) {
				autoIndexRequest();
				DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK FILE EXITED" RESET);
				return 200;
		}
		if (location_ && location_ != default_location_) {
			body_filename_ = default_path + location_->getRoot() + location_->getIndex();
			DEBUG_PRINT("body_filename from server matching: " << body_filename_);
		} else {
			body_filename_ = default_path + default_location_->getIndex();
			DEBUG_PRINT("body_filename from default server: " << body_filename_);
		}
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
		if (location_ != default_location_ && location_->getRoot().find(default_location_->getRoot()) == std::string::npos) {
			body_filename_ = default_path;
		} else
			body_filename_ = "";
		DEBUG_PRINT("current_request: " << current_request_->getPath());
		if (current_request_->getPath().find(".") == std::string::npos) {
			if (location_->getIndex().empty() && location_->isAutoIndex() == true) {
				autoIndexRequest();
				DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "CHECK FILE EXITED" RESET);
				return 200;
			}
			if (!current_request_->getQueryStr().empty() && current_request_->getQueryStr().find("file=") != std::string::npos) {
				size_t equal = 0;
				DEBUG_PRINT("query string for current request: " << current_request_->getQueryStr());
				if ((equal = current_request_->getQueryStr().find("="))) {
					std::string filename = current_request_->getQueryStr().substr(equal + 1, current_request_->getQueryStr().size() - equal + 1);
					DEBUG_PRINT("filename " << filename);
					body_filename_ += location_->getRoot() + filename; 
				} else 
					body_filename_ += location_->getRoot() + location_->getIndex();
			} else
				body_filename_ += location_->getRoot() + location_->getIndex();
			DEBUG_PRINT("body_filename path with file from config: " << body_filename_);
		} else {
			std::string filename = getFilenameFromPath(current_request_->getPath());
			body_filename_ += location_->getRoot() + filename;
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
				switch(errno) {
					case ENOENT:
						DEBUG_PRINT("File not found: " << strerror(errno));
						return 404;
					case EACCES:
					case EPERM:
						DEBUG_PRINT("Permission denied: " << strerror(errno));
						return 403;
					case EISDIR:
						DEBUG_PRINT("Path is directory: " << strerror(errno));
						return 409;
					case ENOSPC:
						DEBUG_PRINT("No space left: " << strerror(errno));
						return 507;
					case ENAMETOOLONG:
						DEBUG_PRINT("Filename too long: " << strerror(errno));
						return 414;
					default:
						DEBUG_PRINT("Unknown file error: " << strerror(errno) << ": " << errno);
						return 500;
				}
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
	DEBUG_PRINT("location_: " << location_->getName());
	std::vector<MET>::iterator begM;
	std::vector<MET>::iterator endM;
	if (location_->getMethod().empty()) {
		Location &default_location_ = *default_server_->getLocationsList().front();
		begM = default_location_.getMethod().begin();
		endM = default_location_.getMethod().end();
			for (; begM != endM; ++begM) {
				if (*begM == current_request_->getMethod()) {
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
				if (*begM == current_request_->getMethod()) {
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

void HTTPResponse::prepareStatusLine() {
	if (!current_request_->getVersion().empty())
		status_line_ += current_request_->getVersion() + " ";
	else 
		status_line_ += "HTTP/1.1 ";
	status_line_ += convertToStr(status_code_) + " ";
	status_line_ += getStatusStr(status_code_);
	status_line_ += "\r\n";
	
	/****DEBUGGING***/
	// DEBUG_PRINT("Response status_line: " << status_line_);
}


int HTTPResponse::prepareHeader() {
	//Probably to adapt so it can change for POST and DELETE as the content can be different
	// std::cout << "INSIDE PREPARE HEARDER\n";
	
	if (current_request_->getPath().find(".") != std::string::npos) {
		size_t pos;
		if ((pos = current_request_->getPath().rfind(".")) != std::string::npos)
			content_type_ = "Content-Type: " + getContentType(current_request_->getPath().substr(pos)) + "\r\n";
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
	const std::string& scriptPath = !body_filename_.empty() ? body_filename_ : current_request_->getPath();
	std::string script_name = getFilenameFromPath(body_filename_);
	status_code_ = checkExtensions(location_, script_name);
	if (status_code_ != 200) {
		DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "CGI_BODY EXITED" RESET);
		setErrorResponse(status_code_);
		return;
	}

	if (script_name == scriptPath) {
		DEBUG_PRINT("Unexpected URI, return 404");
		DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "CGI_BODY EXITED" RESET);
		status_code_ = 404;
		setErrorResponse(status_code_);  // unexpected URI
		return;
	}

	DEBUG_PRINT("uri: " << scriptPath << ", script name: " << script_name);
	// // Strip query string from URI to get actual script file path
	// size_t qmark = scriptPath.find('?');
	// if (qmark != std::string::npos) {
	// 	scriptPath = scriptPath.substr(0, qmark);  // now uri = "/cgi-bin/birthday.py"
	// 	DEBUG_PRINT("URI substring: " << scriptPath);
	// }
	if (access(scriptPath.c_str(), F_OK) != 0) {
		DEBUG_PRINT("CGI script not found, return 404");
		DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "CGI_BODY EXITED" RESET);
		status_code_ = 404;
		setErrorResponse(status_code_);
		return;
	}
	if (access(scriptPath.c_str(), X_OK) != 0) {
		DEBUG_PRINT("CGI script not executable, return 403");
		DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "CGI_BODY EXITED" RESET);
		status_code_ = 403;
		setErrorResponse(status_code_);
		return;
	}
	// Build the RequestData object for CGI
	RequestData data;
	data.setMethod(current_request_->getMethodAsStr());
	// data.setPath(scriptPath);  // Used for SCRIPT_FILENAME and SCRIPT_NAME
	data.setQueryString(current_request_->getQueryStr());
	data.setHeaders(current_request_->getHeaders());
	data.setBody(current_request_->getContent().getBodyWithNoCD());

	DEBUG_PRINT("[CGI] method : " << data.getMethod());
	DEBUG_PRINT("[CGI] - path saved: " << data.getPath());
	DEBUG_PRINT("[CGI] - query str: " << data.getQueryString());
	DEBUG_PRINT("[CGI] - body saved: " << data.getBody());

	if (data.getMethod() == "POST" && data.getBody().empty()) {
		DEBUG_PRINT("[CGI WARNING] POST method but body is empty!");
	}

	CgiHandler handler(data, scriptPath, server_manager_, body_filename_, this); // pass to CGI engine
	std::string cgiOutput = handler.run(server_manager_, this);

	// Try to parse headers from CGI output
	size_t headerEnd = cgiOutput.find("\r\n\r\n");
	// DEBUG_PRINT("cgioutput: " << cgiOutput);
	if (headerEnd == std::string::npos)
		headerEnd = cgiOutput.find("\n\n");
	if (headerEnd != std::string::npos) {
		std::string headers = cgiOutput.substr(0, headerEnd);
		// DEBUG_PRINT("Header: " << headers);
		std::string body = cgiOutput.substr(headerEnd + 2);
		if (headers.find("Content-Type:") == std::string::npos)
			headers = "Content-Type: text/html\r\n" + headers;
		headers += "\r\nContent-Length: " + convertToStr(body.size());
		if (headers.find("Connection:") == std::string::npos)
			headers += "\r\nConnection: close";
		response_ = "HTTP/1.1 200 OK\r\n" + headers + "\r\n\r\n" + body;
		body_filename_.clear();
	} else {
		// fallback response, in case headers were missing
		response_ = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + convertToStr(cgiOutput.size()) + "\r\n\r\n" + cgiOutput;
	}
	// DEBUG_PRINT("response: " << response_);
	_response_ready_ = true;
}


void HTTPResponse::autoIndexRequest() {
	DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "AUTO INDEX REQUEST CALLED" RESET);
	is_autoindex_ = true;
	std::string dir_path = resolvePathWithoutDuplication(location_->getRoot(), current_request_->getPath());
	std::string default_folder = master_server_->getLocationsList().front()->getRoot();
	DEBUG_PRINT("dir_path: " << dir_path << " & default_folder: " << default_folder);
	status_code_ = structureInfo(dir_path, current_request_->getPath(), default_folder);
	DEBUG_PRINT("status code: " << status_code_);
	body_filename_ = default_folder + "auto_index.html";
	DEBUG_PRINT("body_filename: " << body_filename_);
	if (status_code_ != 200) {
		if (!std::remove(body_filename_.c_str())) {
			this->setAutoIndex(false);
		}
		DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "AUTO INDEX REQUEST EXITED" RESET);
		setErrorResponse(status_code_);
		return ;
	}
	DEBUG_PRINT("autoIndex body_filename path: " << body_filename_);
	DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "AUTO INDEX REQUEST EXITED" RESET);
}

