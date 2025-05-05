/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chuleung <chuleung@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:18:48 by chuleung          #+#    #+#             */
/*   Updated: 2025/05/05 20:29:52 by chuleung         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ServerManager.hpp"
//#include "HTTPResponse.hpp"
#include "Content.hpp"

//**define in Server
// enum MET {
// 	GET,
// 	POST,
// 	DELETE
// };

// METHOD PATH HTTP/VERSION
// Header1: Value1
// Header2: Value2
// ...
// Host: example.com

// [optional body data]

class Content;

class HTTPRequest {
	public:
		HTTPRequest();
		~HTTPRequest();
	
	private:
		MET method_;
        // Common (GET)
		std::string path_; // / //path/file.html
		std::string version_; //HTTP1.1
        std::string host_; //Host: example.com
        std::string user_agent_; //User-Agent identifies the client software
        std::string accept_; //Accept: media types the client can process
        std::string accept_language_; //Accept-language:  en-US,en;q=0.5
        std::string connection_; // Connection: keep-alive
        std::string cookie_; //Cookie: session=abc123; user=john
        std::string authorization_; //Authorization: Bearer p@sswoRd

        // POST
        Content content_;
        //

		


};
