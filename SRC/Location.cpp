/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 11:37:06 by cofische          #+#    #+#             */
/*   Updated: 2025/06/04 14:09:07 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/Location.hpp"

/************************/
/*CONSTRUCTOR/DESTRUCTOR*/
/************************/

Location::Location(const std::string &input_name) : name_(input_name), auto_index_(false), upload_enable_(false),
cgi_enabled_(false), redirect_enable_(false) {
	upload_store_folder_ = "";
	max_body_size_ = 0;
	redirect_code_ = -1;
	redirect_url_ = "";
	root_ = "";
};
Location::~Location() {
	// std::cout << "deleting location\n";
};

/********/
/*SETTER*/
/********/

/*******BASIC INFO********/
void Location::setRoot(const std::string &input_root) {
	root_ = input_root;
};
void Location::setMethod(const std::string &input_method) {
	std::stringstream ss(input_method);
	std::string token;
	while (std::getline(ss, token, ' ')) {
		if (token == "GET")
			method_.push_back(GET);
		else if (token == "POST")
			method_.push_back(POST);
		else if (token == "DELETE")
			method_.push_back(DELETE);
	}
};
void Location::setIndex(const std::string &input_index_file) {
	index_file_ = input_index_file;
};
void Location::setAutoIndex(bool input_auto_index) {
	auto_index_ = input_auto_index;
};

/*******UPLOAD INFO********/
void Location::setUpload(bool is_upload) {
	upload_enable_ = is_upload;
};
void Location::setUploadDir(const std::string &input_dir) {
	upload_store_folder_ = input_dir;
};
void Location::setMaxBodySize(size_t input_size) {
	max_body_size_ = input_size;
};

/*******CGI INFO********/
void Location::setCGI(bool is_CGI) {
	cgi_enabled_ = is_CGI;
};
void Location::setCGIExt(const std::string &input_extensions) {
	std::stringstream ss(input_extensions);
	std::string token;

	while (std::getline(ss, token, ' ')) {
    	cgi_extensions_.push_back(token);
	}
};

/*******REDIRECT INFO********/
void Location::setRedirect(bool is_redirect) {
	redirect_enable_ = is_redirect;
};
void Location::setRedirectCode(int input_code) {
	redirect_code_ = input_code;
};
void Location::setRedirectURL(const std::string &input_URL) {
	redirect_url_ = input_URL;
};

/********/
/*GETTER*/
/********/

/*******BASIC INFO********/
std::string &Location::getRoot() {
	return root_;
};
std::vector<MET> &Location::getMethod() {
	return method_;
};
std::string &Location::getIndex() {
	return index_file_;
};
bool Location::isAutoIndex() {
	return auto_index_;
};
std::string &Location::getName() {
	return name_;
};

/*******UPLOAD INFO********/
bool Location::isUpload() {
	return upload_enable_;
};
std::string &Location::getUploadDir() {
	return upload_store_folder_;
};
size_t Location::getMaxSize() {
	return max_body_size_;
};

/*******CGI INFO********/
bool Location::isCGI() {
	return cgi_enabled_;
};
std::vector<std::string> &Location::getCGIExt() {
	return cgi_extensions_;
};

/*******REDIRECT INFO********/
bool Location::getRedirect() {
	return redirect_enable_;
};
int Location::getRedirectCode() {
	return redirect_code_;
};
std::string &Location::getRedirectURL() {
	return redirect_url_;
};

/********/
/*METHOD*/
/********/
