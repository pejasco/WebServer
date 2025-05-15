/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 15:11:28 by cofische          #+#    #+#             */
/*   Updated: 2025/05/15 14:16:29 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "ServerManager.hpp"

class Server;
class Location;
class ServerManager;

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
bool fileExists(const std::string &filename);
std::string toLowerCase(const std::string &input);
int calculateFileSize(std::string &filename);



#endif