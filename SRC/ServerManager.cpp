/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 11:26:00 by cofische          #+#    #+#             */
/*   Updated: 2025/05/14 12:35:57 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

bool server_flag = false;

#include "../INC/utils/ServerManager.hpp"

/************************/
/*CONSTRUCTOR/DESTRUCTOR*/
/************************/

ServerManager::ServerManager(const std::string &inputFilename) : running(true), epoll_fd(-1), num_events(-1), currentFd(-1) {
	std::cout << BOLD RED "Starting MasterServer\n" RESET;
	std::fstream configFile(inputFilename.c_str());
	readFile(configFile);
	setHostPort();
	startSockets();
	startEpoll();
	
	//check on error of the config file (ex: incorrect format, missing essential elements) 
	// --> can be place before the call of the object in main ??
	// --> Write an information on how to write a config file for our server (README.md?)

	/********DEBUGGING*********/
	//Printing the server object to ensure they are well connected	
	// std::vector<Server*>::iterator beg = servers.begin();
	// std::vector<Server*>::iterator end = servers.end();
	// for (; beg != end; ++beg)
	// 	printServer(**beg);
	// std::cout << "\n\n";
	// std::map<std::string, std::string>::iterator start = host_port.begin();
	// std::map<std::string, std::string>::iterator finish = host_port.end();
	// for (; start != finish; ++start) {	
	// 	std::cout << start->first << " " << start->second << std::endl; 
	// }

	// std::vector<Socket*>::iterator begSo = sockets.begin();
	// std::vector<Socket*>::iterator endSo = sockets.end();
	// for (; begSo != endSo; ++begSo) {
	// 	std::cout << (*begSo)->getSocketFd() << std::endl;
	// }
	
	// std::cout << line << std::endl;
	/********DEBUGGING*********/
	
	//clear all the resources

}

ServerManager::~ServerManager() {
	// //closing epoll instance
	// close(epoll_fd);

	// //closing socket_fd from the servers and freeing the struct
	// std::vector<Socket*>::iterator begSo = sockets.begin();
	// std::vector<Socket*>::iterator endSo = sockets.end();
	// for (; begSo != endSo; ++begSo) {
	// 	delete *begSo;
	// }

	// //closing and deleting client info
	// std::map<int,Client*>::iterator begCl = clients.begin();
	// std::map<int,Client*>::iterator endCl = clients.end();
	// for (; begCl != endCl; ++begCl) {
	// 	close(begCl->first);
	// 	delete begCl->second;
	// }

	// //freeing the struct server
	// std::vector<Server*>::iterator beg = servers.begin();
	// std::vector<Server*>::iterator end = servers.end();
	// for (; beg != end; ++beg)
	// 	delete *beg;


	std::cout << BOLD RED "Closing MasterServer\n" RESET;
}

/********/
/*SETTER*/
/********/


void ServerManager::setHostPort() {
	std::vector<Server*>::iterator start = servers.begin();
	std::vector<Server*>::iterator end = servers.end();
	for (; start != end; ++start) {
		if (*start != NULL) {
			std::vector<std::string>::iterator port_beg = (*start)->getPort().begin();
			std::vector<std::string>::iterator port_end = (*start)->getPort().end();
			for (; port_beg != port_end; ++port_beg) {
				host_port.insert(std::pair<std::string, std::string>(*port_beg, (*start)->getHost()));
			}
		} else
			return ;
	}
}

void ServerManager::setRunning(int inputRunning) {
	if (inputRunning)
		running = false;
}

/********/
/*GETTER*/
/********/

std::vector<Server*> &ServerManager::getServers() {
	return servers;
};
std::map<std::string, std::string> &ServerManager::getHostPort() {
	return host_port;
};
std::vector<Socket*> &ServerManager::getSocket() {
	return sockets;
};
std::map<int,Client*> &ServerManager::getClients() {
	return clients;
};
int ServerManager::getEpollFd() {
	return epoll_fd;
};


/********/
/*METHOD*/
/********/

/****************/
/* READFILE purpose is to read each line of the configFile and initiate a new Server Object when needed */
/****************/
int	ServerManager::readFile(std::fstream &configFile) {
	Server *currentServer =  NULL;
	std::string line;
	int serverCount = 1000; //Set an ID for new servers (MAY NOT BE USEFUL?)
	if (configFile.is_open()) {
		while (std::getline(configFile, line)) {
			// std::cout << line << std::endl;
			if (line.find("server {") != std::string::npos || server_flag == true) {				
				servers.push_back(new Server(serverCount++)); // newServerObj will return a pointer to a new Server object that is created
				currentServer = servers.back();
				server_flag = false;
				// create a new server object that will parse each line in a sub ServerConfig object.
			} else {
				parseServer(line, currentServer, configFile);
			}
		}
		configFile.close();
	} else {
		std::cout << BOLD RED "file incorrect\n" RESET;
	}
	return 0;
}


/****************/
/* PARSESERVER purpose is to parse information in the current server structure (like host, port, etc...)*/
/* if a location word is found, it call parseLocation to create an fill in Location object*/
/****************/
void ServerManager::parseServer(std::string &line, Server *currentServer, std::fstream &configFile) {
	size_t pos = 0;
	if (line.find("host") != std::string::npos) {
		if ((pos = line.find(":")) != std::string::npos)
			currentServer->setHost(line.substr(pos + 2));
	} else if (line.find("port") != std::string::npos) {
		if ((pos = line.rfind(":")) != std::string::npos)
			currentServer->setPort(line.substr(pos + 2));
	} else if (line.find("server_name") != std::string::npos) {
		if ((pos = line.rfind(":")) != std::string::npos)
			currentServer->addServerName(line.substr(pos + 2));
	} else if (line.find("error_pages") != std::string::npos) {
		while (std::getline(configFile, line) && line.find("}") == std::string::npos) {
			currentServer->setErrorDir(line);	
		}
	} else if (line.find("keep_alive") != std::string::npos) {
		if (line.find("on") != std::string::npos)
			currentServer->setKeepAlive(true);
	} else if (line.find("client_max_body_size") != std::string::npos) { // limit for the HTTP request body info
		if ((pos = line.rfind(":")) != std::string::npos) {
			std::string newLine = line.substr(pos + 2);
			newLine.erase(newLine.end() - 1);
			currentServer->setMaxSize(convertInt(newLine));
		}
	} else if (line.find("location") != std::string::npos) {
		parseLocation(line, currentServer, configFile);
		if (line.find("server") != std::string::npos)
			server_flag = true;
	} else
		return; // PRINTING ERROR AS UNKNOW CONFIG ELEMENT FOUND
}

/****************/
/* PARSELOCATION purpose is to initiate a new Location object in the current server object when needed or,*/
/* if already in the location block, just parse information in the location structure (like root, index, cgi_enable, etc...)*/
/****************/
void ServerManager::parseLocation(std::string &line, Server *currentServer, std::fstream &configFile) {
	size_t pos;
	std::string name;
	Location *currentLocation = NULL;
	if ((pos = line.rfind("/")) != std::string::npos)
		name = line.substr(pos);
	name.erase(name.end() - 1);
	currentServer->addLocation(name);
	currentLocation = currentServer->getLocation().back();
			//////////////////////////////////
	while (std::getline(configFile, line) && line.find("server") == std::string::npos) {
		if (line.find("location") != std::string::npos) {
			if ((pos = line.rfind("/")) != std::string::npos)
				name = line.substr(pos);
			currentServer->addLocation(name);
			currentLocation = currentServer->getLocation().back();
		} else if (line.find("method") != std::string::npos) {
			if ((pos = line.rfind(":")) != std::string::npos)
				currentLocation->setMethod(line.substr(pos + 2));
		} else if (line.find("root") != std::string::npos) {
			if ((pos = line.rfind(":")) != std::string::npos)
				currentLocation->setRoot(line.substr(pos + 2));
		} else if (line.find("index") != std::string::npos) {
			if ((pos = line.rfind(":")) != std::string::npos)
				currentLocation->setIndex(line.substr(pos + 2));
		} else if (line.find("directories") != std::string::npos) {
			if (line.find("on") != std::string::npos)
				currentLocation->setDirectories(true);
				//////////////////////////////////
		} else if (line.find("upload:") != std::string::npos) {
			if (line.find("on") != std::string::npos)
				currentLocation->setUpload(true);
		} else if (line.find("upload_store") != std::string::npos) {
			if ((pos = line.rfind(":")) != std::string::npos)
				currentLocation->setUploadDir(line.substr(pos + 2));
		} else if (line.find("max_body_size") != std::string::npos) {
			if ((pos = line.rfind(":")) != std::string::npos) {
				std::string newLine = line.substr(pos + 2);
				newLine.erase(newLine.end() - 1);
				currentLocation->setMaxBodySize(static_cast<size_t>(convertInt(newLine)));
			}
				/////////////////////////////////
		} else if (line.find("cgi:") != std::string::npos) {
			if (line.find("on") != std::string::npos)
				currentLocation->setCGI(true);
		} else if (line.find("cgi_extensions") != std::string::npos) {
			if ((pos = line.rfind(":")) != std::string::npos)
				currentLocation->setCGIExt(line.substr(pos + 2));	
				/////////////////////////////////
		} else if (line.find("redirect:") != std::string::npos) {
			if (line.find("on") != std::string::npos)
				currentLocation->setRedirect(true);
		} else if (line.find("redirect_code") != std::string::npos) { // do I need a redirect code or can I simply redirect to a default webpage?
			if ((pos = line.rfind(":")) != std::string::npos)
				currentLocation->setRedirectCode(convertInt(line.substr(pos + 2)));
		} else if (line.find("redirect_url") != std::string::npos) {
			if ((pos = line.find(":")) != std::string::npos)
				currentLocation->setRedirectURL(line.substr(pos + 2));
		} else if (line.find("}") != std::string::npos)
			;
		else
			return; // PRINTING ERROR AS UNKNOW CONFIG ELEMENT FOUND
	}	
}

/****************/
/* StartSockets purpose is to initiate the socket_fd per unique host:port combination that are saved in the attribute host_port*/
/* Each of the socket_fd will start a Socket object that will test the IP address, bind it and start listening*/
/****************/
void ServerManager::startSockets() {
	std::map<std::string, std::string>::iterator beg = host_port.begin();
	std::map<std::string, std::string>::iterator end = host_port.end();
	for (; beg != end; ++beg) {
		sockets.push_back(new Socket(beg->second, beg->first));
		socketsFdList.push_back(sockets.back()->getSocketFd());
	}
	// Now we got a vector that has all the socket_fd active and ready to listen. We are able to start the epoll after this function
}

/****************/
/*startEpoll --> setup and start an epoll instant that will monitore the socketFD from the servers*/
/****************/
void ServerManager::startEpoll() {
	/*STEP1 -- creation of the epoll instance*/
	epoll_fd = epoll_create(4096); // nb of socket_fd to monitor (BUT NOT IN USE NOWADAYS -- epoll_create1())
	if (epoll_fd == -1) {
		std::cerr << "Error pn epoll creation\n";
		return ;
	}
	/*STEP2 -- add the socket_fd to monitor in the epoll*/
	std::vector<int>::iterator begFd = socketsFdList.begin();
	std::vector<int>::iterator endFd = socketsFdList.end();
	for (; begFd != endFd; ++begFd) {
		struct epoll_event event;
		event.events = EPOLLIN;
		event.data.fd = *begFd;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, *begFd, &event) == -1) {
			std::cerr << "Error adding socket " << *begFd << " to epoll instance\n";
		}
	}
}

/****************/
/*serverMonitoring --> start the running loop where epoll instance is going to listen to any fd and wait for event to happen (meaning communication)*/
/*depending on the event, it can be either a new connection that need to be add to the epoll OR an existing one that will receive the HTTP request and send HTTP respond*/
/****************/
void ServerManager::serverMonitoring() {
	while (running) {
		/*STEP3 -- wait for an event to occur in any socket in epoll instance*/
		num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1); // -1 is timeout setup with -1 for infinite
		if (num_events == -1) {
			std::cerr << "Error epoll_wait: " << strerror(errno) << std::endl;
			return ;
		}
		
		/*STEP4 -- Process each event happening*/
		for (int i = 0; i < num_events; i++) {
			currentFd = events[i].data.fd;
			if (std::find(socketsFdList.begin(), socketsFdList.end(), currentFd) != socketsFdList.end()) { //add the check to clients list ??
				createNewClientConnection(); //is the currentFd in the epoll instance (meaning connections has been established so known client or need to be added as it is a new one)
			} else {
				Client *currentClient = clients[currentFd];
				if (currentClient == NULL)
					std::cout << "unknown client\n";
				existingClientConnection(currentClient);
			}
		}
		//running stop either when there is a huge problem with the epoll instance or when receiving a signal from the server to shutdown
		// running become false
		
	}
	/*LAST STEP -- clean the epoll instance and the structure (if needed but should be done in the destructor)*/
}

/****************/
/*createNewClientConnection --> when a connection is receive and it is part of the sockets FD list, */
/*it is a new connection from a client and it needs to be added to the epoll instance*/
/****************/
void ServerManager::createNewClientConnection() {
	/*STEP5 -- create a new socket FD for this client-server connection*/
	/*STEP6 -- make the client socket non-blocking with flags and fcntl*/
	temp_client_addr_len = sizeof(temp_client_addr);
	int temp_fd = accept(currentFd, (struct sockaddr*)&temp_client_addr, &temp_client_addr_len);
	if (temp_fd == -1) {
		std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
		return;
	}
	Client *newClient = new Client(temp_fd, temp_client_addr, temp_client_addr_len);
	int newClientFd = newClient->getClientFd();
	clients.insert(std::pair<int,Client*>(newClientFd, newClient));

	
	/*STEP7 -- add the client fd to the epoll monitoring to continue on same communication channel until closing of one fd*/
	struct epoll_event client_event;
	client_event.events = EPOLLIN;
	client_event.data.fd = newClientFd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, newClientFd, &client_event) == -1) {
		std::cerr << "error adding client_fd to epoll: " << strerror(errno) << std::endl;
		//clean client from structure
		close(newClientFd); 
		return ;
	}
}

/****************/
/* exisingClientConnection --> as the server-client connection is already in the epoll instance, */
/* the function can receive the message request and start analysing it */
/****************/
void ServerManager::existingClientConnection(Client *currentClient) {
	(void)currentClient;
	bool message_completed = false;
	std::string request;
	
	while (!message_completed) {
		ssize_t byte_received = recv(currentFd, received, sizeof(received), MSG_WAITALL); // or MSG_DONTWAIT depending on which flag we want for receiving data
		if (byte_received <= 0) {
			message_completed = cleanClient(currentFd);
			if (byte_received == 0)
				std::cout << "Client disconnect for server\n";
		} else {
			received[byte_received] = '\0';
			request.append(received);
			message_completed = isMessageCompleted(request);
		}	
	}
	/**START THE HTTP READING NOW**/
	
	/********DEBUGGING*********/
	std::cout << "\nrequest: \n" << request << std::endl;
	/********DEBUGGING*********/
	
	/**SEND THE RESPOND TO THE CLIENT**/
	
	/********DEBUGGING*********/
	const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 19\r\n\r\nYooooooooooooooooo!";
	ssize_t bytes_sent = send(currentFd, response, strlen(response), 0);
	if (bytes_sent == 0)
		std::cerr << "Error when sending response to client" << std::endl;
	/********DEBUGGING*********/
}

bool ServerManager::cleanClient(int currentFd) {
	std::map<int,Client*>::iterator itM;
	itM = clients.find(currentFd);
	if (itM != clients.end()) {
		close(currentFd);
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, currentFd, NULL);
		delete itM->second;
		clients.erase(itM);
		return true;
	} else
		return false;
}

void ServerManager::shutdown() {
	cleanShutdown(*this);
};