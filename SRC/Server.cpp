/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 15:32:27 by cofische          #+#    #+#             */
/*   Updated: 2025/04/25 11:55:52 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/Server.hpp"

/*CONSTRUCTORS/DESTRUCTORS*/
Server::Server(int inputID): Socket(), ID(inputID) {
	std::cout << BOLD YELLOW "Server is starting\nHELLO!\n" RESET;
	
	//As error page must be default if none are given in config file --> add a default error-page setup when creating server and erase if error are id

	
	// addlen = sizeof(this->getAddr());
	// std::cout << "Socketfd in the server class:" << &this->getSocketfd() << ", Socket add:" << this->getAddr() << "\n";
	
	// launch();
	// closeServer();
}

Server::~Server() {
	std::cout << BOLD RED "Server is closing\nGOODBYE!\n" RESET;
}

/*SETTER FUNCTIONS*/

void Server::setHost(const std::string &inputHost) {
	host = inputHost;
};

void Server::setPort(int inputPort) {
	port = inputPort;
};

void Server::setErrorDir(const std::string &inputErrorDir) {
	size_t pos;
	if (inputErrorDir.find("default") != std::string::npos) {
		if ((pos = inputErrorDir.rfind("t")) != std::string::npos)
			errors_list.push_back(new Errors(0, (inputErrorDir.substr(pos + 2))));
	} else {
		errors_list.push_back(new Errors(convertInt(inputErrorDir.substr(0,3)), inputErrorDir.substr(0 + 3)));	
	}
};

void Server::addServerName(const std::string &inputName) {
	std::stringstream ss(inputName);
	std::string token;

	while (std::getline(ss, token, ' ')) {
    	serverNames.push_back(token);
	}
};

void Server::setMaxSize(int inputMaxSize) {
	maxSize = inputMaxSize;
};

void Server::addLocation(const std::string &inputLocation) {
	
};

/*GETTER FUNCTIONS*/

std::string Server::getHost() {
	return host;
};

int Server::getPort() {
	return port;
};

std::vector<Errors*> &Server::getErrorDir() {
	return errors_list;
};

std::vector<std::string> &Server::getServerName() {
	return serverNames;
};

int Server::getMaxSize() {
	return maxSize;
};

std::vector<Location*> &Server::getLocation() {
	return locations;
};


/*METHOD*/
void Server::launch() {
	// while(true) {
		std::cout << "+++++++++ WAITING FOR CONNECTTION ++++++++++++++++++" << std::endl;
		if ((new_socket = accept(this->getSocketfd(), reinterpret_cast<sockaddr*>(this->getAddr()), &addlen)) < 0)
		{
			std::cout << "Error on accepting connection\n";
			return ;			
		}
		char buffer[30000] = {0};
		long valread = read(new_socket, buffer, 30000);
		std::cout << valread << std::endl << buffer << std::endl;
		close(new_socket);
	// }
}

int Server::closeServer() {
	close(this->getSocketfd());
	return 0;
}

int Server::getNewSocket() const {
	return new_socket;
}

void printServer(Server &server) {
	std::vector<std::string>::iterator it = server.getServerName().begin();
	std::vector<std::string>::iterator ite = server.getServerName().end();
	std::cout << BOLD BLUE << "server name: " RESET;
	for (; it != ite; ++it)
		std::cout << *it << " ";
	std::cout << BOLD BLUE "\nserver port: " << RESET << server.getPort() << std::endl
	<< BOLD BLUE "server host: " << RESET << server.getHost() << std::endl;
	std::vector<Errors*>::iterator itt = server.getErrorDir().begin();
	std::vector<Errors*>::iterator itte = server.getErrorDir().end();
	std::cout << BOLD BLUE << "error page: " RESET;
	for (; itt != itte; ++itt)
		std::cout << **itt << " ";
	std::cout << BOLD BLUE "\nserver maxsize: " << RESET << server.getMaxSize() << std::endl;
}
