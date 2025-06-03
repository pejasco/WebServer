/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 11:26:11 by cofische          #+#    #+#             */
/*   Updated: 2025/06/03 08:52:19 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Server.hpp"
#include "Socket.hpp"
#include "Client.hpp"
#include "HTTPResponse.hpp"


#define MAX_EVENTS 42

// typedef typename std::vector<Server*>::iterator Iterator;
class Server;
class Location;
class Client;
class Socket;

class ServerManager {
	public:
		ServerManager(const std::string &inputFilename);
		~ServerManager();

		void setHostPort();
		void setRunning(int inputRunning);
		
		std::vector<Server*> &getServers();
		std::map<std::string, std::string> &getHostPort();
		std::vector<Socket*> &getSocket();
		std::map<int,Client*> &getClients();
		int getEpollFd();
		
		int	readFile(std::fstream &configFile);
		void parseServer(std::string &line, Server *currentServer, std::fstream &configFile);
		void parseLocation(std::string &line, Server *currentServer, std::fstream &configFile);
		void startSockets();
		void startEpoll();
		void serverMonitoring();
		void createNewClientConnection();
		void existingClientConnection(Client *currentClient);
		bool cleanClient(int currentFd);
		void shutdown();

		
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
		char buffer[8192];

		
		// adding a vector that will keep track of the socket fd of each server ? 
		// Servermanager will be in charge of the epoll management
		
		

};

/*
class HTTPServer {
private:
    std::ofstream logFile;
    
public:
    HTTPServer() {
        logFile.open("server.log", std::ios::app);  // Append mode
    }
    
    ~HTTPServer() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
    
    void logRequest(const std::string& clientIP, const std::string& request) {
        if (logFile.is_open()) {
            logFile << "[" << getCurrentTime() << "] " 
                    << clientIP << " - " << request << std::endl;
        }
    }
    
    void logError(const std::string& error) {
        if (logFile.is_open()) {
            logFile << "[ERROR] " << error << std::endl;
        }
    }
    
    void handleRequest(int clientSocket) {
        // Handle request...
        logRequest("192.168.1.1", "GET /index.html HTTP/1.1");
    }
};

*/


#endif