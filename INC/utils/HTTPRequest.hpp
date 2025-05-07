/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chuleung <chuleung@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:18:48 by chuleung          #+#    #+#             */
/*   Updated: 2025/05/07 17:40:28 by chuleung         ###   ########.fr       */
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

struct Accept
{
        Accept(std::string type, std::string subtype, float piority);
        ~Accept();

        static int global_index_; //dun know if it is useful
        int instance_index_;
        std::string type_;
        std::string subtype_;
        float piority_;

};

struct Sec_Fetch
{
        std::string sec_fetch_dect_;
        std::string sec_fetch_mode_;
        std::string sec_fetch_site_;
        std::string sec_fetch_user_;
};




class HTTPRequest {
	public:
		HTTPRequest();
		~HTTPRequest();
	
                //Setters:
                void setMet(const MET met_code);
                void setPath(const std::string& path);
                void setVersion(const std::string& version);
                void setHost(const std::string& host);
                void setUserAgent(const std::string& agent);
                void setAccept(const std::string& media_type);
                void setAcceptLanguage(const std::string& languages);
                void setConnection(const std::string& connection);
                // void setCookie(const std::string& version);
                void setAuthorization(const std::string& version);
                void setContent(const std::string& content);
                void setUnknown(const std::string& buffer);

                //Getters
                const std::string& getPath();
                const std::string& getVersion();
                const std::string& getHost();
                const std::string& getUserAgent();
                const std::vector<Accept>& getAccept();
                const std::map<std::string, int>& getAcceptLanguage();
                const bool getConnection();
                const std::pair<std::string, std::string>& getAuthorisation();
                const Content& getContent();
                const std::map <std::string, std::string>& getUnknown();

	private:
                static int global_index_; //dun know if it is useful
                int instance_index_;
                MET method_;

        // Common (GET, DELETE)
                std::string path_; // / //path/file.html
                std::string version_; //HTTP1.1
                std::string host_; //Host: example.com
                std::string user_agent_; //User-Agent identifies the client software
                std::vector<Accept> accept_list_; //Accept: media types the client can process
                std::map<std::string, int> accept_language_; //Accept-language:  en-US,en;q=0.5
                std::vector<std::string> accept_encoding_; //Accept-encoding: gzip, deflate, br, zstd
                bool connection_; // Connection: keep-alive
                // std::string cookie_; //Cookie: session=abc123; user=john
                
                Sec_Fetch fetch_data_;
                std::vector<std::string> priority;
                std::pair<std::string, std::string> authorisation_; //Authorization: Bearer p@sswoRd

        // POST
                Content content_;
                std::map <std::string, std::string> unknown_;
        //


};
