/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 15:32:27 by cofische          #+#    #+#             */
/*   Updated: 2025/07/04 12:36:16 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/Server.hpp"
#include "../INC/MasterHeader.hpp"

/************************/
/*CONSTRUCTOR/DESTRUCTOR*/
/************************/

Server::Server(int input_ID): ID_(input_ID), keep_alive_(false) {
	(void)ID_;
	error_directory_ = "";
	IP_ = "";
	server_names_.clear();
	errors_list_.clear();
	port_.clear();
	locations_list_.clear();
	max_body_size_ = 0;
	keep_alive_ = 0;
		
};

Server::~Server() {};

/********/
/*SETTER*/
/********/

void Server::setIP(const std::string &input_IP) {
	size_t pos = 0;
	if ((pos = input_IP.find("[")) != std::string::npos) {
		std::string newLine = input_IP.substr(pos + 1);
		newLine.erase(newLine.end() - 1);
		IP_ = newLine;
	}
	else
		IP_ = input_IP;
};

void Server::setPort(const std::string &input_port) {
	std::stringstream ss(input_port);
	std::string token;

	while (std::getline(ss, token, ' ')) {
		// for (std::vector<std::string>::iterator it = port_.begin(); 
		// 	it !=port_.end(); ++it){
		// 		if (*it == token){
		// 			exit(1); 
		// }  
		port_.push_back(token);
	}
};

void Server::setErrorList(const std::string &input_error_directory) {
	if (!input_error_directory.empty()) {
		std::stringstream ss(input_error_directory);
		std::string directory;
		size_t pos = 0;
		int code = 0;
		char space;
		ss >> code >> space >> directory;
		errors_list_.insert(std::pair<int, std::string>(code, directory));
		if ((pos = directory.rfind('/')) != std::string::npos)
			error_directory_ = directory.substr(0, pos + 1);
	}
};

void Server::addServerName(const std::string &input_names) {
    if (!input_names.empty()) {
        std::istringstream iss(input_names);
        std::string token;
        
        while (iss >> token)
            server_names_.push_back(token);
    }
};
void Server::setMaxSize(size_t input_max_body_size) {
	max_body_size_ = input_max_body_size;
};
void Server::addLocation(std::string &input_location) {
	std::string new_name = "";
	DEBUG_PRINT("input name: " << input_location);
	if (!input_location.empty()) {
		input_location.erase(input_location.length() - 1);
		new_name = safeSubstrAfter(input_location, 0, 0);
	}
	DEBUG_PRINT("new input name for location id: " << new_name << ", location id size: " << new_name.size());
	locations_list_.push_back(new Location(new_name));
};
void Server::setKeepAlive(bool is_alive) {
	keep_alive_ = is_alive;	
};

/********/
/*GETTER*/
/********/

std::string &Server::getIP() {
	return IP_;
};
std::vector<std::string> &Server::getPort() {
	return port_;
};
std::map<int,std::string> &Server::getErrorList() {
	return errors_list_;
};
std::string &Server::getErrorDirectory() {
	return error_directory_;
};
std::vector<std::string> &Server::getServerNames() {
	return server_names_;
};
size_t Server::getMaxBodySize() {
	return max_body_size_;
};
std::vector<Location*> &Server::getLocationsList() {
	return locations_list_;
};
bool Server::isKeepAlive() {
	return keep_alive_;	
};

/********/
/*METHOD*/
/********/


