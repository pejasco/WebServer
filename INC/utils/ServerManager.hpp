/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 11:26:11 by cofische          #+#    #+#             */
/*   Updated: 2025/06/10 15:41:08 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Server.hpp"
#include "Socket.hpp"
#include "Client.hpp"
#include "HTTPResponse.hpp"
#include "HTTPRequest.hpp"


#define MAX_EVENTS 42
#define MAX_HEADER_SIZE 8192

// typedef typename std::vector<Server*>::iterator Iterator;
class Server;
class Location;
class Client;
class Socket;
class HTTPRequest;
class HTTPResponse;

class ServerManager {
	public:
		ServerManager(std::string &config_file);
		~ServerManager();

		void setHostPort();
		void setRunning(int is_running);
		
		std::vector<Server*> &getServers();
		std::map<std::string, std::string> &getIPPorts();
		std::vector<Socket*> &getSockets();
		std::vector<int> &getSocketsFD();
		std::map<int,Client*> &getClients();
		int getEpollFd();
		
		int	readFile(std::fstream &config_file);
		void parseServer(std::string &line, Server *current_server, std::fstream &config_file);
		void parseLocation(std::string &line, Server *current_server, std::fstream &config_file);
		int startSockets();
		int startEpoll();
		void serverMonitoring();
		void createNewClientConnection();
		void existingClientConnection();
		bool cleanClient(int current_fd);
		void shutdown();
		bool isBlocked(const void *IP);
		bool readClientHeaders();
		bool parseHeadersAndCheckBodySize(HTTPRequest& current_request);
		bool readRequestBody(HTTPRequest& current_request, size_t content_length, size_t max_body_size);
		void processAndSendResponse(HTTPRequest& current_request, Server *server_requested, Location *location_requested);
		bool sendResponseBodyFile();
		

		
	private:
		std::vector<Server*> servers_list_;
		std::map<std::string, std::string> IP_ports_list_; // MAY NOT BE USEFUL AS WE GOT SERVER ID 
		std::vector<Socket*> sockets_list_;
		std::vector<int> sockets_fd_list_;
		bool running_;
		Server *default_server_;

		/*EPOLL INSTANCE ATTRIBUTES*/
		int epoll_fd_;
		int num_events_;
		int current_fd_;
		struct epoll_event events_[MAX_EVENTS];
		std::string header_;
		std::string body_;
		int client_id_;

		/*CLIENT INFORMATION ATTRIBUTES*/
		struct sockaddr_storage temp_client_addr_;
		socklen_t temp_client_addr_len_;
		std::map<int,Client*> clients_list_;
		std::set<const void *> blocked_clients_list_;

		char received_[4096];
		char buffer_[8192];
		int error_code_;

};

size_t maxBodySizeLocation(Server *default_server, Server *server_requested, Location *location_requested);

#endif