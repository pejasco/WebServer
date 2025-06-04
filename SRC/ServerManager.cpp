/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 11:26:00 by cofische          #+#    #+#             */
/*   Updated: 2025/06/04 14:55:49 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/ServerManager.hpp"

bool server_flag = false;

/************************/
/*CONSTRUCTOR/DESTRUCTOR*/
/************************/

ServerManager::ServerManager(const std::string &input_config_file) : running_(true), epoll_fd_(-1), num_events_(-1), current_fd_(-1) {
	std::cout << BOLD RED "Starting MasterServer\n" RESET;
	std::fstream config_file(input_config_file.c_str());
	if (!readFile(config_file))
		return; 
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
	std::vector<Server*>::iterator start = servers_list_.begin();
	std::vector<Server*>::iterator end = servers_list_.end();
	for (; start != end; ++start) {
		if (*start != NULL) {
			std::vector<std::string>::iterator port_beg = (*start)->getPort().begin();
			std::vector<std::string>::iterator port_end = (*start)->getPort().end();
			for (; port_beg != port_end; ++port_beg) {
				IP_ports_list_.insert(std::pair<std::string, std::string>(*port_beg, (*start)->getIP()));
			}
		} else
			return ;
	}
}

void ServerManager::setRunning(int is_running) {
	running_= is_running;
}

/********/
/*GETTER*/
/********/

std::vector<Server*> &ServerManager::getServers() {
	return servers_list_;
};
std::map<std::string, std::string> &ServerManager::getIPPorts() {
	return IP_ports_list_;
};
std::vector<Socket*> &ServerManager::getSockets() {
	return sockets_list_;
};
std::vector<int> &ServerManager::getSocketsFD() {
	return sockets_fd_list_;
}
std::map<int,Client*> &ServerManager::getClients() {
	return clients_list_;
};
int ServerManager::getEpollFd() {
	return epoll_fd_;
};


/********/
/*METHOD*/
/********/

/****************/
/* READFILE purpose is to read each line of the config_file and initiate a new Server Object when needed */
/****************/
int	ServerManager::readFile(std::fstream &config_file) {
	Server *current_server =  NULL;
	std::string line;
	int server_ID = 1000; //Set an ID for new servers (MAY NOT BE USEFUL?)
	if (config_file.is_open()) {
		while (std::getline(config_file, line)) {
			// std::cout << line << std::endl;
			if (line.find("server {") != std::string::npos || server_flag == true) {				
				servers_list_.push_back(new Server(server_ID++)); // newServerObj will return a pointer to a new Server object that is created
				current_server = servers_list_.back();
				server_flag = false;
				// create a new server object that will parse each line in a sub ServerConfig object.
			} else {
				parseServer(line, current_server, config_file);
			}
		}
		config_file.close();
	} else {
		std::cerr << BOLD RED "Error:\n" RESET "file incorrect\n";
		return 0;
	}
	return 1;
}


/****************/
/* PARSESERVER purpose is to parse information in the current server structure (like host, port, etc...)*/
/* if a location word is found, it call parseLocation to create an fill in Location object*/
/****************/
void ServerManager::parseServer(std::string &line, Server *current_server, std::fstream &config_file) {
	size_t pos = 0;
	if (line.find("host") != std::string::npos) {
		if ((pos = line.find(":")) != std::string::npos)
			current_server->setIP(line.substr(pos + 2));
	} else if (line.find("port") != std::string::npos) {
		if ((pos = line.rfind(":")) != std::string::npos)
			current_server->setPort(line.substr(pos + 2));
	} else if (line.find("server_name") != std::string::npos) {
		if ((pos = line.rfind(":")) != std::string::npos)
			current_server->addServerName(line.substr(pos + 2));
	} else if (line.find("error_pages") != std::string::npos) {
		while (std::getline(config_file, line) && line.find("}") == std::string::npos) {
			current_server->setErrorList(line);	
		}
	} else if (line.find("keep_alive") != std::string::npos) {
		if (line.find("on") != std::string::npos)
			current_server->setKeepAlive(true);
	} else if (line.find("client_max_body_size") != std::string::npos) { // limit for the HTTP request body info
		if ((pos = line.rfind(":")) != std::string::npos) {
			current_server->setMaxSize(getMaxSize(line.substr(pos + 2)));
		}
	} else if (line.find("location") != std::string::npos) {
		parseLocation(line, current_server, config_file);
		if (line.find("server") != std::string::npos)
			server_flag = true;
	} else
		return; // PRINTING ERROR AS UNKNOW CONFIG ELEMENT FOUND
}

/****************/
/* PARSELOCATION purpose is to initiate a new Location object in the current server object when needed or,*/
/* if already in the location block, just parse information in the location structure (like root, index, cgi_enable, etc...)*/
/****************/
void ServerManager::parseLocation(std::string &line, Server *current_server, std::fstream &config_file) {
	size_t pos;
	std::string name;
	Location *current_location = NULL;
	if ((pos = line.rfind("/")) != std::string::npos)
		name = line.substr(pos);
	name.erase(name.end() - 1);
	current_server->addLocation(name);
	current_location = current_server->getLocationsList().back();
			//////////////////////////////////
	while (std::getline(config_file, line) && line.find("server") == std::string::npos) {
		if (line.find("location") != std::string::npos) {
			if ((pos = line.rfind("/")) != std::string::npos)
				name = line.substr(pos);
			current_server->addLocation(name);
			current_location = current_server->getLocationsList().back();
		} else if (line.find("method") != std::string::npos) {
			if ((pos = line.rfind(":")) != std::string::npos)
				current_location->setMethod(line.substr(pos + 2));
		} else if (line.find("root") != std::string::npos) {
			if ((pos = line.rfind(":")) != std::string::npos)
				current_location->setRoot(line.substr(pos + 2));
		} else if (line.find("autoindex") != std::string::npos) {
			if (line.find("on") != std::string::npos)
				current_location->setAutoIndex(true);
		} else if (line.find("index:") != std::string::npos) {
			if ((pos = line.rfind(":")) != std::string::npos)
				current_location->setIndex(line.substr(pos + 2));
		} else if (line.find("upload:") != std::string::npos) {
			if (line.find("on") != std::string::npos)
				current_location->setUpload(true);
		} else if (line.find("upload_store") != std::string::npos) {
			if ((pos = line.rfind(":")) != std::string::npos)
				current_location->setUploadDir(line.substr(pos + 2));
		} else if (line.find("max_body_size") != std::string::npos) {
			if ((pos = line.rfind(":")) != std::string::npos) {
				current_location->setMaxBodySize(getMaxSize(line.substr(pos + 2)));
			}
				/////////////////////////////////
		} else if (line.find("cgi:") != std::string::npos) {
			if (line.find("on") != std::string::npos)
				current_location->setCGI(true);
		} else if (line.find("cgi_extensions") != std::string::npos) {
			if ((pos = line.rfind(":")) != std::string::npos)
				current_location->setCGIExt(line.substr(pos + 2));	
				/////////////////////////////////
		} else if (line.find("redirect:") != std::string::npos) {
			if (line.find("on") != std::string::npos)
				current_location->setRedirect(true);
		} else if (line.find("redirect_code") != std::string::npos) { // do I need a redirect code or can I simply redirect to a default webpage?
			if ((pos = line.rfind(":")) != std::string::npos)
				current_location->setRedirectCode(convertToNb<int>(line.substr(pos + 2)));
		} else if (line.find("redirect_url") != std::string::npos) {
			if ((pos = line.find(":")) != std::string::npos)
				current_location->setRedirectURL(line.substr(pos + 2));
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
	std::map<std::string, std::string>::iterator beg = IP_ports_list_.begin();
	std::map<std::string, std::string>::iterator end = IP_ports_list_.end();
	for (; beg != end; ++beg) {
		sockets_list_.push_back(new Socket(beg->second, beg->first));
		sockets_fd_list_.push_back(sockets_list_.back()->getSocketFd());
	}
	// Now we got a vector that has all the socket_fd active and ready to listen. We are able to start the epoll after this function
}

/****************/
/*startEpoll --> setup and start an epoll instant that will monitore the socketFD from the servers*/
/****************/
void ServerManager::startEpoll() {
	/*STEP1 -- creation of the epoll instance*/
	epoll_fd_ = epoll_create(4096); // nb of socket_fd to monitor (BUT NOT IN USE NOWADAYS -- epoll_create1())
	if (epoll_fd_ == -1) {
		std::cerr << "Error pn epoll creation\n";
		return ;
	}
	/*STEP2 -- add the socket_fd to monitor in the epoll*/
	std::vector<int>::iterator begFd = sockets_fd_list_.begin();
	std::vector<int>::iterator endFd = sockets_fd_list_.end();
	for (; begFd != endFd; ++begFd) {
		struct epoll_event event;
		event.events = EPOLLIN;
		event.data.fd = *begFd;
		if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, *begFd, &event) == -1) {
			std::cerr << "Error adding socket " << *begFd << " to epoll instance\n";
		}
	}
}

/****************/
/*serverMonitoring --> start the running loop where epoll instance is going to listen to any fd and wait for event to happen (meaning communication)*/
/*depending on the event, it can be either a new connection that need to be add to the epoll OR an existing one that will receive the HTTP request and send HTTP respond*/
/****************/
void ServerManager::serverMonitoring() {
	while (running_) {
		/*STEP3 -- wait for an event to occur in any socket in epoll instance*/
		num_events_ = epoll_wait(epoll_fd_, events_, MAX_EVENTS, -1); // -1 is timeout setup with -1 for infinite
		if (num_events_ == -1) {
			if (errno != 4)
				std::cerr << "Error epoll_wait: " << strerror(errno) << std::endl;
			return ;
		}
		
		/*STEP4 -- Process each event happening*/
		for (int i = 0; i < num_events_; i++) {
			current_fd_ = events_[i].data.fd;
			if (std::find(sockets_fd_list_.begin(), sockets_fd_list_.end(), current_fd_) != sockets_fd_list_.end()) { //add the check to clients list ??
				createNewClientConnection(); //is the current_fd_ in the epoll instance (meaning connections has been established so known client or need to be added as it is a new one)
			} else {
				Client *current_client = clients_list_[current_fd_];
				if (current_client == NULL)
					std::cout << "unknown client\n";
				existingClientConnection(current_client);
			}
		}
		//running stop either when there is a huge problem with the epoll instance or when receiving a signal from the server to shutdown
		// running become false
		
	}
	this->cleanClient(current_fd_);
	this->shutdown();
}

/****************/
/*createNewClientConnection --> when a connection is receive and it is part of the sockets FD list, */
/*it is a new connection from a client and it needs to be added to the epoll instance*/
/****************/
void ServerManager::createNewClientConnection() {
	/*STEP5 -- create a new socket FD for this client-server connection*/
	/*STEP6 -- make the client socket non-blocking with flags and fcntl*/
	temp_client_addr_len_ = sizeof(temp_client_addr_);
	int temp_fd = accept(current_fd_, (struct sockaddr*)&temp_client_addr_, &temp_client_addr_len_);
	if (temp_fd == -1) {
		 //at this level, block so not accepting this client at all to avoid infinite loop or comment printing 
		std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
		return;
	}
	Client *new_client = new Client(temp_fd, temp_client_addr_, temp_client_addr_len_);
	int new_client_fd = new_client->getClientFd();
	clients_list_.insert(std::pair<int,Client*>(new_client_fd, new_client));

	
	/*STEP7 -- add the client fd to the epoll monitoring to continue on same communication channel until closing of one fd*/
	struct epoll_event client_event;
	client_event.events = EPOLLIN;
	client_event.data.fd = new_client_fd;
	if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, new_client_fd, &client_event) == -1) {
		std::cerr << "error adding client_fd to epoll: " << strerror(errno) << std::endl;
		//clean client from structure
		close(new_client_fd); 
		return ;
	}
}

/****************/
/* exisingClientConnection --> as the server-client connection is already in the epoll instance, */
/* the function can receive the message request and start analysing it */
/****************/
void ServerManager::existingClientConnection(Client *current_client) {
	(void)current_client;
	bool message_completed = false;
	std::string request;
	
	// RECEIVE CURRENT CLIENT REQUEST
	while (!message_completed) {
		ssize_t byte_received = recv(current_fd_, received_, sizeof(received_), MSG_WAITALL); // or MSG_DONTWAIT depending on which flag we want for receiving data
		if (byte_received <= 0) {
			message_completed = cleanClient(current_fd_);
			if (byte_received == 0)
				std::cout << "Client disconnect for server\n";
		} else {
			received_[byte_received] = '\0';
			request.append(received_);
			message_completed = isMessageCompleted(request);
		}	
	}
	/**START THE HTTP READING NOW**/
	HTTPRequest current_request;
	if (!request.empty()) {
		current_request.parseRequest(request);
		std::string serverIP = getServerIP(current_fd_);
		
		//SEND THE RESPONSE HEADER
		HTTPResponse current_response(current_request, *this, serverIP);
		std::string response = current_response.getResponse();
		std::cout << "response to send: " << response << std::endl;
		if (current_fd_ < 0)
			std::cerr << "Error, current_fD_ socket is already close: " << std::endl;
		if (response.empty())
			std::cerr << "Error, response is empty" << std::endl;
		ssize_t bytes_sent = send(current_fd_, response.c_str(), strlen(response.c_str()), 0);
		if (bytes_sent < 0) {
			std::cerr << "Error when sending response to client: " << strerror(errno) << std::endl;
			return ;
		}

		if (current_response.isReady()) {
			std::cout << "Response is already ready (e.g. from CGI), skipping body file send\n";
			close(current_fd_);
		return;
		}

		
		// SEND THE BODY FILE IF THERE IS ONE
		std::string body_file = current_response.getBodyFilename();
		if (!body_file.empty()) {
			std::cout << "check body_file: " << body_file << std::endl;
			std::ifstream file(body_file.c_str(), std::ios::binary);
			if (file.is_open()) {
				while (!file.eof()) {
					file.read(buffer_, sizeof(buffer_));
					std::streamsize bytes_read = file.gcount();
					if (bytes_read > 0) {
						bytes_sent = send(current_fd_, buffer_, bytes_read, 0);
						if (bytes_sent <= 0) { // Check for both error and connection closed
							if (bytes_sent < 0) {
								std::cerr << "Error sending file data: " << strerror(errno) << std::endl;
							} else {
								std::cerr << "Connection closed by client" << std::endl;
							}
							break; // Exit the loop on error
						}
						
						// Optional: Check for partial sends
						if (bytes_sent < bytes_read) {
							std::cerr << "Warning: Partial send - expected " << bytes_read 
									<< " bytes, sent " << bytes_sent << " bytes" << std::endl;
						}
					}
					
					// Check for file read errors
					if (file.bad()) {
						std::cerr << "Error reading from file: " << body_file << std::endl;
						break;
					}
				}
				file.close();
				if (current_response.isAutoIndex()) {
					if (!std::remove(body_file.c_str())) {
						current_response.setAutoIndex(false);
					}
				}
				body_file = "";
					
			}
		}
		std::cout << "SUCCESSFULLY REACH END OF RESPONSE!\n";
	}
	close(current_fd_);
	/********DEBUGGING*********/
	// std::cout << "\nrequest: \n" << request << std::endl;
	/********DEBUGGING*********/
	
	/**SEND THE RESPOND TO THE CLIENT**/
	

	
	/********DEBUGGING*********/
	// const char* response1 = "HTTP/1.1 200 OK\r\nContent-Length: 19\r\n\r\nYooooooooooooooooo!";
	// ssize_t bytes_sent = send(current_fd_, response1, strlen(response1), 0);
	// if (bytes_sent == 0)
	// 	std::cerr << "Error when sending response to client" << std::endl;
	/********DEBUGGING*********/
}

bool ServerManager::cleanClient(int current_fd_) {
	std::map<int,Client*>::iterator beg;
	beg = clients_list_.find(current_fd_);
	if (beg != clients_list_.end()) {
		close(current_fd_);
		epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, current_fd_, NULL);
		delete beg->second;
		clients_list_.erase(beg);
		return true;
	} else
		return false;
}

void ServerManager::shutdown() {
	cleanShutdown(*this);
};