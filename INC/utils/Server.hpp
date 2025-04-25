/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 15:28:29 by cofische          #+#    #+#             */
/*   Updated: 2025/04/25 11:33:52 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Webserv.hpp"
#include "Socket.hpp"
#include "Location.hpp"
#include "Errors.hpp"


class Server : public Socket{
	public:
		Server(int inputID);
		~Server();
	
		//setter functions
		void setHost(const std::string &inputHost);
		void setPort(int inputPort);
		void setErrorDir(const std::string &inputErrorDir);
		void addServerName(const std::string &inputName);
		void setMaxSize(int inputMaxSize);
		void addLocation(const std::string &inputLocation);
		/*Add something for the Location object as locatino will be parsed in several lines*/
		std::string getHost();
		int getPort();
		std::vector<Errors*> &getErrorDir();
		std::vector<std::string> &getServerName();
		int getMaxSize();
		std::vector<Location*> &getLocation();
		
		int getNewSocket() const;
		void launch();
		int closeServer();
		
		
	private:
		//For configuration purpose
		int ID;
		std::string host;
		int port;
		std::vector<Errors*> errors_list; // if none are provided by the config file, we have to add manually defaults. 
		std::vector<std::string> serverNames;
		int maxSize;
		
		//location info
		std::vector<Location*> locations;
		
		// For server setup
		int new_socket;
		socklen_t addlen;
		Socket mainSocket;
};

// std::ostream &operator<<(std::ostream &os, Server &server);
void printServer(Server &server); 

#endif