#include "http.hpp"
#include <iostream>

int http_CD_::global_index_ = 0;
int http_content::global_index_ = 0;

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
    ;
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




//############# http #############

http::http(): instance_index_(global_index_++) {
	// Constructor body
	std::cout << "http_content_ instance created with index: " << instance_index_ << std::endl;
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
    ;
}

void http::setContent(std::string content){
    ;
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

const std::pair<std::string, std::string> http::getContentType() const{
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
    if (line_input.find("Content-Type") != std::string::npos){
		if ((pos_begin = line_input.rfind(":")) != std::string::npos){
			pos_begin = line_input.find_first_not_of(" \t", pos_begin + 1);
			pos_end = line_input.find(";");
            std::string cttype = line_input.substr(pos_begin, pos_end - pos_begin);
			setContentType(cttype);
            


        }
    }
}

void http::printHttp(){
    std::cout << "global index: " << global_index_ << "\n";
    std::cout << "instance index: " << instance_index_ << "\n";
    std::cout << "method: "
    std::cout << "path: "
    std::cout << "format: "
    std::cout << "host: "
    std::cout << "boundary"
    std::cout <<
    std::cout <<
    std::cout <<
    std::cout <<
    std::cout <<
    std::cout <<
    std::cout <<
    std::cout <<
    std::cout <<






}