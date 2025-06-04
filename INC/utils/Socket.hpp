/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 14:08:06 by cofische          #+#    #+#             */
/*   Updated: 2025/06/04 14:16:06 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "Webserv.hpp"

/*
struct sockaddr_in
{
	__uint8_t         sin_len;
	sa_family_t       sin_family;
	in_port_t         sin_port;
	struct in_addr    sin_addr;
	char              sin_zero[8];
};



// Initialize hints structure
memset(&hints, 0, sizeof(hints));
hints.ai_family = AF_UNSPEC;      // Either IPv4 or IPv6
hints.ai_socktype = SOCK_STREAM;  // TCP
hints.ai_flags = 0;               // Not using AI_PASSIVE since we provide an IP
hints.ai_protocol = 0;


*/

class Socket {
	public:
		Socket();
		Socket(const std::string &server_IP, const std::string &server_port);
		~Socket();

		int setSocketFd();

		int &getSocketFd();
		struct sockaddr_in *getAddr();
		
		class SOCKERROR: public std::exception {
			private:
				const char *message;
			public:
				SOCKERROR(const char *msg);
				virtual const char *what() const throw();
		};
		
	private:
		struct sockaddr_in addr_;
		struct addrinfo hints_;
		struct addrinfo *result_;
		struct addrinfo *rp_;
		int status_;
		int socket_fd_;
		
		
};

#endif