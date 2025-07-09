/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client copy.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 11:54:15 by cofische          #+#    #+#             */
/*   Updated: 2025/07/08 14:41:20 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/Client.hpp"

Client::Client(int inputClientFd, struct sockaddr_storage &inputClientAddr, socklen_t inputClientAddrLen): header_completed(false), file_sending_complete(true), 
	current_response(), current_request(), body_bytes_read(0), max_body_size(0), expected_content_length(0),
	state(CLIENT_READING_HEADERS), last_status_code_(0), error_(false), client_fd_(inputClientFd), client_addr_(inputClientAddr), client_addr_len_(inputClientAddrLen) {
	header_buffer = "";
	body_buffer = "";
	headers_string = "";
	pending_response = "";
	
	flags_ = fcntl(client_fd_, F_GETFL, 0);
	if (flags_ == -1)
		error_ = true;
	if (!error_) {
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
	}// to check if authorize otherwise re-write the function
}

Client::~Client() {
	if (current_response)
		delete current_response;
	if (file_stream.is_open())
		file_stream.close();
}

void Client::setResponse(HTTPResponse* response) {
	if (current_response != response) {
		delete current_response;
		current_response = response;
	}
	current_response = response;
	file_sending_complete = true;
	std::cerr << "CLIENT setReponse: Allocated HTTPResponse of size " << sizeof(HTTPResponse) << " at " << this->current_response << ", size of client response: " << sizeof(current_response) << std::endl;
}

void Client::setRequest(HTTPRequest* request) {
    if (current_request != request) {
        delete current_request;
        current_request = request;
    }
	current_request = request;
}

void Client::setLastStatusCode(int status_code) {
	last_status_code_ = status_code;
}


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
bool Client::getError() {
	return error_;
}
int Client::getLastStatusCode() {
	return last_status_code_;
}

void Client::resetForNextRequest() {
    DEBUG_PRINT(BOLD RED "Client::resetForNextRequest() called" RESET);
    header_buffer.clear();
    body_buffer.clear();
    headers_string.clear();
    pending_response.clear();
    body_bytes_read = 0;
    expected_content_length = 0;
    max_body_size = 0;
    header_completed = false;
    file_sending_complete = true;
    state = CLIENT_READING_HEADERS;
    if (file_stream.is_open()) {
        file_stream.close();
    }
   	DEBUG_PRINT(BOLD RED "Client::resetForNextRequest() exited" RESET);
}