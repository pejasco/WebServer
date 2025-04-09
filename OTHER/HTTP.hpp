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

#ifndef HTTP_HPP
#define HTTP_HPP

#include "WebServer.hpp"

// HTTP request class that will read from a client request, parse the information for server
// and send a respond with the correct information

class HTTP {
	public:
		HTTP();
		~HTTP();

		
	private:
		int method;
		std::string URI;
		float HTTPVersion;
};


#endif