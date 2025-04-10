/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 15:32:27 by cofische          #+#    #+#             */
/*   Updated: 2025/04/10 13:28:29 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/Server.hpp"

/*CONSTRUCTORS/DESTRUCTORS*/
Server::Server(int inputID): ID(inputID), Socket() {
	std::cout << BOLD YELLOW "Server is starting\nHELLO!\n" RESET;
	
	addlen = sizeof(this->getAddr());
	
	// std::cout << "Socketfd in the server class:" << &this->getSocketfd() << ", Socket add:" << this->getAddr() << "\n";
	
	launch();
	closeServer();
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
	errorDir = inputErrorDir;
};

void Server::addServerName(const std::string &inputName) {
	serverNames.push_back(inputName);
};

void Server::setMaxSize(int inputMaxSize) {
	maxSize = inputMaxSize;
};

/*GETTER FUNCTIONS*/



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
