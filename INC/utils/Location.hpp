/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 11:36:56 by cofische          #+#    #+#             */
/*   Updated: 2025/04/25 11:29:09 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "Webserv.hpp"

enum MET {
	GET,
	POST,
	DELETE
};

class Location {
	public:
		Location(const std::string &inputName);
		~Location();

		//Setter
		void setRoot(const std::string &inputRoot);
		void setMethod(const std::string &inputMethod);
		void setIndex(const std::string &inputIndex);
		void setAutoIndex(bool inputAIndex);
		//getter
		std::string &getRoot();
		std::vector<MET> &getMethod();
		std::string &getIndex();
		bool getAutoIndex();
		std::string &getPath();

	private:
		//Basic settings
		std::string root;
		std::vector<MET> method;
		std::string index; //vector as well as there can be several index file 
		bool autoindex;
		std::string pathPattern; // name will be used to find a matching path when server receive the HTTP request

		// Upload settings
		bool allow_uploads;		// Whether uploads are permitted
		std::string upload_dir; // Directory for uploaded files
		size_t max_body_size;	// Maximum upload size in bytes

		// CGI handling
		bool cgi_enabled;								   // Whether CGI is enabled for this location
		std::string cgi_path;							   // Path to CGI executable/interpreter
		std::map<std::string, std::string> cgi_extensions; // Map file extensions to handlers (.php -> php-cgi)

		// Redirection
		bool redirect;			  // Is this a redirect location?
		int redirect_code;		  // 301, 302, etc.
		std::string redirect_url; // Where to redirect

		// Error handling
		bool custom_error_page;
		std::map<int, std::string> error_pages; // Custom error pages

		// Authentication
		bool auth_required;			// Basic auth required?
		std::string auth_type;		// "Basic", etc.
		std::string auth_user_file; // Path to credentials file

		// MIME types
		std::map<std::string, std::string> mime_overrides; // Custom MIME types for this location

		// Timeout settings
		int request_timeout; // Seconds to wait for complete request
		int cgi_timeout;
};

#endif