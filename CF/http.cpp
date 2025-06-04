#include "http.hpp"
#include <iostream>

int http_CD_::global_index_ = 0;
int http_content::global_index_ = 0;
int http::global_index_ = 0;

//######### http_CD_ ###########//

http_CD_::http_CD_() : instance_index_(global_index_++) {
	// Constructor body
	std::cout << "http_CD_ instance created with index: " << instance_index_ << std::endl;
}

http_CD_::~http_CD_() {
	// Destructor body
}

void http_CD_::setCDType_(std::string type){
    CD_type_ = type;
}

void http_CD_::setName_(std::string name){
    name_ = name;
}

void http_CD_::setFilename_(std::string filename){
    filename_ = filename;
}

void http_CD_::setInnerContentType_(std::string type){
    inner_content_type_ = type;
}

void http_CD_::setContent_(std::string content){
    content_ = content;
}

void http_CD_::setFileContent_(std::string file_content){
    file_content_ = file_content;
}

int http_CD_::getGlobalIndex_(){
    return global_index_;
}


const int http_CD_::getInstanceIndex_() const{
    return instance_index_;
}

const std::string http_CD_::getCDType_() const{
    return CD_type_;
}

const std::string http_CD_::getName_() const{
    return name_;
}

const std::string http_CD_::getFilename_() const{
    return filename_;
}

const std::string http_CD_::getInnerContentType_() const{
    return inner_content_type_;
}

const std::string http_CD_::getContent_() const{
    return content_;
}

const std::string http_CD_::getFileContent_() const{
    return file_content_;
}

void http_CD_::printHttpCD(){
    std::cout << "xxxxxxxxxxxx" << global_index_ <<"xxxxxxxxxxxxx" << "\n";
    std::cout << "global index: " << global_index_ << "\n";
    std::cout << "instance index: " << instance_index_ << "\n";
    std::cout << "CD_type: " << CD_type_ << "\n";
    std::cout << "name: " << name_ << "\n";
    std::cout << "filename: " << filename_ << "\n";
    std::cout << "inner_content_type: " << inner_content_type_ << "\n";
    std::cout << "content: " << content_ << "\n";
    std::cout << "file_content: " << file_content_ << "\n";
    std::cout << "xxxxxxxxxxxxxxxxxxxxxxxxx" << "\n";
}


//############# http_content #############

http_content::http_content(): instance_index_(global_index_++) {
	// Constructor body
	std::cout << "http_content_ instance created with index: " << instance_index_ << std::endl;
}

http_content::~http_content(){

}

void http_content::setBoundary(std::string boundary){
    boundary_ = boundary;
}

void http_content::setOpenBoundary(std::string boundary){
    open_boundary_ = boundary;
}

void http_content::setCloseBoundary(std::string boundary){
    close_boundary_ = boundary;
}

void http_content::setBodyWithNoCD(std::string body){
    body_with_no_cd_ = body;
}

void http_content::setHttpCD(std::string body){

}

void http_content::addHttpCD(){
    CDs_list_.push_back(http_CD_());
}


int http_content::getGlobalIndex(){
    return global_index_;
}

const int http_content::getInstanceIndex() const{
    return instance_index_;
}


const std::string http_content::getBoundary() const{
    return boundary_;
}

const std::string http_content::getOpenBoundary() const{
    return open_boundary_;
}

const std::string http_content::getCloseBoundary() const{
    return close_boundary_;
}

const std::string http_content::getBodyWithNoCD() const{
    return body_with_no_cd_;
}

std::vector<http_CD_>& http_content::getCDs_list_(){
    return CDs_list_;
}

void http_content::printHttpContent(){
    std::cout << "global index: " << global_index_ << "\n";
    std::cout << "instance index: " << instance_index_ << "\n";
    std::cout << "method: " << boundary_ << "\n";
    std::cout << "path: " << open_boundary_ << "\n";
    std::cout << "format: " << close_boundary_ << "\n";
    std::cout << "host: " << body_with_no_cd_ << "\n";
    std::cout << "boundary" << boundary_ << "\n";

    std::cout << "\nContent Dispositions List:\n";
    for(std::vector<http_CD_>::iterator it = CDs_list_.begin();
         it != CDs_list_.end(); ++it)
    {
        it->printHttpCD();
    }
}


//############# http #############

http::http(): instance_index_(global_index_++) {
	// Constructor body
	std::cout << "http instance created with index: " << instance_index_ << std::endl;
}

http::~http(){

}


void http::setMethod(std::string method){
    method_ = method;
}

void http::setPath(std::string path){
    path_ = path;
}

void http::setFormat(std::string format){
    format_ = format;
}

void http::setHost(std::string host){
    host_ = host;

}


void http::setContentType(std::string type){
    
}

void http::setContent(std::string content){
    ;
}

void http::setBoundary(std::string boundary){
    boundary_ = boundary;
    setOpenBoundary(("--" + boundary_));
    setCloseBoundary((open_boundary_ + "--"));
}


const std::string http::getMethod() const{
    return method_;
}

const std::string http::getPath() const{
    return path_;
}

const std::string http::getFormat() const{
    return format_;
}

const std::string http::getBoundary() const{
    return boundary_;
}

const std::string http::getOpenBoundary() const{
    return open_boundary_;
}

const std::string http::getCloseBoundary() const{
    return close_boundary_;
}

const std::map<std::string, std::string> http::getContentType() const{
    return content_type_;
}

http_content& http::getContent(){
    return content_;
}

void http::setHttp(std::string line_input){
    size_t pos_begin;
    size_t pos_end;

    if (line_input.find("Host") != std::string::npos){
        if ((pos_begin = line_input.rfind(":")) != std::string::npos)
        pos_begin = line_input.find_first_not_of(" \t", pos_begin + 1);
		std::string host = line_input.substr(pos_begin, std::string::npos);
        setHost(host);
    }
    else if (line_input.find("Content-Type") != std::string::npos){
		if ((pos_begin = line_input.rfind(":")) != std::string::npos){
			pos_begin = line_input.find_first_not_of(" \t", pos_begin + 1);
			pos_end = line_input.find(";");
            std::string cttype = line_input.substr(pos_begin, pos_end - pos_begin);
			setContentType(cttype);
            if (line_input.find("boundary") != std::string::npos){
                pos_begin = line_input.find("boundary") + 9;
                std::string boundary = line_input.substr(pos_begin, std::string::npos); 
                setBoundary(boundary);
            } 
        }
    }
    else if (line_input.find("Content-Length") != std::string::npos){
        if ((pos_begin = line_input.find(":")) != std::string::npos){
            pos_begin = line_input.find_first_not_of(" \t", pos_begin + 1);
            std::string length = line_input.substr(pos_begin);
            setContentLength(atoi(length.c_str()));
        }
    }
    //if able to find the boundary 
    else if (line_input.find(getBoundary().c_str())){
        


    }



}

void http::printHttp(){
    std::cout << "global index: " << global_index_ << "\n";
    std::cout << "instance index: " << instance_index_ << "\n";
    std::cout << "method: " << method_ << "\n";
    std::cout << "path: " << path_ << "\n";
    std::cout << "format: " << format_ << "\n";
    std::cout << "host: " << host_ << "\n";
    std::cout << "boundary" << boundary_ << "\n";
    for(std::map<std::string, std::string>::iterator it = content_type_.begin();
         it != content_type_.end(); ++it)
    {
        std::cout << "first: " << it->first << "\n";
        std::cout << "second: " << it->second << "\n";
    }

}