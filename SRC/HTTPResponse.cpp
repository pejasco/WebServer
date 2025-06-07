/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssottori <ssottori@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:19:25 by chuleung          #+#    #+#             */
/*   Updated: 2025/06/07 04:35:29 by ssottori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/HTTPResponse.hpp"
#include "../CGI/inc/receiveRequest.hpp"
#include "../CGI/inc/CgiHandler.hpp"

bool cgi_flag = false;

HTTPResponse::HTTPResponse(const HTTPRequest &input_request, Server *default_server, Server *server_requested, Location *location_requested, int error_flag) : 
current_request_(input_request), server_(server_requested), location_(location_requested), default_server_(default_server), default_location_(default_server->getLocationsList().front()), empty_line_("\r\n"), is_autoindex_(false), _response_ready_(false) {
	std::cout << "here the error code is: " << error_flag << std::endl;;
	if (error_flag > 0) {
		setErrorResponse(error_flag);
		return ;
	}
	if (checkRedirection()) {
		std::cout << "redirection was true, bye!\n";
		return ;
	}
	switch (current_request_.getMethod()) {
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
		setPostResponse();
		// SM got a array of server > setPostResponse(); // main > SM::serverMonitoring() > SM::existingClientConnection > HTTPRequest current_request_ > HTTPResponse constructor(current_request_, *the pointer of serverManger, string serverIP)
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
	std::cout << "\nPRINTING RESPONSE\n\n" << response_ << std::endl;
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
	// std::cout << "status_line: " << status_line << std::endl;
	// std::cout << "header: " << header << std::endl;
	// std::cout << "response header that is going to be sent:\n" << response;

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
	std::cout << "redirect debugging\n";
	if (!location_->getRedirectURL().empty()) {
		std::cout << "location identify as redirection\n"; 
		setErrorResponse(location_->getRedirectCode());
		return true;
	}
	return false;
}

void HTTPResponse::setGetResponse() {
	//1st -- check if the path request by the user exist
	//2nd -- check if the file exist and readable (not already open, with correct permission (not sure if we need to set it up)) 
	cgi_flag = current_request_.getCGIFlag();
	// std::cerr << "[debugging] cgi_flag = " << cgi_flag << std::endl;
	//body_filename_ = "./CGI/cgi-bin/test.py"; //need to figure out how to set the correct root path for the cgi....maybe checkFile?

	//int status_code = checkFile();
	int status_code;
	if (cgi_flag) {
		body_filename_ = "./CGI/cgi-bin/test.py";  // TEMPORARY override for testing
		status_code = fileExists(body_filename_) ? 200 : 404;
	} else {
		status_code = checkFile();
		std::cout << "status_code of checkFIle in GET: " << status_code << std::endl;
	}
	if (status_code == 200) {
		if (cgi_flag) {
			CGI_Body(); // Run CGI handler, which builds full HTTP response
			std::cerr << "[CGI] Early return after CGI_Body()\n";
			_response_ready_ = true;
			return ;
		} else {
			prepareStatusLine(status_code);
			prepareHeader();
			headerResponse();
			//_response_ready_ = true;
		}
	}
	else
	{
		std::cerr << "[ERRODEBUGG] Non-200 GET status. Calling setErrorResponse(" << status_code << ")\n";
		setErrorResponse(status_code);
		_response_ready_ = true;
	}
}

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


int HTTPResponse::createUploadFile(std::string& upload_dir, const Content& content){
	const std::vector<ContentDisposition_>& allCDs = content.getCDs();
	std::vector<ContentDisposition_>::const_iterator it = allCDs.begin();
	
	allCDs.empty() ? std::cout << "<<sievdebug>> " << "CD is empty!\n" : std::cout << "<<sievdebug>> " << "CD is not empty!\n";

	//<<sievdebug>>
	content.printCDsList();
	for(; it != allCDs.end() && !((it->filename_).empty()); ++it){
	}
	
	if (it == allCDs.end()){
		std::cout << "Error: There is no filename found" << std::endl;
		return 500;
	}
	
	std::cout << "<<sievdebug>> " << "before filename\n";
	std::string filename = it->filename_;
	std::cout << "<<sievdebug>> " << "filename:" << filename << "\n";
	std::cout << "<<sievdebug>> " << "after filename\n";

	std::string file_content = it->content_;
	
	std::string filepath = upload_dir + "/" + filename;
	std::cout << "Trying to create file: " << filepath << std::endl;
	
	std::ofstream file(filepath.c_str(), std::ios::binary);
	if (!file.is_open()) {
		std::cout << "Error: Could not open file for writing: " << strerror(errno) << std::endl;
		return 500;
	}
	
	file.write(file_content.c_str(), file_content.length());
	
	if (file.fail()){
		std::cout << "Error: Failed to write to file: " << strerror(errno) << std::endl;
		file.close();
		return 500;
	}
	
	file.close();
	std::cout << "File created successfully: " << filepath << std::endl;
	return 200;
}

//std::vector<ContentDisposition_> CDs_list_;


void HTTPResponse::setPostResponse() {
	// Switch or if statement to see if it is an upload request (CD --> filename)
		// IF fielname exist --> create a file with a filenema define in CD and fill it with the file content of cd and save it under upload		
		std::string upload_dir;
		int status_code;

		std::cout << "<<sievdebug>>" << "yoyoyo " << location_ << " yoyoyo" << "\n";
		std::cout << "<<sievdebug>>" << "xoxoxo " << current_request_.getPath().find("upload") << " xoxoxo" << "\n";

		if (location_ && current_request_.getPath().find("upload") != std::string::npos)
			upload_dir = location_->getUploadDir(); //upload_dir
		const Content& content = current_request_.getContent(); //content
		std::cout << "<<sievdebug>>" << "!!!!!!!!!!!!!!!!!" << upload_dir << "!!!!!!!!!!!!!!!!!" << "\n";
		//std::cout << "!!!!!!!!!!!!!!!!!" << << "!!!!!!!!!!!!!!!!!\'n";

		status_code = checkDirectory(upload_dir);
		if (status_code != 200){
			setErrorResponse(status_code);
			return;
		}
		status_code = createUploadFile(upload_dir, content);
		if (status_code == 200) { 
			prepareStatusLine(status_code);
			body_msg_ = "<!DOCTYPE html><html><head><title>Success</title><meta http-equiv=\"refresh\" content=\"3;url=/\"></head><body><h1>Upload Successful!!!!!!!</h1></body></html>";
		// if (cgi_flag) -- Check with Shally if we need that
		// 	CGI_Body();
			content_length_ = body_msg_.length();
			header_ = "Content-Type: text/html; charset=UTF-8\r\nContent-Length: " + convertToStr(content_length_) + "\r\n";
			response_ = status_line_ + header_ + empty_line_ + body_msg_;
	
		} else {
			setErrorResponse(status_code);
	}
}

// void HTTPResponse::makePostResponse(ContentDisposition_ &cd) {
// 	// Switch or if statement to see if it is an upload request (CD --> filename)
// 		// IF fielname exist --> create a file with a filenema define in CD and fill it with the file content of cd and save it under upload		
		
// 		int status_code = 400; //default to be bad
// 		std::string path = current_request_.getPath();
// 		Content cd = current_request_.getContent();
// 		if (!path.empty()) //for upload only && not cgi 
// 			status_code = createUploadFile(path, cd);
// 		if (status_code == 200) { 
// 			prepareStatusLine(status_code);
			
// 			body = "<!DOCTYPE html><html><head><title>Success</title><meta http-equiv=\"refresh\" content=\"3;url=/\"></head><body><h1>Upload Successful!!!!!!!</h1></body></html>";
// 		// if (cgi_flag) -- Check with Shally if we need that
// 		// 	CGI_Body();
// 			content_length = body.length();
// 			header = "Content-Type: text/html; charset=UTF-8\r\nContent-Length: " + convertToStr(content_length) + "\r\n";
// 			response = status_line + header + empty_line + body;
	
// 		} else
// 			setErrorResponse(status_code);
// }




void HTTPResponse::setDeleteResponse() {
	body_filename_ = "documents" + current_request_.getPath();
	if (fileExists(body_filename_)) {
		if (!std::remove(body_filename_.c_str()))
			response_ = current_request_.getVersion() + " 204 No Content\r\n\r\n";
		else
			setErrorResponse(500); // file exist but not closable so internal error ?
	} else {
		setErrorResponse(404); // file not found as return false to fileExist()? 
	}
}

void HTTPResponse::setErrorResponse(int error_code) {
	if (!server_->getErrorList().empty()) {
		std::cout << "Error list exist inside current server\n";
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
	std::cout << "body_filename_ found via map lookup: " << body_filename_ << std::endl;
	std::cerr << "[DEBUG hellooo] setErrorResponse: using body_filename_ = " << body_filename_ << std::endl;
	if (fileExists(body_filename_)) {
		std::cout << "error_file exist " << body_filename_ << std::endl;
		prepareStatusLine(error_code);
		content_length_ = calculateFileSize(body_filename_);
		header_ = "Content-Type: text/html; charset=UTF-8\r\nContent-Length: " + convertToStr(content_length_) + "\r\nConnection: close\r\n";
		if (!location_->getRedirectURL().empty())
			header_ += "Location: " + location_->getRedirectURL() + "\r\n";
		response_ = status_line_ + header_ + empty_line_;
		std::ifstream file(body_filename_.c_str());
		if (file) {
			std::stringstream buffer;
			buffer << file.rdbuf();
			response_ += buffer.str();
			file.close();
		} else {
			std::cerr << "[DEBUG STILL TRYING TO FIX ERROR] setErrorResponse: could not open " << body_filename_ << " for reading\n";
		}
	} else {
		if (!location_->getRedirectURL().empty())
			draftRedirectResponse();
		else
			draftErrorResponse();
	}
	std::cerr << "[DEBUG draftErrorResponse whyyyys it ot working] response_ built = \n" << response_ << "\n";
}

void HTTPResponse::draftRedirectResponse() {
	std::cout << "error: error-file " << body_filename_ << " doesn't exist\n";
	// body = "<!DOCTYPE html><html><head><title>301 Moved permanently</title></head><body><h1>301 Moved permanently</h1><p>This page is not available anymore. If you are not automatically redirect, please follow this <a href=" + location->getRedirectURL() +">link</a></p></body></html>";
	status_line_ = current_request_.getVersion() + " 301 Moved Permanently\r\n";
	header_ = "Content-Type: text/html; charset=UTF-8\r\nContent-Length: 0\r\nConnection: close\r\nLocation: " + location_->getRedirectURL() + "\r\n";
	response_ = status_line_ + header_ + empty_line_;
	body_filename_ = "";
}

void HTTPResponse::draftErrorResponse() {
	std::cout << "error: error-file " << body_filename_ << " doesn't exist\n";
	// body_msg_ = "<!DOCTYPE html><html><head><title>500 Error</title></head><body><h1>500 Internal Server Error</h1><p>The server encountered an error and could not complete your request.</p></body></html>";
	// status_line_ = current_request_.getVersion() + " 500 Internal server error\r\n";
	// header_ = "Content-Type: text/html; charset=UTF-8\r\nContent_Length: " + convertToStr(body_msg_.size()) + "\r\nConnection: close\r\n";
	// response_ = status_line_ + header_ + empty_line_ + body_msg_;
	// body_filename_ = "";
}

//Checking if the file requested exist and if it is possible to read it 
int HTTPResponse::checkFile() {
	std::string default_path = default_location_->getRoot();
	default_path.erase(std::remove(default_path.begin(), default_path.end(), '/'), default_path.end());
	std::cout << "check2\n";
	if (current_request_.getPath() == "/") {
		body_filename_ = default_path + "/" + default_location_->getIndex();
		std::cout << "check3\n";
		std::cout << "body_filename_: " << body_filename_ << std::endl;
		std::ifstream body_file(body_filename_.c_str(), std::ios::binary);
		std::cout << "check4\n";
		if (body_file.is_open()) {
			body_file.close();
			return 200;
		} else {
			body_file.close();
			std::cout << "Error: " << strerror(errno) << std::endl;
			return 500;
		}
		std::cout << "check5\n";
	} else {
		body_filename_ = default_path;
		std::cout << BOLD YELLOW "body_filename_: " << body_filename_ << "current_request_: " << current_request_.getPath() << RESET << std::endl;
		if (current_request_.getPath().find(".") == std::string::npos) {
			std::cout << "XXXXXX -- " << location_->getIndex() << " -- " << location_->isAutoIndex() << std::endl;
			if (location_->getIndex().empty() && location_->isAutoIndex() == true) {
				autoIndexRequest();
				return 200;
			}
			body_filename_ += location_->getRoot() + "/" + location_->getIndex();
			std::cout << "adding the file from config\n";
		} else {
			body_filename_ += current_request_.getPath();
			std::cout << "adding the file from request\n";
		}
		std::cout << BOLD YELLOW "New body_filename_: " << body_filename_ << RESET << std::endl;
		if (fileExists(body_filename_)){
			std::ifstream body_file(body_filename_.c_str(), std::ios::binary);
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
	if (location_->getMethod().empty()) {
		Location &default_location_ = *default_server_->getLocationsList().front();
		begM = default_location_.getMethod().begin();
		endM = default_location_.getMethod().end();
			for (; begM != endM; ++begM) {
				if (*begM == current_request_.getMethod()) {
					std::cout << "return value of checkMethod: 200\n";
					return 200;
				}
			}
			std::cout << "return value of checkMethod: 405\n";
			return 405;
	} else {
		begM = location_->getMethod().begin();
		endM = location_->getMethod().end();
		for (; begM != endM; ++begM) {
				if (*begM == current_request_.getMethod()) {
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
	status_line_ += current_request_.getVersion();
	status_line_ += " " + convertToStr(status_code);
	status_line_ += " " + getStatusStr(status_code);
	status_line_ += "\r\n";
	
	/****DEBUGGING***/
	// std::cout << "status_line: " << status_line << std::endl;
}


int HTTPResponse::prepareHeader() {
	//Probably to adapt so it can change for POST and DELETE as the content can be different
	std::cout << "INSIDE PREPARE HEARDER\n";
	
	if (current_request_.getPath().find(".") != std::string::npos) {
		size_t pos;
		if ((pos = current_request_.getPath().rfind(".")) != std::string::npos)
			content_type_ = "Content-Type: " + getContentType(current_request_.getPath().substr(pos)) + "\r\n";
	}
	std::cout << "body_filename_: " << body_filename_ << std::endl;;
	content_length_ = calculateFileSize(body_filename_);
	if (content_length_ < 0) {
		std::cout << "Error: Content-Length is -1\n";
		return 500;
	}
	header_ = content_type_ + "Content-Length: " + convertToStr(content_length_) + "\r\n";
	/****DEBUGGING***/
	std::cout << "HEADER = \n" << header_ << std::endl;
	return 200;
}

void HTTPResponse::headerResponse() {
	response_ = status_line_ + header_ + empty_line_;

	/****DEBUGGING***/
	// std::cout << "response: " << response << std::endl;
}

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
		std::cerr << "[CGI DEBUG] full path to script: " << scriptPath << std::endl;
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


void HTTPResponse::autoIndexRequest() {
	//needed to look inside a directory
	is_autoindex_ = true;
	std::string dir_path = body_filename_ + current_request_.getPath();
	std::cout << "directory to lookup: " << body_filename_ + current_request_.getPath() << std::endl;
	// needed to save the auto_index file in the base directory
	std::string default_folder = default_location_->getRoot();
	size_t pos = 0;
	if ((pos = default_folder.find("/")) != std::string::npos)
		default_folder = default_folder.substr(pos + 1);
	int status_code = 0;
	std::cout << "Auto_index future location: " << default_folder << std::endl;
	status_code = structureInfo(dir_path, current_request_.getPath(), default_folder);
	body_filename_ = default_folder + "/auto_index.html";
	if (status_code != 200) {
		if (!std::remove(body_filename_.c_str())) {
			this->setAutoIndex(false);
		}
		setErrorResponse(status_code);
		return ;
	}
	std::cout << "Auto_index file location: " << body_filename_ << std::endl;
}

