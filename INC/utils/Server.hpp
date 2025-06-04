/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 15:28:29 by cofische          #+#    #+#             */
/*   Updated: 2025/06/04 14:32:00 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Webserv.hpp"
#include "Location.hpp"

class Location;
class Errors;

class Server {
	public:
		Server(int input_ID);
		~Server();
	
		//SETTER//
		void setIP(const std::string &input_IP);
		void setPort(const std::string &input_port);
		void setErrorList(const std::string &input_error_list);
		void addServerName(const std::string &input_names);
		void setMaxSize(size_t input_max_body_size);
		void addLocation(std::string &input_locations);
		void setKeepAlive(bool is_alive);

		//GETTER//
		std::string &getIP();
		std::vector<std::string> &getPort();
		std::map<int, std::string> &getErrorList();
		std::string &getErrorDirectory();
		std::vector<std::string> &getServerNames();
		size_t getMaxBodySize();
		std::vector<Location*> &getLocationsList();
		bool isKeepAlive();
		
		//METHOD//
		// int getNewSocket() const;
		// void launch();
		// int closeServer();
		
		
	private:
		// Config settings
		int ID_;
		std::string IP_;
		std::vector<std::string> port_;
		std::vector<std::string> server_names_;
		size_t max_body_size_;
		bool keep_alive_;

		// error_pages settings
		std::map<int, std::string> errors_list_;
		std::string error_directory_;
		// location settings
		std::vector<Location*> locations_list_;
		
};

void printServer(Server &server); 

#endif