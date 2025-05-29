/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 15:28:29 by cofische          #+#    #+#             */
/*   Updated: 2025/05/29 10:37:53 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Webserv.hpp"
#include "Location.hpp"

class Location;
class Errors;

class Server {
	public:
		Server(int inputID);
		~Server();
	
		//SETTER//
		void setHost(const std::string &inputHost);
		void setPort(const std::string &inputPort);
		void setErrorList(const std::string &inputErrorInfo);
		void addServerName(const std::string &inputName);
		void setMaxSize(size_t inputMaxSize);
		void addLocation(std::string &inputLocation);
		void setKeepAlive(bool inputAlive);

		//GETTER//
		std::string &getHost();
		std::vector<std::string> &getPort();
		std::map<int, std::string> &getErrorList();
		std::string &getErrorDir();
		std::vector<std::string> &getServerName();
		size_t getMaxSize();
		std::vector<Location*> &getLocation();
		bool getKeepAlive();
		
		//METHOD//
		// int getNewSocket() const;
		// void launch();
		// int closeServer();
		
		
	private:
		// Config settings
		int ID;
		std::string host;
		std::vector<std::string> port;
		std::vector<std::string> serverNames;
		size_t maxSize;
		bool keep_alive;

		// error_pages settings
		std::map<int, std::string> errors_list;
		std::string error_dir;
		// location settings
		std::vector<Location*> locations;
		
		// socket settings
		// int new_socket;
		// socklen_t addlen;
		// Socket mainSocket;
};

void printServer(Server &server); 

#endif