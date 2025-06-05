/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:19:15 by chuleung          #+#    #+#             */
/*   Updated: 2025/06/05 12:32:46 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/HTTPResponse.hpp"

// #define PORT 8080
#define BUFFER_SIZE 4096


//<<static variables>>
int Accept::global_index_ = 0;
int HTTPRequest::global_index_ = 0;

//<<Accept>>
Accept::Accept(std::string type, std::string subtype, float piority) : type_(type), subtype_(subtype), piority_(piority){}
Accept::~Accept(){}



//<<HTTPRequest>>
HTTPRequest::HTTPRequest() : instance_index_(global_index_++), content_flag_(false),
	is_in_the_body_flag_(false), boundary_index_(-1), cgi_flag_(false){}

HTTPRequest::~HTTPRequest(){}


//Setters

void HTTPRequest::setContentFlag(const bool flag){
	content_flag_ = flag;

}

void HTTPRequest::setBoundaryIndex(int index){
	boundary_index_ = index;
}


void HTTPRequest::setIsInTheBodyFlag(const bool flag){
	is_in_the_body_flag_ = flag;
}


void HTTPRequest::setCGIFlag(const bool flag){
	cgi_flag_ = flag;
	std::cerr << "[debuggging] setting CGI flag to-> " << (flag ? "true" : "false") << std::endl;
}

void HTTPRequest::setCGIType(const std::string cgi_type){
	cgi_type_ = cgi_type;
}


void HTTPRequest::setMet(const std::string&  method){
	if (method == "GET")
		method_ = GET;
	else if (method == "POST")
	{
		method_ = POST;
		setContentFlag(true); //indicating there is content
	}
	else if (method == "DELETE")
		method_ = DELETE;
	else
		throw std::invalid_argument("Invalid HTTP method: " + method);
}

void HTTPRequest::setPath(const std::string& path) {
	path_ = path;
	// std::cout << "path: " << path << std::endl;
	if (path_.find("cgi") != std::string::npos)
		setCGIFlag(true);
}

void HTTPRequest::setVersion(const std::string& version){
	version_ = version;
	// std::cout << "version: " << version << std::endl;
}

void HTTPRequest::setHost(std::string& host) {
	host.erase(host.end() - 1);
	host_ = host;
	// std::cout << "host: " << host << std::endl;
}

void HTTPRequest::setUserAgent(const std::string& agents){
	std::stringstream iss(agents);
	std::string token;
	std::string last_key;

	while(iss >> token)
	{
		if (token.find('/') != std::string::npos){
			last_key = token;
			user_agent_[last_key] = "";
		} 
		else if (token[0] == '('){
			std::string group = token;

			bool group_complete = false;
			// if (token.find(')') != std::string::npos) {
			//     group_complete = true;

			// }
			while (!group_complete && iss >> token){

				group += " " + token;
				if (token.find(')') != std::string::npos) {
					group_complete = true;
				}
			}
			user_agent_[last_key] = group;
		}
	}
}

void HTTPRequest::setAccept(const std::string& media_types){
	std::stringstream iss(media_types);
	std::string media_type;
	size_t pos_begin;
	size_t pos_end;
	std::string type;
	std::string subtype;
	float priority;

	while(getline(iss, media_type, ','))
	{
		priority = 1.0;
		
		if (media_type.find(';') == std::string::npos) {
			pos_end = media_type.find('/');
			pos_begin = 0;
			
			if (pos_begin == pos_end)
				type = "*";
			else
				type = media_type.substr(0, pos_end);
				
			pos_begin = pos_end + 1;
			subtype = media_type.substr(pos_begin, std::string::npos);
		} else {
			// Handle piority
			pos_end = media_type.find('/');
			type = media_type.substr(0, pos_end);
			
			pos_begin = pos_end + 1;
			pos_end = media_type.find(';');
			subtype = media_type.substr(pos_begin, pos_end - pos_begin);
			
			// Extract piority
			if (media_type.find("q=") != std::string::npos) {
				pos_begin = media_type.find("q=") + 2;
				std::string q = media_type.substr(pos_begin);
				std::stringstream ss(q);
				ss >> priority;
			}
		}
		
		//need to handle more type in the future
		if (subtype.empty() && type == "image") {
			subtype = "*";
		}
		
		accept_list_.push_back(Accept(type, subtype, priority));
	}
}

void HTTPRequest::setAcceptLanguage(const std::string& languages){
	std::stringstream iss(languages);
	std::string language;
	size_t pos_begin;
	size_t pos_end;
	std::string lang;
	float priority;

	while(getline(iss, language, ',')){
		priority = 1.0;

		if (language.find(';') == std::string::npos)
			lang = language;
		else {
			pos_end = language.find(';');
			lang = language.substr(0, pos_end);
			if (language.find("q=") != std::string::npos){
				pos_begin = language.find("q=") + 2;
				std::string q = language.substr(pos_begin);
				std::stringstream ss(q);
				ss >> priority;
			}
		}
		accept_language_[lang] = priority;
	}
}

void HTTPRequest::setReferer(const std::string& referer){
	referer_ = referer;
	// std::cout << "referer: " << referer << std::endl;
}

void HTTPRequest::setAcceptEncoding(const std::string& encoding){
	std::istringstream iss(encoding);
	std::string line;

	while (std::getline(iss, line, ',')){
		accept_encoding_.push_back(line);
	}
}

void HTTPRequest::setConnection(const std::string& connection){
	if (connection == "keep-alive")
		connection_ = true;
}

// void setCookie(const std::string& version)


void HTTPRequest::setAuthorization(const std::string& version){
	(void)version;

}


void HTTPRequest::setContent(const std::string& content){
	(void)content;


}

void HTTPRequest::setUnknown(const std::string& buffer){
	(void)buffer;

	
}

MET HTTPRequest::getMethod() {
	return method_;
};

const std::string& HTTPRequest::getPath() const{
	return path_;
}

const std::string& HTTPRequest::getVersion(){
	return version_;
}

const std::string& HTTPRequest::getHost() const {
	return host_;
}

const std::map<std::string, std::string>& HTTPRequest::getUserAgent(){
	return user_agent_;
}

const std::vector<Accept>& HTTPRequest::getAccept(){
	return accept_list_;
}

const std::map<std::string, float>& HTTPRequest::getAcceptLanguage(){
	return accept_language_;
}

bool HTTPRequest::getConnection(){
	return connection_;
}

const std::string& HTTPRequest::getReferer(){
	return referer_;
}


const std::pair<std::string, std::string>& HTTPRequest::getAuthorisation(){
	return authorisation_;
}

const Content& HTTPRequest::getContent(){
	return content_;
}

const std::map <std::string, std::string>& HTTPRequest::getUnknown(){
	return unknown_;
}

bool HTTPRequest::getCGIFlag() {
	return cgi_flag_;
}

bool HTTPRequest::getIsInTheBody() {
	return is_in_the_body_flag_;
}

//Parser

void HTTPRequest::parseRequestLine(const std::string& request_line){
	std::string method, path_with_query, version;
	
	std::istringstream stream(request_line);
	stream >> method >> path_with_query >> version;

	// Split path and query
	size_t pos = path_with_query.find('?');
	if (pos != std::string::npos) {
		std::string path = path_with_query.substr(0, pos);
		std::string query = path_with_query.substr(pos + 1);
		setPath(path);
		setQueryStr(query);
	} else {
		setPath(path_with_query);
		setQueryStr("");
	}

	setMet(method);
	setVersion(version);
}




void HTTPRequest::parseContent(const std::string& body_line){
	size_t pos_begin;
	size_t pos_end;
	std::string line;
	(void)pos_end;
	
	// std::cout << BOLD MAGENTA "I got the body_line, time to parse content\n" RESET;
	if (body_line == content_.getBoundary()){
		// std::cout << "check1\n";
		setIsInTheBodyFlag(true);
		if (boundary_index_ == -1)
			boundary_index_ = 0;
		else
			boundary_index_ += 1;
		// multipart/form-data
	} else if (body_line == content_.getBoundary() + "--"){
		// std::cout << "check1bis\n";
		setContentFlag(false);
		setCGIFlag(false);
		setIsInTheBodyFlag(false);
		setBoundaryIndex(-1);

	} else if (cgi_flag_ == 0 && content_.getContentLength() >= 0 
		&& !content_.getBoundary().empty() && is_in_the_body_flag_){
		// std::cout << "check2\n";
		if (cgi_flag_ == true){
			// std::cout << "check2bis\n";
			size_t pos_begin = path_.rfind(".");
			cgi_type_ = path_.substr(pos_begin + 1);
		}
		
		if (body_line.find("Content-Disposition:") != std::string::npos){
			// std::cout << "check3\n";
			content_.addContentDisposition(ContentDisposition_());
			// std::cout << "is it working here\n";
			if((pos_begin = body_line.find(':')) != std::string::npos){
				pos_begin = body_line.find_first_not_of(" \t", pos_begin + 1);
				line = body_line.substr(pos_begin);
				content_.setCDs(line, ContentDisposition, boundary_index_);
			}
		} else if (body_line.find("Content-Type:") != std::string::npos){
			// std::cout << "check4\n";
			if((pos_begin = body_line.find(':')) != std::string::npos){
				pos_begin = body_line.find_first_not_of(" \t", pos_begin + 1);
				line = body_line.substr(pos_begin);
				content_.setCDs(line, InterContentType, boundary_index_);}
		
		} else if (!body_line.empty()
			&& (content_.getCDs()[boundary_index_].filename_.empty())){
				// std::cout << "check4\n";
				content_.setCDs(body_line, Cont, boundary_index_);
				
		} else if (!body_line.empty()
			&& !(content_.getCDs()[boundary_index_].filename_.empty())){
				// std::cout << "check5\n";
				content_.setCDs(body_line, FileContent, boundary_index_);
	
		}
			
	} else {
		// Handle
		// std::cout << "check6\n";
		content_.setBodyWithNoCD(body_line); 
		
	}
}
//if (!(content_.getContentType()->first).empty() && content_.getContentLength() >= 0

void HTTPRequest::parseRequestHeader(std::istringstream& stream){
	std::string line;
	size_t pos_begin;
	// size_t pos_end;

	while (std::getline(stream, line))
	{
		size_t colon = line.find(":"); //setting headers to _headers
		if (colon != std::string::npos) {
			std::string key = line.substr(0, colon);
			std::string val = line.substr(colon + 1);

			// Trim leading space
			val.erase(0, val.find_first_not_of(" \t"));

			headers_[key] = val;
		}
		// std::cout << "ParseRequest: " << line << std::endl;
		//GET
		if (line.empty()){
			if (!(content_.getContentType().first).empty() && content_.getContentLength() >= 0 && is_in_the_body_flag_) {
				// std::cout << BOLD MAGENTA "check content to fill\n" RESET;
				parseContent(line);
			}
			else
				continue;
		} else if (line.find("Host") != std::string::npos){
			if ((pos_begin = line.rfind(":")) != std::string::npos){
				pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
				std::string host = line.substr(pos_begin, std::string::npos);
				setHost(host);
				// std::cout << "Host: " << host << std::endl;
			}
		} else if (line.find("Connection") != std::string::npos){
			if ((pos_begin = line.rfind(":")) != std::string::npos){
				pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
				std::string connection = line.substr(pos_begin, std::string::npos);
				setConnection(connection);
				// std::cout << "connection: " << connection << std::endl;
			}
		} else if (line.find("User-Agent") != std::string::npos){   
			if ((pos_begin = line.rfind(":")) != std::string::npos){
				pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
				std::string agents = line.substr(pos_begin, std::string::npos);
				setUserAgent(agents);
				// std::cout << "agents: " << agents << std::endl;
			}
		} else if (line.find("Accept") != std::string::npos){
			if ((pos_begin = line.rfind(":")) != std::string::npos){
				pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
				std::string accept = line.substr(pos_begin, std::string::npos);
				setAccept(accept);
				// std::cout << "accept: " << accept << std::endl;
			}
		} else if (line.find("Referer") != std::string::npos){
			if ((pos_begin = line.rfind(":")) != std::string::npos){
				pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
				std::string referer = line.substr(pos_begin, std::string::npos);
				setReferer(referer);
				// std::cout << "referer: " << referer << std::endl;
			}
		} else if (line.find("Accept-Encoding") != std::string::npos){
			if ((pos_begin = line.rfind(":")) != std::string::npos){
				pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
				std::string encoding = line.substr(pos_begin, std::string::npos);
				setAcceptEncoding(encoding);
				// std::cout << "encoding: " << encoding << std::endl;
			}
		} else if (line.find("Accept-Language") != std::string::npos){
			if ((pos_begin = line.rfind(":")) != std::string::npos){
				pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
				std::string languages = line.substr(pos_begin, std::string::npos);
				setAcceptLanguage(languages);
				// std::cout << "languages: " << languages << std::endl;
			}
		} 
		// content related (only applicable to POST and DEL)
		else if (line.find("Content-Type") != std::string::npos){
			if ((pos_begin = line.rfind(":")) != std::string::npos){
				pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
				std::string cttype = line.substr(pos_begin, std::string::npos);
				content_.setContentType(cttype);
				// std::cout << "Content-Type: " << cttype << std::endl;
			}
		} else if (line.find("Content-Length") != std::string::npos){
			if ((pos_begin = line.rfind(":")) != std::string::npos){
				pos_begin = line.find_first_not_of(" \t", pos_begin + 1);
				std::string ctlength = line.substr(pos_begin, std::string::npos);
				content_.setContentLength(ctlength);
				// std::cout << "Content-Length: " << ctlength << std::endl;
			}
		} else if (content_flag_ == true) {
			// std::cout << BOLD YELLOW "Checker\n" RESET;  
			this->parseContent(line);
		} else
			return;
	}
}

void HTTPRequest::parseRequest(const std::string& request){

	std::istringstream stream(request);
	std::string request_line;

	std::getline(stream, request_line);
	parseRequestLine(request_line);
	parseRequestHeader(stream);

	


	// size_t pos = request.find(" ");
	// if (pos != std::string::npos)
	// {
	//     std::string method = request.substr(0, pos);
	//     try
	//     {
	//         setMet("INVALID_METHOD");}
	//     catch (const std::invalid_argument& e)
	//     {
	//         std::cerr << "Error: " << e.what() << std::endl;}
	// }
}

//cgi
std::string HTTPRequest::getMethodAsStr() const //cgi needs it as a string
{
	switch (method_) {
		case GET: return "GET";
		case POST: return "POST";
		case DELETE: return "DELETE";
		default: return "UNKNOWN";
	}
}

std::string HTTPRequest::getQueryStr() const { return query_string_; }

std::map<std::string, std::string> HTTPRequest::getHeaders() const { return headers_; }

std::string HTTPRequest::getRawBody() const
{
	return content_.getBodyWithNoCD();
}

void HTTPRequest::setQueryStr(const std::string& query) { query_string_ = query; }
//std::string HTTPRequest::getcgiPath() { return path_; }