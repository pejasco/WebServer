/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 15:03:14 by cofische          #+#    #+#             */
/*   Updated: 2025/06/17 12:31:20 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/Socket.hpp"

Socket::Socket() {};

Socket::Socket(const std::string &serverIP, const std::string &serverPort) : status_(-1), socket_fd_(-1), error_(1) {
	//defining the add structure with info received from constructor
	result_ = NULL;
	memset(&hints_, 0, sizeof(hints_));
	hints_.ai_family = AF_UNSPEC;      // Either IPv4 or IPv6
	hints_.ai_socktype = SOCK_STREAM;  // TCP
	hints_.ai_flags = 0;               // Not using AI_PASSIVE since we provide an IP
	hints_.ai_protocol = 0;
	
	status_ = getaddrinfo(serverIP.c_str(), serverPort.c_str(), &hints_, &result_);
	if (status_ == 0) {
		if (!setSocketFd()) {
			std::cerr << BOLD RED "Error: " << strerror(errno) << RESET << std::endl;
			error_ = 0;
		}
			
	}
	if (result_)
		freeaddrinfo(result_);
};

Socket::~Socket() {
	// struct stat buf;
	// // if socket is not close, close it
	// if (fstat(socket_fd_, &buf) == -1) {
	// 	close(socket_fd_);
	// }
	// std::cout << "closing the socket_fd\n";
};

int Socket::setSocketFd() {
	for (rp_ = result_; rp_ != NULL; rp_ = rp_->ai_next) {
		if (rp_ == NULL) {
			std::cerr << BOLD RED "Error: Binding impossible\n" << RESET;
			return 0;
		}
		socket_fd_ = socket(rp_->ai_family, rp_->ai_socktype, rp_->ai_protocol);
		if (socket_fd_ == -1) {
			std::cerr << BOLD RED "Error: socketing failed\n" RESET;
			return 0;
		}
		int opt = 1;
		if (setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) > 0) {
				close(socket_fd_);
				std::cerr << BOLD RED "Error: setsockopt failed\n" RESET;
				return 0;
		}
		if (bind(socket_fd_, rp_->ai_addr, rp_->ai_addrlen) < 0) {
			std::cerr << BOLD RED "Error: binding failed\n" RESET;
			return 0;
		}
		if (listen(socket_fd_, 1000000) < 0) {
			close(socket_fd_);
			std::cerr << BOLD RED "Error: listening failed\n" RESET;
			return 0; 
		}
	}
	return 1;
};

int Socket::getSocketFd(){
	return socket_fd_;
}

int Socket::getSocketError() {
	return error_;
};

struct sockaddr_in *Socket::getAddr() {
	return &addr_;
};

Socket::SOCKERROR::SOCKERROR(const char *msg) : message(msg) {};
const char *Socket::SOCKERROR::what() const throw() { return message; };