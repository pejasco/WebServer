/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 11:54:18 by cofische          #+#    #+#             */
/*   Updated: 2025/05/06 14:21:51 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Webserv.hpp"

class Client {
	public:
		Client(int inputClientFd, struct sockaddr_storage &inputClientAddr, socklen_t inputClientAddrLen);
		~Client();

		int getClientFd();
		const void* getClientIP();
		uint16_t getClientPort();
		struct sockaddr_storage &getClientAddr();
		socklen_t getClientAddrLen();
		
	private:
		int clientFd;
		struct sockaddr_storage client_addr;
		socklen_t client_addr_len;
		int flags;
		char ip_str[INET6_ADDRSTRLEN];
		const void *ip;
		uint16_t port;
};

#endif