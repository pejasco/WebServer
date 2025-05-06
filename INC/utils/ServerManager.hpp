/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 11:26:11 by cofische          #+#    #+#             */
/*   Updated: 2025/05/06 16:38:08 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Webserv.hpp"
#include "Server.hpp"
#include "Socket.hpp"
#include "Client.hpp"

#define MAX_EVENTS 42

// typedef typename std::vector<Server*>::iterator Iterator;

class ServerManager {
	public:
		ServerManager(const std::string &inputFilename);
		~ServerManager();
		
		int	readFile(std::fstream &configFile);
		void parseServer(std::string &line, Server *currentServer, std::fstream &configFile);
		void parseLocation(std::string &line, Server *currentServer, std::fstream &configFile);
		void setHostPort();
		void startSockets();
		void startEpoll();
		void serverMonitoring();
		void createNewClientConnection();
		void existingClientConnection(Client *currentClient);
		bool cleanClient(int currentFd);
		

		std::vector<Server*> &getServers();
		std::map<int, std::string> &getHostPort();
		std::vector<Socket*> &getSocket();
		
	private:
		std::vector<Server*> servers;
		std::map<std::string, std::string> host_port; // MAY NOT BE USEFUL AS WE GOT SERVER ID 
		std::vector<Socket*> sockets;
		std::vector<int> socketsFdList;
		bool running;

		/*EPOLL INSTANCE ATTRIBUTES*/
		int epoll_fd;
		int num_events;
		int currentFd;
		struct epoll_event events[MAX_EVENTS];

		/*CLIENT INFORMATION ATTRIBUTES*/
		struct sockaddr_storage temp_client_addr;
		socklen_t temp_client_addr_len;
		std::map<int,Client*> clients;

		char received[4096];

		
		// adding a vector that will keep track of the socket fd of each server ? 
		// Servermanager will be in charge of the epoll management
		
		

};


#endif