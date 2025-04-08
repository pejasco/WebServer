/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 15:32:27 by cofische          #+#    #+#             */
/*   Updated: 2025/04/08 09:41:33 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/Server.hpp"

Server::Server(int domain, int type, int protocol, int port, int backlog) {
	mainSocket = Socket(domain, type, protocol, port, backlog);
	std::cout << "Socketfd in the server class:" << mainSocket.getSocketfd() << "\n";
	socklen_t addlen = sizeof(mainSocket.getAddr());
	while(true) {
		std::cout << "+++++++++ WAITING FOR CONNECTTION ++++++++++++++++++" << std::endl;
		if ((new_socket = accept(mainSocket.getSocketfd(), reinterpret_cast<sockaddr*>(mainSocket.getAddr()), &addlen)) < 0)
		{
			std::cout << "Error on accepting connection\n";
			return ;			
		}
		char buffer[30000] = {0};
		long valread = read(new_socket, buffer, 30000);
		std::cout << valread << std::endl << buffer << std::endl;
		close(new_socket);
	}
}

Server::~Server() {
	close(mainSocket.getSocketfd());
}

int Server::getNewSocket() const {
	return new_socket;
}