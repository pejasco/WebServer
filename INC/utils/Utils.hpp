/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 15:11:28 by cofische          #+#    #+#             */
/*   Updated: 2025/06/04 16:23:56 by cofische         ###   ########.fr       */
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

void printServer(Server &server);
void printLocation(Location &location);
bool isMessageCompleted(const std::string &request);
void cleanShutdown(ServerManager &masterServer);
std::string getStatusStr(int status_code);
std::string getContentType(const std::string &inputExtension);
bool is_file_empty(const std::string &config_file);
bool fileExists(const std::string &filename);
std::string toLowerCase(const std::string &input);
std::string getServerIP(int socketFd);
int calculateFileSize(std::string &filename);
size_t getMaxSize(const std::string &inputSize);
Server *getCurrentServer(const HTTPRequest &inputRequest, ServerManager &serverManager, const std::string &serverIP);
Location *getCurrentLocation(const HTTPRequest &inputRequest, Server &currentServer);
// std::string formatURL(const std::string &input);
std::string getFilenameFromPath(const std::string& path);

#endif