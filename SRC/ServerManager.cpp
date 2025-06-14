/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 11:26:00 by cofische          #+#    #+#             */
/*   Updated: 2025/06/14 13:31:21 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/ServerManager.hpp"

bool server_flag = false;

/************************/
/*CONSTRUCTOR/DESTRUCTOR*/
/************************/

ServerManager::ServerManager(std::string &input_config_file) : running_(true), default_server_(NULL), epoll_fd_(-1), num_events_(-1), current_fd_(-1), error_code_(-1) {
	std::cout << BOLD RED "Starting MasterServer\n" RESET;
	if (is_file_empty(input_config_file))
		input_config_file = "configuration/default.conf";
	std::fstream config_file(input_config_file.c_str());

	if (!readFile(config_file))
		exit(EXIT_FAILURE); 
	setHostPort();
	if (!startSockets())
		exit(EXIT_FAILURE);
	if (!startEpoll())
		exit(EXIT_FAILURE);
	
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
int ServerManager::startSockets() {
	std::map<std::string, std::string>::iterator beg = IP_ports_list_.begin();
	std::map<std::string, std::string>::iterator end = IP_ports_list_.end();
	for (; beg != end; ++beg) {
		sockets_list_.push_back(new Socket(beg->second, beg->first));
		if (!sockets_list_.back()->getSocketError()) {
			delete sockets_list_.back();
			sockets_list_.pop_back();
			return 0;
		} else
			sockets_fd_list_.push_back(sockets_list_.back()->getSocketFd());
	}
	return 1;// Now we got a vector that has all the socket_fd active and ready to listen. We are able to start the epoll after this function
}

/****************/
/*startEpoll --> setup and start an epoll instant that will monitore the socketFD from the servers*/
/****************/
int ServerManager::startEpoll() {
	/*STEP1 -- creation of the epoll instance*/
	epoll_fd_ = epoll_create(4096); // nb of socket_fd to monitor (BUT NOT IN USE NOWADAYS -- epoll_create1())
	if (epoll_fd_ == -1) {
		std::cerr << "Error on epoll creation\n";
		return 0;
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
			close(*begFd); // closing the problematic socket to avoid error 
		}
	}
	return 1;
}

/****************/
/*serverMonitoring --> start the running loop where epoll instance is going to listen to any fd and wait for event to happen (meaning communication)*/
/*depending on the event, it can be either a new connection that need to be add to the epoll OR an existing one that will receive the HTTP request and send HTTP respond*/
/****************/
void ServerManager::serverMonitoring() {
	int loop = 2000;
	int nb_loop = 0;

	std::cout << "########[DEBUG] Server monitoring started ############" << std::endl;
	// while (running_) {
	while (running_ && nb_loop < loop) {
		nb_loop++;
		std::cout << "[DEBUG] Loop " << nb_loop << " - waiting for events..." << std::endl;
		
		num_events_ = epoll_wait(epoll_fd_, events_, MAX_EVENTS, -1);
		std::cout << "[DEBUG] epoll_wait returned " << num_events_ << " events" << std::endl;
		
		if (num_events_ == -1) {
			std::cerr << "[DEBUG] epoll_wait error: " << strerror(errno) << std::endl;
			if (errno != EINTR)
				std::cerr << "Error epoll_wait: " << strerror(errno) << std::endl;
			running_ = false;
			break;
		}
		
		for (client_id_ = 0; client_id_ < num_events_; client_id_++) {
			current_fd_ = events_[client_id_].data.fd;
			std::cout << "[DEBUG] Processing fd: " << current_fd_ << ", events: " << events_[client_id_].events << std::endl;
			
			// Check if this is a server socket
			bool is_server_socket = (std::find(sockets_fd_list_.begin(), sockets_fd_list_.end(), current_fd_) != sockets_fd_list_.end());
			std::cout << "[DEBUG] Is server socket: " << (is_server_socket ? "YES" : "NO") << std::endl;
			
			if (is_server_socket) {
				std::cout << "[DEBUG] Calling createNewClientConnection()" << std::endl;
				createNewClientConnection();
				std::cout << "########[DEBUG] Server monitoring ended ############\n" << std::endl;
			} else {
				std::cout << "[DEBUG] Looking for existing client for fd: " << current_fd_ << std::endl;
				Client *current_client = clients_list_[current_fd_];
				if (current_client == NULL) {
					std::cout << "[DEBUG] No client found for fd: " << current_fd_ << ", cleaning up" << std::endl;
					close(current_fd_);
					epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, current_fd_, NULL);
				} else {
					std::cout << "[DEBUG] Found client, calling existingClientConnection()" << std::endl;
					existingClientConnection();
				}
			}
		}
	}
	if (nb_loop > loop)
		std::cout << BOLD RED "[DEBUG] Reaching the loop limits" RESET << std::endl;
	
	std::cout << "[DEBUG] Server monitoring loop ended, cleaning up..." << std::endl;
	this->cleanClient(current_fd_);
	this->shutdown();
}

void ServerManager::createNewClientConnection() {
	std::cout << "[DEBUG] createNewClientConnection() called for server fd: " << current_fd_ << std::endl;
	
	temp_client_addr_len_ = sizeof(temp_client_addr_);
	std::cout << "[DEBUG] Calling accept() on fd: " << current_fd_ << std::endl;
	
	int temp_fd = accept(current_fd_, (struct sockaddr*)&temp_client_addr_, &temp_client_addr_len_);
	std::cout << "[DEBUG] accept() returned fd: " << temp_fd << std::endl;
	
	if (temp_fd == -1) {
		std::cerr << "[DEBUG] accept() failed: " << strerror(errno) << std::endl;
		return;
	}
	
	std::cout << "[DEBUG] Creating new Client object for fd: " << temp_fd << std::endl;
	Client *new_client = new Client(temp_fd, temp_client_addr_, temp_client_addr_len_);
	
	if (new_client->getError()) {
		std::cout << "[DEBUG] Client has error, adding to blocked list" << std::endl;
		blocked_clients_list_.insert(new_client->getClientIP());
	} else if (isBlocked(new_client->getClientIP())) {
		std::cout << "[DEBUG] Client IP is blocked, closing connection" << std::endl;
		close(new_client->getClientFd());
		std::cerr << BOLD RED "Error\n" RESET "Client has been blocked\n";
		delete new_client;
		return;
	}
	
	int new_client_fd = new_client->getClientFd();
	std::cout << "[DEBUG] Client fd: " << new_client_fd << ", IP: " << new_client->getClientIP() << std::endl;
	
	clients_list_.insert(std::pair<int,Client*>(new_client_fd, new_client));
	std::cout << "[DEBUG] Added client to clients_list_, total clients: " << clients_list_.size() << std::endl;

	// Add the client fd to epoll
	std::cout << "[DEBUG] Adding client fd " << new_client_fd << " to epoll" << std::endl;
	struct epoll_event client_event;
	client_event.events = EPOLLIN;
	client_event.data.fd = new_client_fd;
	
	if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, new_client_fd, &client_event) == -1) {
		std::cerr << "[DEBUG] epoll_ctl ADD failed: " << strerror(errno) << std::endl;
		close(new_client_fd);
		clients_list_.erase(new_client_fd);
		delete new_client;
		return;
	}
	
	std::cout << "[DEBUG] Successfully added client fd " << new_client_fd << " to epoll" << std::endl;
}

/****************/
/* exisingClientConnection --> as the server-client connection is already in the epoll instance, */
/* the function can receive the message request and start analysing it */
/****************/

void ServerManager::existingClientConnection() {
	std::cout << "[DEBUG] existingClientConnection() called for fd: " << current_fd_ << std::endl;
	
	Client* client = clients_list_[current_fd_];
	if (!client) {
		std::cout << "[DEBUG] No client found for fd: " << current_fd_ << std::endl;
		return;
	}
	
	std::cout << "\nClient state: " << client->state << ", file_sending_complete: " << client->file_sending_complete << std::endl;
	std::cout << "Events for client_id " << client_id_ << ": " << events_[client_id_].events << "\n" << std::endl;
	
	// Handle file sending if in progress
	if (!client->file_sending_complete) {
		std::cout << "[DEBUG] File sending in progress" << std::endl;
		if (events_[client_id_].events & EPOLLOUT) {
			std::cout << "[DEBUG] EPOLLOUT ready, calling sendResponseBodyFile()" << std::endl;
			if (sendResponseBodyFile()) {
				std::cout << "[DEBUG] File sending completed, resetting client" << std::endl;
				client->resetForNextRequest();
			}
			return;
		} else if (events_[client_id_].events & EPOLLIN) {
			std::cout << "[DEBUG] EPOLLIN during file transfer - resetting client state" << std::endl;
			// Client sending new data while file transfer in progress - reset state
			client->resetForNextRequest();
		} else {
			std::cout << "[DEBUG] Waiting for EPOLLOUT during file transfer" << std::endl;
			return; // Wait for EPOLLOUT
		}
	}
	
	 if (client->state == CLIENT_READY_TO_RESPOND && (events_[client_id_].events & EPOLLOUT)) {
        std::cout << BOLD CYAN "[DEBUG] Client ready to respond and EPOLLOUT available - sending response" RESET << std::endl;
        
        // Handle pending response data first
        if (!client->header_buffer.empty()) {
            std::cout << "[DEBUG] Sending pending response data: " << client->header_buffer.size() << " bytes" << std::endl;
            ssize_t bytes_sent = send(current_fd_, client->header_buffer.c_str(), client->header_buffer.length(), 0);
            std::cout << "\n$$$$$ SENDING PENDING RESPONSE INFORMATION $$$$$\n" << std::endl << client->header_buffer << std::endl;
			std::cout << "[DEBUG] send() returned: " << bytes_sent << " bytes" << std::endl;
            
            if (bytes_sent <= 0) {
                std::cout << "[DEBUG] Error sending pending response" << std::endl;
                return; // Error or would block
            }
            
            if (bytes_sent < static_cast<ssize_t>(client->header_buffer.length())) {
                // Still partial - update pending data
                std::cout << "[DEBUG] Still partial send: " << bytes_sent << "/" << client->header_buffer.length() << " bytes sent" << std::endl;
                client->header_buffer = client->header_buffer.substr(bytes_sent);
                return;
            }
            
            // All pending data sent
            std::cout << "[DEBUG] All pending response data sent" << std::endl;
            client->header_buffer.clear();
        }
        
        // Check if response is complete
        if (client->current_response && client->current_response->isReady()) {
            std::cout << "[DEBUG] Response has been sent, resetting client for next request" << std::endl;
            // Switch back to EPOLLIN for next request
			std::cout << "[DEBUG] Switching epoll back to EPOLLIN" << std::endl;
			struct epoll_event event;
			event.events = EPOLLIN;
			event.data.fd = current_fd_;
			int epoll_result = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, current_fd_, &event);
			std::cout << "[DEBUG] epoll_ctl result: " << epoll_result << std::endl;
			client->resetForNextRequest();
            return;
        }
        
        // Check if there's a body file to send
        if (client->current_response && !client->current_response->getBodyFilename().empty()) {
            std::cout << "[DEBUG] Response has body file: " << client->current_response->getBodyFilename() << std::endl;
            client->file_sending_complete = false;
            sendResponseBodyFile();
        } else {
            std::cout << "[DEBUG] No body file, resetting client for next request" << std::endl;
            client->resetForNextRequest();
        }
        
        return;
    }

	// Handle new request data - ONLY if EPOLLIN is set
	if (events_[client_id_].events & EPOLLIN) {
		std::cout << "[DEBUG] EPOLLIN ready, processing request data" << std::endl;
		
		// State machine approach
		switch (client->state) {
			case CLIENT_READING_HEADERS:
				std::cout << BOLD BLUE "[DEBUG] State: CLIENT_READING_HEADERS" RESET << std::endl;
				if (readClientHeaders()) {
					std::cout << "[DEBUG] Headers read successfully, transitioning to CLIENT_PROCESSING_HEADERS" << std::endl;
					client->state = CLIENT_PROCESSING_HEADERS;
					// Fall through to process headers
				} else {
					std::cout << "[DEBUG] Need more header data" << std::endl;
					return; // Need more data
				}
				
			case CLIENT_PROCESSING_HEADERS: {
				std::cout << BOLD MAGENTA "[DEBUG] State: CLIENT_PROCESSING_HEADERS" RESET << std::endl;
				if (!parseHeadersAndCheckBodySize()) {
					std::cout << "[DEBUG] Header parsing failed, closing connection" << std::endl;
					if (current_fd_ > 0) 
						close(current_fd_);
					return;
				}
				
				int content_length = client->current_request->getContentLength();
				std::cout << "[DEBUG] Content-Length: " << content_length << std::endl;
				if (content_length > 0 && error_code_ < 0) {
					client->expected_content_length = content_length;
					client->state = CLIENT_READING_BODY;
					std::cout << "[DEBUG] Transitioning to CLIENT_READING_BODY" << std::endl;
					// Fall through to read body
				} else {
					client->state = CLIENT_READY_TO_RESPOND;
					std::cout << "[DEBUG] No body to read, transitioning to CLIENT_READY_TO_RESPOND" << std::endl;
					// Server/location resolution happens in parseHeadersAndCheckBodySize
					// and response is prepared there for requests without body
					return;
				}
			}
				
			case CLIENT_READING_BODY: {
				std::cout << BOLD GREEN "[DEBUG] State: CLIENT_READING_BODY" RESET << std::endl;
				int result = readRequestBody(client->expected_content_length, client->max_body_size);
				std::cout << "[DEBUG] readRequestBody() returned: " << result << std::endl;
				if (result == 1) {
					// Body complete - response preparation happens in readRequestBody
					client->state = CLIENT_READY_TO_RESPOND;
					std::cout << "[DEBUG] Body reading complete, transitioning to CLIENT_READY_TO_RESPOND" << std::endl;
				} else if (result == -1) {
					// Error
					std::cout << "[DEBUG] Error reading body, closing connection" << std::endl;
					if (current_fd_ > 0) 
						close(current_fd_);
					return;
				} else
					std::cout << "[DEBUG] Need more body data" << std::endl;
				break;
			}
				
			case CLIENT_READY_TO_RESPOND:
				std::cout << BOLD YELLOW "[DEBUG] State: CLIENT_READY_TO_RESPOND - should not reach here with EPOLLIN" RESET << std::endl;
				// Should not reach here with EPOLLIN
				break;
		}
	} else {
		std::cout << "[DEBUG] No EPOLLIN event, nothing to process" << std::endl;
	}
	
	std::cout << "[DEBUG] existingClientConnection() finished" << std::endl;
}

bool ServerManager::readClientHeaders() {
	std::cout << "[DEBUG] readClientHeaders() called for fd: " << current_fd_ << std::endl;
	
	Client* client = clients_list_[current_fd_];
	if (!client) {
		std::cout << "[DEBUG] No client found for fd: " << current_fd_ << std::endl;
		return false;
	}
	
	// CRITICAL: Only read if epoll says data is available
	if (!(events_[client_id_].events & EPOLLIN)) {
		std::cout << "[DEBUG] No EPOLLIN event, cannot read" << std::endl;
		return false;
	}

	std::cout << "[DEBUG] Attempting to read headers from socket" << std::endl;
	ssize_t byte_received = recv(current_fd_, received_, sizeof(received_) - 1, 0);    
	std::cout << "[DEBUG] recv() returned: " << byte_received << " bytes" << std::endl;
	
    if (byte_received <= 0) {
        if (byte_received == 0) {
            std::cout << "[DEBUG] Client disconnected gracefully - cleaning up connection" << std::endl;
        } else {
            std::cout << "[DEBUG] Error during header reading - cleaning up connection" << std::endl;
        }
        cleanClient(current_fd_);
        
        return false; // Connection closed or error
    }
	
	received_[byte_received] = '\0';
	std::cout << "[DEBUG] Header buffer size before append: " << client->header_buffer.size() << std::endl;
	client->header_buffer.append(received_, byte_received);
	std::cout << "[DEBUG] Header buffer size after append: " << client->header_buffer.size() << std::endl;
	
	// Check for end of headers
	size_t header_end = client->header_buffer.find("\r\n\r\n");
	if (header_end != std::string::npos) {
		std::cout << "[DEBUG] Found end of headers at position: " << header_end << std::endl;
		client->headers_string = client->header_buffer.substr(0, header_end + 4);
		std::cout << "[DEBUG] Header string after split with body: \n" << client->headers_string << std::endl;
		size_t body_start = header_end + 4;
		if (body_start < client->header_buffer.length()) {
			client->body_buffer = client->header_buffer.substr(body_start);
			client->body_bytes_read = client->body_buffer.length();
			std::cout << "[DEBUG] Found " << client->body_bytes_read << " body bytes in header buffer" << std::endl;
		}
		client->header_completed = true;
		std::cout << "[DEBUG] Headers completed successfully" << std::endl;
		return true;
	}
	
	// Header size check 
	if (client->header_buffer.size() > MAX_HEADER_SIZE) {
		std::cout << "[DEBUG] Header size exceeded: " << client->header_buffer.size() << " > " << MAX_HEADER_SIZE << std::endl;
		error_code_ = 431;
		client->header_completed = true;
		std::cout << "[DEBUG] Headers too large -- sending HTTP error" << std::endl;
		return true;
	}
	
	std::cout << "[DEBUG] Headers not complete yet, need more data" << std::endl;
	return false; // Need more header data
}

bool ServerManager::parseHeadersAndCheckBodySize() {
	std::cout << "[DEBUG] parseHeadersAndCheckBodySize() called" << std::endl;
	
	Client* client = clients_list_[current_fd_];
	if (!client) {
		std::cout << "[DEBUG] No client found for fd: " << current_fd_ << std::endl;
		return false;
	}
	HTTPRequest* http_request = new HTTPRequest();
	std::cout << "[DEBUG] Parsing request headers" << std::endl;
	std::cout << client->headers_string << "\nheader_buffer:\n" << client->header_buffer << std::endl;
	http_request->parseRequest(client->headers_string);
	client->setRequest(http_request);
	
	// Server/location resolution - done once here
	std::string server_IP = getServerIP(current_fd_);
	std::cout << "[DEBUG] Server IP: " << server_IP << std::endl;
	
	default_server_ = servers_list_.front();
	Server *server_requested = getCurrentServer((*client->current_request), *this, server_IP);
	Location *location_requested = getCurrentLocation((*client->current_request), *server_requested);
	
	std::cout << "[DEBUG] Server requested: " << (server_requested ? "found" : "null") << std::endl;
	std::cout << "[DEBUG] Location requested: " << (location_requested ? "found" : "null") << std::endl;
	
	if (location_requested == NULL && (client->current_request->getPath() == "/")) {
		location_requested = servers_list_.front()->getLocationsList().front();
		std::cout << "[DEBUG] Using default location for root path" << std::endl;
	}
	
	int max_body_size = maxBodySizeLocation(servers_list_.front(), server_requested, location_requested);
	int content_length = client->current_request->getContentLength();
	
	std::cout << "[DEBUG] Max body size: " << max_body_size << std::endl;
	std::cout << "[DEBUG] Content length: " << content_length << std::endl;
	
	if (content_length > 0) {
		if (max_body_size > 0 && content_length > max_body_size) {
			std::cout << "[DEBUG] Request body size " << content_length 
					  << " exceeds limit " << max_body_size << std::endl;
			error_code_ = 413;
			processAndSendResponse(server_requested, location_requested);
		}
		// Store max_body_size for later use in body reading
		client->max_body_size = max_body_size;
		std::cout << "[DEBUG] Stored max_body_size: " << max_body_size << " for body reading" << std::endl;
	} else {
		// No body - process response immediately
		std::cout << "[DEBUG] No body to read, processing response immediately" << std::endl;
		processAndSendResponse(server_requested, location_requested);
	}
	
	std::cout << "[DEBUG] parseHeadersAndCheckBodySize() completed successfully" << std::endl;
	return true;
}

int ServerManager::readRequestBody(size_t content_length, size_t max_body_size) {
	std::cout << "[DEBUG] readRequestBody() called - content_length: " << content_length 
			  << ", max_body_size: " << max_body_size << std::endl;
	
	Client* client = clients_list_[current_fd_];
	if (!client) {
		std::cout << "[DEBUG] No client found for fd: " << current_fd_ << std::endl;
		return -1;
	}
	
	// Use stored max_body_size from parseHeadersAndCheckBodySize
	max_body_size = client->max_body_size;
	std::cout << "[DEBUG] Using stored max_body_size: " << max_body_size << std::endl;
	
	std::cout << "Content_length of file: " << content_length << std::endl;
	std::cout << "client body byte before reading: " << client->body_bytes_read << std::endl;
	
	if (client->body_bytes_read < content_length) {
		// CRITICAL: Only read if epoll says data is available
		if (!(events_[client_id_].events & EPOLLIN)) {
			std::cout << "[DEBUG] No EPOLLIN event, cannot read body data" << std::endl;
			return 0; // No data available, try again later
		}
		
		size_t remaining = content_length - client->body_bytes_read;
		size_t to_read = std::min(sizeof(received_) - 1, remaining);
		std::cout << "[DEBUG] Attempting to read " << to_read << " bytes from socket" << std::endl;
		ssize_t byte_received = recv(current_fd_, received_, to_read, 0);
		
		std::cout << "\nremaining: " << remaining << ", to_read: " << to_read 
				  << ", byte_received: " << byte_received << std::endl;
		
		// DON'T CHECK ERRNO - per requirements
    	if (byte_received <= 0) {
        	if (byte_received == 0) {
            	std::cout << "[DEBUG] Client disconnected gracefully - cleaning up connection" << std::endl;
        	} else {
            	std::cout << "[DEBUG] Error during header reading - cleaning up connection" << std::endl;
        	}
        	cleanClient(current_fd_);
        
        	return false; // Connection closed or error
    	}
		
		client->body_bytes_read += byte_received;
		std::cout << "client body bytes after reading: " << client->body_bytes_read << std::endl;
		
		if (max_body_size > 0 && client->body_bytes_read > max_body_size) {
			std::cerr << "[DEBUG] Body size exceeded during reading: " << client->body_bytes_read 
					  << " > " << max_body_size << std::endl;
			error_code_ = 413;
			return -1;
		}
		
		std::cout << "\nold client body buffer size: " << client->body_buffer.size() << std::endl;
		received_[byte_received] = '\0';
		client->body_buffer.append(received_, byte_received);
		std::cout << "new client body buffer size: " << client->body_buffer.size() << std::endl;
		std::cout << "bytes added this chunk: " << byte_received << std::endl;
	}
	
	if (client->body_bytes_read >= content_length) {
		std::cout << "[DEBUG] Body reading complete, parsing content" << std::endl;
		std::cout << "got all the body content, here is the boundary: ";
		std::cout << client->current_request->getBoundary() << std::endl;
		client->current_request->parseContent(client->body_buffer);
		std::string server_IP = getServerIP(current_fd_);
		default_server_ = servers_list_.front();
		Server *server_requested = getCurrentServer((*client->current_request), *this, server_IP);
		Location *location_requested = getCurrentLocation((*client->current_request), *server_requested);
		processAndSendResponse(server_requested, location_requested);
		return 1; // Complete
	}
	
	std::cout << "[DEBUG] Body reading incomplete, need more data" << std::endl;
	return 0; // Need more data
}

void ServerManager::processAndSendResponse(Server *server_requested, Location *location_requested) {
	std::cout << "[DEBUG] processAndSendResponse() called" << std::endl;
	
	Client* client = clients_list_[current_fd_];
	if (!client) {
		std::cout << "[DEBUG] No client found for fd: " << current_fd_ << std::endl;
		return;
	}
	
	std::cout << "[DEBUG] Creating HTTP response" << std::endl;
	HTTPResponse* http_response = new HTTPResponse((*client->current_request), default_server_, server_requested, location_requested, error_code_);
	client->setResponse(http_response);
	
	client->header_buffer = client->current_response->getResponse();
	std::cout << "[DEBUG] Response headers size: " << client->header_buffer.size() << std::endl;
	
	if (current_fd_ < 0) {
		std::cerr << "[DEBUG] Error, current_fd_ socket is already closed" << std::endl;
		return;
	}
	if (client->header_buffer.empty()) {
		std::cerr << "[DEBUG] Error, response is empty" << std::endl;
		return;
	}
	
	// CRITICAL: Check if socket is ready for writing before send()
	if (!(events_[client_id_].events & EPOLLOUT)) {
		std::cout << "[DEBUG] Socket not ready for writing, modifying epoll for EPOLLOUT" << std::endl;
		// Modify epoll to wait for EPOLLOUT
		struct epoll_event event;
		event.events = EPOLLOUT;
		event.data.fd = current_fd_;
		int epoll_result = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, current_fd_, &event);
		std::cout << "[DEBUG] epoll_ctl result: " << epoll_result << std::endl;
		return; // Will be called again when socket is ready
	}
	
	std::cout << "[DEBUG] Socket ready for writing, sending " << client->header_buffer.length() << " bytes" << std::endl;
	ssize_t bytes_sent = send(current_fd_, client->header_buffer.c_str(), client->header_buffer.length(), 0);
	std::cout << "\n$$$$$ SENDING HEADER RESPONSE $$$$$\n" << std::endl << client->header_buffer << std::endl;
	std::cout << "[DEBUG] send() returned: " << bytes_sent << " bytes" << std::endl;
	
	// DON'T CHECK ERRNO - per requirements
	if (bytes_sent <= 0) {
		std::cout << "[DEBUG] Error when sending response to client" << std::endl;
		return; // Error or would block
	}
	
	if (bytes_sent < static_cast<ssize_t>(client->header_buffer.length())) {
		// Partial send - store remaining data and wait for next EPOLLOUT
		std::cout << "[DEBUG] Partial send: " << bytes_sent << "/" << client->header_buffer.length() 
				  << " bytes sent" << std::endl;
		client->pending_response = client->header_buffer.substr(bytes_sent);
		std::cout << "[DEBUG] Stored " << client->pending_response.size() << " bytes for next send" << std::endl;
		return;
	}

	std::cout << "[DEBUG] All headers sent successfully" << std::endl;
	
	if (client->current_response->isReady()) {
		std::cout << "[DEBUG] Response has been sent, resetting client for next request" << std::endl;
		// Switch back to EPOLLIN for next request
		std::cout << "[DEBUG] Switching epoll back to EPOLLIN" << std::endl;
		struct epoll_event event;
		event.events = EPOLLIN;
		event.data.fd = current_fd_;
		int epoll_result = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, current_fd_, &event);
		std::cout << "[DEBUG] epoll_ctl result: " << epoll_result << std::endl;
		client->resetForNextRequest();
		return;
	}
	
	if (!client->current_response->getBodyFilename().empty()) {
		std::cout << "[DEBUG] Response has body file: " << client->current_response->getBodyFilename() << std::endl;
		client->file_sending_complete = false;
		sendResponseBodyFile();
	} else {
		std::cout << "[DEBUG] No body file, resetting client for next request" << std::endl;
		client->resetForNextRequest();
	}
}

bool ServerManager::sendResponseBodyFile() {
	std::cout << "[DEBUG] sendResponseBodyFile() called" << std::endl;
	
	Client* client = clients_list_[current_fd_];
	if (!client) {
		std::cout << "[DEBUG] No client found for fd: " << current_fd_ << std::endl;
		return false;
	}
	
	std::string body_file = client->current_response->getBodyFilename();
	std::cout << "[DEBUG] Body file: " << body_file << std::endl;
	
	// Initialize file reading state if first call
	if (!client->file_stream.is_open()) {
		std::cout << "[DEBUG] Opening file for reading" << std::endl;
		client->file_stream.open(body_file.c_str(), std::ios::binary);
		if (!client->file_stream.is_open()) {
			std::cerr << "[DEBUG] Failed to open body file: " << body_file << std::endl;
			error_code_ = 500;
			return false;
		}
		client->file_sending_complete = false;
		std::cout << "[DEBUG] File opened successfully" << std::endl;
	}
	
	// CRITICAL: Only send if epoll says socket is ready for writing
	if (!(events_[client_id_].events & EPOLLOUT)) {
		std::cout << "[DEBUG] Socket not ready for writing, requesting EPOLLOUT" << std::endl;
		// Request EPOLLOUT notification
		struct epoll_event event;
		event.events = EPOLLOUT;
		event.data.fd = current_fd_;
		int epoll_result = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, current_fd_, &event);
		std::cout << "[DEBUG] epoll_ctl result: " << epoll_result << std::endl;
		return false; // Will be called again when ready
	}
	
	// Read chunk from file
	if (!client->file_stream.eof()) {
		std::cout << "[DEBUG] Reading chunk from file" << std::endl;
		client->file_stream.read(buffer_, sizeof(buffer_));
		std::streamsize bytes_read = client->file_stream.gcount();
		std::cout << "[DEBUG] Read " << bytes_read << " bytes from file" << std::endl;
		
		if (bytes_read > 0) {
			std::cout << "[DEBUG] Sending " << bytes_read << " bytes to client" << std::endl;
			ssize_t bytes_sent = send(current_fd_, buffer_, bytes_read, 0);
			std::string debug_content(buffer_, bytes_read);
			std::cout << "\n$$$$$ SENDING FILE INFORMATION $$$$$\n" << std::endl << debug_content << std::endl;
			std::cout << "[DEBUG] send() returned: " << bytes_sent << " bytes" << std::endl;
			
			// DON'T CHECK ERRNO - per requirements
			if (bytes_sent <= 0) {
				std::cout << "[DEBUG] Send would block or error occurred" << std::endl;
				// Would block or error - seek back and try again later
				client->file_stream.seekg(-bytes_read, std::ios::cur);
				std::cout << "[DEBUG] Seeked back " << bytes_read << " bytes in file" << std::endl;
				return false;
			} else if (bytes_sent < bytes_read) {
				std::cout << "[DEBUG] Partial send: " << bytes_sent << "/" << bytes_read << " bytes" << std::endl;
				// Partial send - seek back to unsent position
				client->file_stream.seekg(bytes_sent - bytes_read, std::ios::cur);
				std::cout << "[DEBUG] Seeked back " << (bytes_read - bytes_sent) << " bytes in file" << std::endl;
				return false;
			}
			std::cout << "[DEBUG] All bytes sent successfully" << std::endl;
		}
		
		if (client->file_stream.bad()) {
			std::cerr << "[DEBUG] Error reading from file: " << body_file << std::endl;
			client->file_stream.close();
			return false;
		}
	}
	
	// Check if file sending is complete
	if (client->file_stream.eof()) {
		std::cout << "[DEBUG] File sending complete, closing file" << std::endl;
		client->file_stream.close();
		client->file_sending_complete = true;
		
		// Clean up auto-generated file if needed
		if (client->current_response->isAutoIndex()) {
			std::cout << "[DEBUG] Removing auto-generated file: " << body_file << std::endl;
			if (std::remove(body_file.c_str()) == 0) {
				client->current_response->setAutoIndex(false);
				std::cout << "[DEBUG] Auto-generated file removed successfully" << std::endl;
			} else {
				std::cout << "[DEBUG] Failed to remove auto-generated file" << std::endl;
			}
		}
		
		// Switch back to EPOLLIN for next request
		std::cout << "[DEBUG] Switching epoll back to EPOLLIN" << std::endl;
		struct epoll_event event;
		event.events = EPOLLIN;
		event.data.fd = current_fd_;
		int epoll_result = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, current_fd_, &event);
		std::cout << "[DEBUG] epoll_ctl result: " << epoll_result << std::endl;

		if (error_code_ > 0) {
			close(current_fd_);
			error_code_ = -1;
		}
		
		std::cout << BG_BLACK BOLD WHITE "[DEBUG] File transfer complete - resetting client for next request" RESET << std::endl;
        client->resetForNextRequest();
		std::cout << "[DEBUG] check header string cleanup: " << client->headers_string.size() << std::endl;
		return true;
	}
	
	std::cout << "[DEBUG] File sending not complete yet" << std::endl;
	return false;
}

bool ServerManager::cleanClient(int current_fd_) {
	std::cout << BG_CYAN BOLD BLACK "[DEBUG] cleanClient() called for fd: " << current_fd_ << RESET << std::endl;
	
	if (current_fd_ <= 0) {
		std::cout << "[DEBUG] Invalid fd: " << current_fd_ << std::endl;
		return false;
	}
	if (!clients_list_.empty()) {
		std::map<int,Client*>::iterator beg;
		beg = clients_list_.find(current_fd_);
		if (beg != clients_list_.end()) {
			std::cout << "[DEBUG] Found client for fd: " << current_fd_ << ", cleaning up" << std::endl;
			int epoll_result = epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, current_fd_, NULL);
			std::cout << "[DEBUG] epoll_ctl DEL result: " << epoll_result << std::endl;
			if (epoll_result == -1) {
				std::cerr << "[DEBUG] epoll_ctl DEL failed: " << strerror(errno) << std::endl;
			}
			close(current_fd_);

			delete beg->second;
			clients_list_.erase(beg);
			std::cout << "[DEBUG] Client cleanup complete, remaining clients: " << clients_list_.size() << std::endl;
			return true;
		} else {
			std::cout << "[DEBUG] No client found for fd: " << current_fd_ << "\n" << std::endl;
			return false;
		}
	}
	return true;
}

void ServerManager::shutdown() {
	cleanShutdown(*this);
};

/* BLOCKING PROBLEMATIC CLIENT ++ OPTION COUNTING STRIKE ATTEMPT*/
bool ServerManager::isBlocked(const void *IP) {
	return blocked_clients_list_.find(IP) != blocked_clients_list_.end();
}