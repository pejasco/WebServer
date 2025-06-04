/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 11:54:15 by cofische          #+#    #+#             */
/*   Updated: 2025/06/04 13:39:04 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/Client.hpp"

Client::Client(int inputClientFd, struct sockaddr_storage &inputClientAddr, socklen_t inputClientAddrLen): client_fd_(inputClientFd), client_addr_(inputClientAddr), client_addr_len_(inputClientAddrLen) {
	flags_ = fcntl(client_fd_, F_GETFL, 0);
	fcntl(client_fd_, F_SETFL, flags_ | O_NONBLOCK);
	
	if (client_addr_.ss_family == AF_INET) {
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)&client_addr_; // fill in information from the client addr structure to the ipv4 with sockaddr_in 
		ip_ = &(ipv4->sin_addr);
		port_ = ntohs(ipv4->sin_port); //network to host small function to convert the port info from the ipv4 structure 
	} else {
		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)&client_addr_; // client info to ipv6 structure with sockaddr_in6 for formating. 
		ip_ = &(ipv6->sin6_addr);
		port_ = ntohs(ipv6->sin6_port);
	}
	inet_ntop(client_addr_.ss_family, ip_, ip_str_, sizeof(ip_str_));
	// to check if authorize otherwise re-write the function
}

Client::~Client() {}

int Client::getClientFd() {
	return client_fd_;
}
const void *Client::getClientIP() {
	return ip_;
}
uint16_t Client::getClientPort() {
	return port_;
}
struct sockaddr_storage &Client::getClientAddr() {
	return client_addr_;
}
socklen_t Client::getClientAddrLen() {
	return client_addr_len_;
}