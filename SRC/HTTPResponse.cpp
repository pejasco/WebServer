/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:19:25 by chuleung          #+#    #+#             */
/*   Updated: 2025/05/14 14:33:54 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/HTTPResponse.hpp"

HTTPResponse::HTTPResponse() {
	switch (HTTPRequest::getMethod()) {
	case GET:
		setGetResponse();
		break;
	case POST:
		setPostResponse();
		break;
	case DELETE:
		setDeleteResponse();
		break;
	default:
		setErrorResponse();
		break;
	}	
};
HTTPResponse::~HTTPResponse() {
	std::cout << "method destructor\n";
};

// SETTER

// GETTER
const std::string &HTTPResponse::getResponse() {
	return response;
};

//METHOD
void HTTPResponse::setGetResponse() {
	//1st -- check if the path request by the user exist
	//2nd -- check if the file exist and readable (not already open, with correct permission (not sure if we need to set it up)) 
	int status_code = checkFile(this->getPath());
	if (status_code == 200) {
		//3rdb -- if file is good, prepare response with 200 ok and content of file 
	} else {
		//3rda -- if file has error, prepare an error request (404, 400, 403, etc...)
	}

}

void HTTPResponse::setPostResponse() {
	
}

void HTTPResponse::setDeleteResponse() {
	
}

void HTTPResponse::setErrorResponse() {
	//if no GET, POST or DELETE, then it is a not allow method or bad request and send the error 
		// - 405
		// - 400 ?
}
