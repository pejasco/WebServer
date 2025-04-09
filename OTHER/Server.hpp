/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 15:28:29 by cofische          #+#    #+#             */
/*   Updated: 2025/04/08 12:09:13 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "WebServer.hpp"
#include "Socket.hpp"


class Server : public Socket{
	public:
		Server();
		Server(int configurationFile);
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