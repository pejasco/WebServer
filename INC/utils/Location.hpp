/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 11:36:56 by cofische          #+#    #+#             */
/*   Updated: 2025/04/24 17:06:58 by cofische         ###   ########.fr       */
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
		Location();
		~Location();

		//Setter
		void setRoot(const std::string &inputRoot);
		void setMethod(const MET inputMethod);
		void setIndex(const std::string &inputIndex);
		void setAutoIndex(bool inputAIndex);
		//getter
		std::string &getRoot();
		MET getMethod();
		std::string &getIndex();
		bool getAutoIndex();

	private:
		std::string root;
		MET method;
		std::string index;
		bool autoindex;
		
		/*
		adding the verification if restriction in the page
		adding the cgi check (prepare for handling)
		adding storage directory for file exchange
		adding redirect instruction
		*/
};

#endif