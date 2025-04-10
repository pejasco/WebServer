/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 15:28:29 by cofische          #+#    #+#             */
/*   Updated: 2025/04/10 11:41:57 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Webserv.hpp"
#include "Socket.hpp"
#include "Location.hpp"


class Server : public Socket{
	public:
		Server();
		~Server();
	
		int getNewSocket() const;
		void launch();
		int closeServer();
		
	private:
		int new_socket;
		socklen_t addlen;
		Socket mainSocket;
};

#endif