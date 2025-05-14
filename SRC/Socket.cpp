/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 15:03:14 by cofische          #+#    #+#             */
/*   Updated: 2025/05/14 17:44:45 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/Socket.hpp"

Socket::Socket() {};

Socket::Socket(const std::string &serverIP, const std::string &serverPort) : status(-1), socketFd(-1) {
	//defining the add structure with info received from constructor
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;      // Either IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;  // TCP
	hints.ai_flags = 0;               // Not using AI_PASSIVE since we provide an IP
	hints.ai_protocol = 0;
	
	status = getaddrinfo(serverIP.c_str(), serverPort.c_str(), &hints, &result);
	if (status == 0) {
		if (setSocketFd())
			;
	// 		std::cout << "Server listening on: " BOLD BLUE << serverIP << ":" << serverPort << RESET "\n";		
	}
	freeaddrinfo(result);
	
		
		
	// std::cout << "Socketfd in the socket class:" << &socketfd << ", Socket add:" << &add << "\n";
	// std::cout << "socket listening:" << socketfd << "\n";
};

Socket::~Socket() {
	close(socketFd);
	// std::cout << "closing the socket_fd\n";
};

int Socket::setSocketFd() {
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		if (rp == NULL) {
			std::cout << BOLD RED "Binding impossible" << RESET "\n";
			// return 0;
		}
		socketFd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		/*DEBUGGING FOR IP VERSION*/
		if (rp->ai_family == AF_INET) {
			// std::cout << "Creating IPv4 socket" << std::endl;
			// IPv4-specific code if needed
		} else if (rp->ai_family == AF_INET6) {
			// std::cout << "Creating IPv6 socket" << std::endl;
			// IPv6-specific code if needed
		}
		/*DEBUGGING FOR IP VERSION*/
		if (socketFd == -1)
			std::cout << "socketing failed\n";
			// return 0;
		int opt = 1;
		if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) > 0) {
				close(socketFd);
				std::cout << "setsockopt failed\n";
				// return 0;
		}
		if (bind(socketFd, rp->ai_addr, rp->ai_addrlen) < 0) {
			std::cerr << "binding failed " << strerror(errno) << "\n";
			return 0;
		}
		if (listen(socketFd, 10) < 0) { // 10 is number of max connections but can be changed
			close(socketFd);
			std::cout << "listening failed\n";
			// return 0; 
		}
	}
	return 1;
};

int &Socket::getSocketFd(){
	return socketFd;
}

struct sockaddr_in *Socket::getAddr() {
	return &add;
};

Socket::SOCKERROR::SOCKERROR(const char *msg) : message(msg) {};
const char *Socket::SOCKERROR::what() const throw() { return message; };