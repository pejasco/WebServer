/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chuleung <chuleung@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 11:54:18 by cofische          #+#    #+#             */
/*   Updated: 2025/06/08 01:54:46 by chuleung         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Webserv.hpp"
#include "HTTPResponse.hpp"

class HTTPResponse;

class Client {
	public:
		Client(int inputClientFd, struct sockaddr_storage &inputClientAddr, socklen_t inputClientAddrLen);
		~Client();

		void setResponse(HTTPResponse* response);

		int getClientFd();
		const void* getClientIP();
		uint16_t getClientPort();
		struct sockaddr_storage &getClientAddr();
		socklen_t getClientAddrLen();
		bool getError();
		
		//siev:
		std::string body_buffer;
		std::string header_buffer;
		size_t body_bytes_read;
		bool header_completed;
		std::ifstream file_stream;
    	bool file_sending_complete;
		HTTPResponse* current_response;

	private:
		bool error_;
		int client_fd_;
		struct sockaddr_storage client_addr_;
		socklen_t client_addr_len_;
		int flags_;
		char ip_str_[INET6_ADDRSTRLEN];
		const void *ip_;
		uint16_t port_;

};

#endif