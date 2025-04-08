/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 15:28:29 by cofische          #+#    #+#             */
/*   Updated: 2025/04/08 09:32:31 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "WebServer.hpp"
#include "Socket.hpp"


class Server {
	public:
		Server(int domain, int type, int protocol, int port, int backlog);
		~Server();
	
		int getNewSocket() const;
		
	private:
		int new_socket;
		Socket mainSocket;
};

#endif