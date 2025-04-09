/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 15:03:14 by cofische          #+#    #+#             */
/*   Updated: 2025/04/08 11:21:30 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/Socket.hpp"

Socket::Socket() {};

Socket::Socket(int domain, int type, int protocol, int port, int backlog) : socketfd(-1) {
	//defining the add structure with info received from constructor
	memset(&add, 0, sizeof(add));
	add.sin_family = domain;
	add.sin_addr.s_addr = htonl(INADDR_ANY);
	add.sin_port = htons(port);
	
	try {
		socketfd = socket(domain, type, protocol);
	} catch (std::exception &e) {
		std::cerr << BOLD RED "Error\n" RESET "Socket failed on creation\n";
	}

	try {
		bind(socketfd, reinterpret_cast<struct sockaddr*>(&add), sizeof(add));
	} catch (std::exception &e) {
		std::cerr << BOLD RED "Error\n" RESET "Socket failed binding\n";
	}

	try {
		listen(socketfd, backlog);
	} catch (std::exception &e) {
		std::cerr << BOLD RED "Error\n" RESET "Socket failed listening\n";
	}
		
		
	// std::cout << "Socketfd in the socket class:" << &socketfd << ", Socket add:" << &add << "\n";
	// std::cout << "socket listening:" << socketfd << "\n";
};

Socket::~Socket() {};

int &Socket::getSocketfd(){
	return socketfd;
}

struct sockaddr_in *Socket::getAddr() {
	return &add;
};

Socket::SOCKERROR::SOCKERROR(const char *msg) : message(msg) {};
const char *Socket::SOCKERROR::what() const throw() { return message; };