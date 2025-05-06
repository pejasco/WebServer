/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 11:54:15 by cofische          #+#    #+#             */
/*   Updated: 2025/05/06 14:36:02 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/Client.hpp"

Client::Client(int inputClientFd, struct sockaddr_storage &inputClientAddr, socklen_t inputClientAddrLen): clientFd(inputClientFd), client_addr(inputClientAddr), client_addr_len(inputClientAddrLen) {
	flags = fcntl(clientFd, F_GETFL, 0);
	fcntl(clientFd, F_SETFL, flags | O_NONBLOCK);
	
	if (client_addr.ss_family == AF_INET) {
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)&client_addr; // fill in information from the client addr structure to the ipv4 with sockaddr_in 
		ip = &(ipv4->sin_addr);
		port = ntohs(ipv4->sin_port); //network to host small function to convert the port info from the ipv4 structure 
	} else {
		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)&client_addr; // client info to ipv6 structure with sockaddr_in6 for formating. 
		ip = &(ipv6->sin6_addr);
		port = ntohs(ipv6->sin6_port);
	}
	inet_ntop(client_addr.ss_family, ip, ip_str, sizeof(ip_str));
	// to check if authorize otherwise re-write the function
}

Client::~Client() {}

int Client::getClientFd() {
	return clientFd;
}
const void *Client::getClientIP() {
	return ip;
}
uint16_t Client::getClientPort() {
	return port;
}
struct sockaddr_storage &Client::getClientAddr() {
	return client_addr;
}
socklen_t Client::getClientAddrLen() {
	return client_addr_len;
}