/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 11:37:06 by cofische          #+#    #+#             */
/*   Updated: 2025/04/25 11:54:16 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/Location.hpp"

Location::Location(const std::string &inputPath) : pathPattern(inputPath), autoindex(false), allow_uploads(false),
cgi_enabled(false), redirect(false), custom_error_page(false), auth_required(false) {
	upload_dir = "";
	max_body_size = 0;
	cgi_path = "";
	redirect_code = -1;
	redirect_url = "";
	auth_type = "";
	auth_user_file = "";
	request_timeout = -1;
	cgi_timeout = -1;	
};
Location::~Location() {};

//Setter
void Location::setRoot(const std::string &inputRoot) {
	root = inputRoot;
};
void Location::setMethod(const std::string &inputMethod) {
	std::stringstream ss(inputMethod);
	std::string token;
	while (std::getline(ss, token, ' ')) {
		if (token == "GET")
			method.push_back(GET);
		else if (token == "POST")
			method.push_back(POST);
		else if (token == "DELETE")
			method.push_back(DELETE);
	}
};
void Location::setIndex(const std::string &inputIndex) {
	index = inputIndex;
};
void Location::setAutoIndex(bool inputAIndex) {
	autoindex = inputAIndex;
};

//getter
std::string &Location::getRoot() {
	return root;
};
std::vector<MET> &Location::getMethod() {
	return method;
};
std::string &Location::getIndex() {
	return index;
};
bool Location::getAutoIndex() {
	return autoindex;
};

std::string &Location::getPath() {
	return pathPattern;
}