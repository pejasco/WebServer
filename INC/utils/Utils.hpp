/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 15:11:28 by cofische          #+#    #+#             */
/*   Updated: 2025/07/08 16:59:49 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "ServerManager.hpp"
#include "HTTPResponse.hpp"

class Server;
class Location;
class ServerManager;
class HTTPRequest;

template <typename T>
std::string convertToStr(T nb) {
	std::stringstream str;
	str << nb;
	std::string newStr = str.str();
	return newStr;
}

template <typename T>
T convertToNb(const std::string &str) {
	T nb;
	std::istringstream (str) >> nb;
	//if no error
	return nb;
}

std::string safeSubstrAfter(const std::string& str, size_t pos, size_t offset = 1);
std::string extractValueAfterColon(const std::string& line, bool use_rfind = true);
void printServer(Server &server);
void printLocation(Location &location);
bool isMessageCompleted(const std::string &request);
void cleanShutdown(ServerManager &masterServer);
std::string getStatusStr(int status_code);
std::string getContentType(const std::string &inputExtension);
bool is_file_empty(const std::string &config_file);
bool fileExists(const std::string &filename);
std::string toLowerCase(const std::string &input);
std::string getServerIPPort(int socket_fd);
int calculateFileSize(std::string &filename);
size_t getMaxSize(const std::string &inputSize);
Server *getCurrentServer(const HTTPRequest &inputRequest, ServerManager &serverManager, const std::string &serverIP);
Location *getCurrentLocation(const HTTPRequest &inputRequest, Server &currentServer);
bool isFdOpen(int fd);
std::string getFilenameFromPath(const std::string& path);
std::string trimString(const std::string& s);
std::string urlDecoder(std::string &url_string);
int checkExtensions(Location *current_location, const std::string &script_name);
int URILength(const std::string &uri);
int fileDeletable(const std::string &body_filename, Location *location, Location *default_location);
std::string getDefautlErrorPage(int code);
bool isConfigFileOK(ServerManager &server_manager);
std::string resolvePathWithoutDuplication(const std::string &root_path, const std::string &request_url) ;

#endif