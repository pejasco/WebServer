/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 11:48:18 by cofische          #+#    #+#             */
/*   Updated: 2025/04/08 11:52:09 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <

// #include "WebServer.hpp"

// HTTP request class that will read from a client request, parse the information for server
// and send a respond with the correct information

class HTTP {
	public:
		HTTP();
		~HTTP();

		
	private:
		std::map <int, std::string> 	
		// int method;
		// std::string URI;
		// float HTTPVersion;
};


