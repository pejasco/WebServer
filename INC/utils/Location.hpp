/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 11:36:56 by cofische          #+#    #+#             */
/*   Updated: 2025/06/04 14:09:00 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "Webserv.hpp"

class Location {
	public:
		Location(const std::string &input_name);
		~Location();

		//SETTER//
		/***BASIC INFO***/
		void setRoot(const std::string &input_root);
		void setMethod(const std::string &input_method);
		void setIndex(const std::string &input_index);
		void setAutoIndex(bool input_autoindex);
		
		/***UPLOAD INFO***/
		void setUpload(bool input_upload);
		void setUploadDir(const std::string &input_dir);
		void setMaxBodySize(size_t input_size);

		/***CGI INFO***/
		void setCGI(bool input_CGI);
		void setCGIExt(const std::string &input_extension);

		/***REDIRECTION INFO***/		
		void setRedirect(bool input_redirect);
		void setRedirectCode(int input_code);
		void setRedirectURL(const std::string &input_URL);
		
		//GETTER//
		/***BASIC INFO***/
		std::string &getRoot();
		std::vector<MET> &getMethod();
		std::string &getIndex();
		bool isAutoIndex();
		std::string &getName();
		
		/***UPLOAD INFO***/
		bool isUpload();
		std::string &getUploadDir();
		size_t getMaxSize();

		/***CGI INFO***/
		bool isCGI();
		std::vector<std::string> &getCGIExt();

		/***REDIRECTION INFO***/		
		bool getRedirect();
		int getRedirectCode();
		std::string &getRedirectURL();

		//METHOD//

	private:
		// Basic settings
		std::string name_; 	// name will be used to find a matching path when server receive the HTTP request
		std::string root_; 			// location of the folder
		std::vector<MET> method_; 	// methods allow for this location
		std::string index_file_; 			// set a default file to server when the request is for the directory
		bool auto_index_; 			// set if directory is listed or not 

		// Upload settings
		bool upload_enable_;			// Whether uploads are permitted
		std::string upload_store_folder_; 	// Directory where uploaded files are stored
		size_t max_body_size_;		// Maximum upload size in bytes

		// CGI handling
		bool cgi_enabled_; 			// Whether CGI is enabled for this location
		std::vector<std::string> cgi_extensions_; // Map file extensions to handlers (.php -> php-cgi)

		// Redirection
		bool redirect_enable_;		// Whether redirect is permitted or if automatically send an error
		int redirect_code_;			// 301 ex, to identify where to redirect in case of file not found
		std::string redirect_url_; 	// location of the redirect folder (ex Homepage)


		/************************************************/
		/***NOT MANDATORY -- TO COMPLETE LATER IF TIME***/
		/************************************************/
		/* Authentication
		bool auth_required;			// Basic auth required?
		std::string auth_type;		// "Basic", etc.
		std::string auth_user_file; // Path to credentials file
		*/
};

#endif