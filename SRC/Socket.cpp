/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 15:03:14 by cofische          #+#    #+#             */
/*   Updated: 2025/04/08 09:36:47 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/Socket.hpp"

Socket::Socket() {};

Socket::Socket(int domain, int type, int protocol, int port, int backlog) {
	add.sin_family = domain;
	add.sin_addr.s_addr = htonl(INADDR_ANY);
	add.sin_port = htons(port);
	try {
		socketfd = socket(domain, type, protocol);
		std::cout << "Initial socket:" << socketfd << "\n";
		bind(socketfd, reinterpret_cast<struct sockaddr*>(&add), sizeof(add));
		listen(socketfd, backlog);
		std::cout << "socket listening:" << socketfd << "\n";
	} catch (std::exception &e) {
		std::cerr << BOLD RED "Error on socket creation\n" RESET;
	}
};
Socket::~Socket() {};

int Socket::getSocketfd() const{
	return socketfd;
}

struct sockaddr_in *Socket::getAddr() { //using a pointer or a reference ? Can it be null? is it worth creating a copy or calling the same? Is it going to overwrite ?
	return &add;
};

Socket::SOCKERROR::SOCKERROR(const char *msg) : message(msg) {};
const char *Socket::SOCKERROR::what() const throw() { return message; };