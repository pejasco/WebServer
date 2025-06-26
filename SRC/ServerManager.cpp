#include "../INC/utils/ServerManager.hpp"

bool server_flag = false;
bool error_flag = false;

/************************/
/*CONSTRUCTOR/DESTRUCTOR*/
/************************/

ServerManager::ServerManager(std::string &input_config_file) : running_(true), default_server_(NULL), epoll_fd_(-1), num_events_(-1), current_fd_(-1), error_code_(-1) {
	std::cout << BOLD MAGENTA "Starting MasterServer\n" RESET;
	if (is_file_empty(input_config_file))
		input_config_file = "configuration/default.conf";
	std::fstream config_file(input_config_file.c_str());

	if (!readFile(config_file))
		throw std::runtime_error("Failed to read config file / Error found when reading config file");
	setHostPort();
	if (!startSockets())
		throw std::runtime_error("Failed to setting up socket(s) / Error found when setting up socket(s)");
	if (!startEpoll())
		throw std::runtime_error("Failed to setting up Epoll / Error found when setting up Epoll");
	
	//check on error of the config file (ex: incorrect format, missing essential elements) 
	// --> can be place before the call of the object in main ??
	// --> Write an information on how to write a config file for our server (README.md?)

	/********DEBUGGING*********/
	// Printing the server object to ensure they are well connected	
	// std::vector<Server*>::iterator beg = servers_list_.begin();
	// std::vector<Server*>::iterator end = servers_list_.end();
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


	std::cout << BOLD MAGENTA "Closing MasterServer\n" RESET;
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

bool ServerManager::checkDuplicatePort(std::fstream &config_file){
	std::string line;
	std::vector<std::string> ports;
	
	while (std::getline(config_file, line)){
		//std::multimap<std::string, std::string> ports_of_each_server;
		if (line.find("server") != std::string::npos)
			ports.clear();
		if (line.find("port") != std::string::npos)
		{
		
			size_t pos_beg = line.find(":") + 1;
			pos_beg = line.find_first_not_of(' ', pos_beg);
			std::string ports_str = line.substr(pos_beg);
			std::stringstream ss(ports_str);
			std::string port;
			//std::cout << "ports_str: " << ports_str << "\n";
			while (ss >> port){
				//std::cout << "ports: " << port << "\n";
				if (std::find(ports.begin(), ports.end(), port) != ports.end()){
					return true;
				}
				ports.push_back(port);
				//for (std::vector<std::string>::iterator it = ports.begin(); it != ports.end(); it++)
				//	std::cout << "ports_vector: " << *it << "\n";
			}
		}
	}
	return false;
}	

int	ServerManager::readFile(std::fstream &config_file) {
	Server *current_server =  NULL;
	std::string line;
	int server_ID = 1000; //Set an ID for new servers (MAY NOT BE USEFUL?)
	
	if (config_file.is_open()){
		if (checkDuplicatePort(config_file))
		{
			std::cerr << "Error: Duplicate port found\n";
			config_file.close();
			return (0);
		}
	}
	config_file.clear();
	config_file.seekg(0, std::ios::beg);
	if (config_file.is_open()) {
		while (std::getline(config_file, line)) {
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
		std::cerr << BOLD RED "Error: file incorrect\n" RESET;
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
	if (line.find("server_name") != std::string::npos) {
		if ((pos = line.rfind(":")) != std::string::npos)
			current_server->addServerName(line.substr(pos + 2));
	} else if (line.find("host") != std::string::npos) {
		if ((pos = line.find(":")) != std::string::npos && pos > line.find("host"))
			current_server->setIP(line.substr(pos + 2));
	} else if (line.find("port") != std::string::npos) {
		if ((pos = line.rfind(":")) != std::string::npos)
			current_server->setPort(line.substr(pos + 2));
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
	std::set<std::string> created_addresses;
	
	for (; beg != end; ++beg) {
		// Create IP:port string for duplicate checking (display format)
		std::string address = beg->second + ":" + beg->first;
		
		// Skip if this IP:port combination already exists
		if (created_addresses.find(address) != created_addresses.end()) {
			DEBUG_PRINT("startSockets() --> address does exist already");
			continue;
		}
		
		// Use your custom order: second element (IP) first, then first element (port)
		sockets_list_.push_back(new Socket(beg->second, beg->first));
		
		if (!sockets_list_.back()->getSocketError()) {
			delete sockets_list_.back();
			sockets_list_.pop_back();
			return 0;
		} else {
			sockets_fd_list_.push_back(sockets_list_.back()->getSocketFd());
			created_addresses.insert(address);
		}
	}
	return 1;
}

/****************/
/*startEpoll --> setup and start an epoll instant that will monitore the socketFD from the servers*/
/****************/
int ServerManager::startEpoll() {
	/*STEP1 -- creation of the epoll instance*/
	epoll_fd_ = epoll_create(4096); // nb of socket_fd to monitor (BUT NOT IN USE NOWADAYS -- epoll_create1())
	if (epoll_fd_ == -1) {
		std::cerr << BOLD RED "Error on epoll creation\n" RESET;
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
			std::cerr << BOLD RED "Error adding socket " << *begFd << " to epoll instance\n" RESET;
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

	#ifdef DEBUG
		int loop = 5000;
		int nb_loop = 0;
		while (running_ && nb_loop < loop) {
			nb_loop++;
			DEBUG_PRINT("Loop " << nb_loop << " - waiting for events...");
	#else
		while (running_) {
	#endif
		num_events_ = epoll_wait(epoll_fd_, events_, MAX_EVENTS, EPOLL_TIMEOUT);
		DEBUG_PRINT("epoll_wait returned " << num_events_ << " events");
		if (num_events_ == 0) {
			DEBUG_PRINT("Timeout detected");
			checkClientTimeouts();
		} else if (num_events_ == -1) {
			#ifdef DEBUG
			//FOR DEBBUGING ONLY
				if (errno != EINTR)
					std::cerr << BOLD RED "Error epoll_wait: " << strerror(errno) << RESET << std::endl;
			// FOR DEBBUGING ONLY
			#else
				;
			#endif
			running_ = false;
			break;
		} else {
			for (client_id_ = 0; client_id_ < num_events_; client_id_++) {
				current_fd_ = events_[client_id_].data.fd;
				DEBUG_PRINT("Processing fd: " << current_fd_ << ", events: " << events_[client_id_].events);
				
				// Check if this is a server socket
				bool is_server_socket = (std::find(sockets_fd_list_.begin(), sockets_fd_list_.end(), current_fd_) != sockets_fd_list_.end());
				DEBUG_PRINT("Is server socket: " << (is_server_socket ? "YES" : "NO"));
				if (is_server_socket) {
					createNewClientConnection();
				} else {
					DEBUG_PRINT("Looking for existing client for fd: " << current_fd_);
					Client *current_client = clients_list_[current_fd_];
					if (current_client == NULL) {
						DEBUG_PRINT("No client found for fd: " << current_fd_ << ", cleaning up");
						close(current_fd_);
						epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, current_fd_, NULL);
					} else {
						current_client->last_activity = time(NULL);
						DEBUG_PRINT("Found client, calling existingClientConnection()");
						existingClientConnection();
					}
				}
			}
		}
	#ifdef DEBUG
		}
		DEBUG_PRINT("server monitor is over. " << (nb_loop > loop ? BOLD RED "Reaching the loop limits" RESET : ""));
	#else
	}
		DEBUG_PRINT("Server monitoring ended, cleaning up...");
	#endif
	this->cleanClient(current_fd_);
	this->shutdown();
}

void ServerManager::createNewClientConnection() {
	DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "createNewClientConnection() called for server fd: " << current_fd_ << RESET);
	
	temp_client_addr_len_ = sizeof(temp_client_addr_);
	DEBUG_PRINT("Calling accept() on fd: " << current_fd_);
	
	int temp_fd = accept(current_fd_, (struct sockaddr*)&temp_client_addr_, &temp_client_addr_len_);
	DEBUG_PRINT("accept() returned fd: " << temp_fd);
	
	if (temp_fd == -1) {
		std::cerr << BOLD RED "Error accept() failed: " << strerror(errno) << RESET << std::endl;
		DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "createNewClientConnection() existed" RESET);
		return;
	}
	
	DEBUG_PRINT("Creating new Client object for fd: " << temp_fd);
	Client *new_client = new Client(temp_fd, temp_client_addr_, temp_client_addr_len_);
	
	if (new_client->getError()) {
		DEBUG_PRINT("Client has error, adding to blocked list");
		blocked_clients_list_.insert(new_client->getClientIP());
	} else if (isBlocked(new_client->getClientIP())) {
		DEBUG_PRINT("Client IP is blocked, closing connection");
		close(new_client->getClientFd());
		DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "createNewClientConnection() existed" RESET);
		delete new_client;
		return;
	}

	new_client->last_activity = time(0);
	
	int new_client_fd = new_client->getClientFd();
	DEBUG_PRINT("Client fd: " << new_client_fd << ", IP: " << new_client->getClientIP());
	
	clients_list_.insert(std::pair<int,Client*>(new_client_fd, new_client));
	DEBUG_PRINT("Added client to clients_list_, total clients: " << clients_list_.size());

	// Add the client fd to epoll
	DEBUG_PRINT("Adding client fd " << new_client_fd << " to epoll");
	struct epoll_event client_event;
	client_event.events = EPOLLIN;
	client_event.data.fd = new_client_fd;
	
	if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, new_client_fd, &client_event) == -1) {
		std::cerr << BOLD RED "Error epoll_ctl ADD failed: " << strerror(errno) << RESET << std::endl;
		close(new_client_fd);
		clients_list_.erase(new_client_fd);
		delete new_client;
		DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "createNewClientConnection() existed" RESET);
		return;
	}
	DEBUG_PRINT("Successfully added client fd " << new_client_fd << " to epoll");
	DEBUG_PRINT(BOLD UNDERLINE BG_GREEN BLACK "createNewClientConnection() existed" RESET);
}

/****************/
/* exisingClientConnection --> as the server-client connection is already in the epoll instance, */
/* the function can receive the message request and start analysing it */
/****************/

void ServerManager::existingClientConnection() {
	DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "existingClientConnection() called for server fd: " << current_fd_ << RESET);
	
	Client* client = clients_list_[current_fd_];
	if (!client) {
		DEBUG_PRINT("No client found for fd: " << current_fd_);
		DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "existingClientConnection() existed" RESET);
		return;
	}
	// if (reinterpret_cast<uintptr_t>(client->current_request) < 0x1000) {
	// 	DEBUG_PRINT("ERROR: Request pointer looks invalid: " << client->current_request);
	// }	
	uint32_t current_events = events_[client_id_].events;
	DEBUG_PRINT("Client [" << client_id_ << "]: " << client->state << ", file_sending_complete: " << client->file_sending_complete);
	DEBUG_PRINT("=== EVENT ANALYSIS ===");
	DEBUG_PRINT("EPOLLIN (1): " << (current_events & EPOLLIN ? "YES" : "NO"));
	DEBUG_PRINT("EPOLLOUT (4): " << (current_events & EPOLLOUT ? "YES" : "NO"));
	DEBUG_PRINT("EPOLLHUP (16): " << (current_events & EPOLLHUP ? "YES" : "NO"));
	DEBUG_PRINT("EPOLLERR (8): " << (current_events & EPOLLERR ? "YES" : "NO"));
	DEBUG_PRINT("EPOLLRDHUP (8192): " << (current_events & EPOLLRDHUP ? "YES" : "NO"));
	DEBUG_PRINT("======================");
	if (current_events & (EPOLLHUP | EPOLLERR)) {
		if (current_events & EPOLLHUP)
			DEBUG_PRINT("Client disconnected (EPOLLHUP) - fd: " << current_fd_);
		else if (current_events & EPOLLERR)
			DEBUG_PRINT("Error occurred (EPOLLERR) - fd: " << current_fd_);
		cleanClient(current_fd_);
		DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "existingClientConnection() existed" RESET);
		return;
	}
	
	// Handle file sending if in progress
	if (!client->file_sending_complete) {
		DEBUG_PRINT("File sending in progress");
		if (events_[client_id_].events & EPOLLOUT) {
			DEBUG_PRINT( BOLD YELLOW "EPOLLOUT ready" RESET);
			if (sendResponseBodyFile()) {
				DEBUG_PRINT("File sending completed, resetting client");
				cleanHTTPElement(current_fd_);
				client->resetForNextRequest();
			}
			DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "existingClientConnection() existed" RESET);
			return;
		} else if (events_[client_id_].events & EPOLLIN) {
			DEBUG_PRINT(BOLD MAGENTA "EPOLLIN during file transfer - resetting client state" RESET);
			// Client sending new data while file transfer in progress - reset state
			client->resetForNextRequest();
		} else {
			DEBUG_PRINT(BOLD MAGENTA "Waiting for EPOLLOUT during file transfer" RESET);
			DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "existingClientConnection() existed" RESET);
			return; // Wait for EPOLLOUT
		}
	}
	
	 if (client->state == CLIENT_READY_TO_RESPOND && (events_[client_id_].events & EPOLLOUT)) {
		DEBUG_PRINT(BOLD YELLOW "Client ready to respond and EPOLLOUT available - sending response" RESET);
		
		// Handle pending response data first
		if (!client->header_buffer.empty()) {
			DEBUG_PRINT("Header bytes to send: " BOLD << client->header_buffer.size() << RESET " bytes");
			ssize_t bytes_sent = send(current_fd_, client->header_buffer.c_str(), client->header_buffer.length(), 0);
			//DEBUG_PRINT("Header sent to client: " << client->header_buffer);
			DEBUG_PRINT("send() returned: " BOLD << bytes_sent << RESET" bytes");
			
			if (bytes_sent <= 0) {
				DEBUG_PRINT("Error sending pending response");
				return; // Error or would block
			}
			
			if (bytes_sent < static_cast<ssize_t>(client->header_buffer.length())) {
				// Still partial - update pending data
				DEBUG_PRINT("Still partial send: " BOLD << bytes_sent << RESET "/" BOLD << client->header_buffer.length() << RESET " bytes sent");
				client->header_buffer = client->header_buffer.substr(bytes_sent);
				DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "existingClientConnection() existed" RESET);
				return;
			}
			
			// All pending data sent
			DEBUG_PRINT(BOLD YELLOW "All pending response data sent, cleaning header_buffer from client" RESET);
			client->header_buffer.clear();
		}
		
		// Check if response is complete
		if (client->current_response && client->current_response->isReady()) {
			DEBUG_PRINT("Response has been sent, resetting client for next request");
			// Switch back to EPOLLIN for next request
			DEBUG_PRINT(BOLD MAGENTA "Switching epoll back to EPOLLIN" RESET);
			struct epoll_event event;
			event.events = EPOLLIN;
			event.data.fd = current_fd_;
			int epoll_result = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, current_fd_, &event);
			DEBUG_PRINT("epoll_ctl result: " << epoll_result);
			(void)epoll_result;
			cleanHTTPElement(current_fd_);
			client->resetForNextRequest();
			DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "existingClientConnection() existed" RESET);
			return;
		}
		
		// Check if there's a body file to send
		if (client->current_response && !client->current_response->getBodyFilename().empty()) {
			DEBUG_PRINT("Response has body file: " << client->current_response->getBodyFilename());
			client->file_sending_complete = false;
			sendResponseBodyFile();
			DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "existingClientConnection() existed" RESET);
		} else {
			DEBUG_PRINT("No body file, resetting client for next request");
			DEBUG_PRINT(BOLD MAGENTA "Switching epoll back to EPOLLIN" RESET);
			struct epoll_event event;
			event.events = EPOLLIN;
			event.data.fd = current_fd_;
			int epoll_result = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, current_fd_, &event);
			DEBUG_PRINT("epoll_ctl result: " << epoll_result);
			(void)epoll_result;
			DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "existingClientConnection() existed" RESET);
			cleanHTTPElement(current_fd_);
			client->resetForNextRequest();
		}
		return;
	}

	// Handle new request data - ONLY if EPOLLIN is set
	if (events_[client_id_].events & EPOLLIN) {
		DEBUG_PRINT(BOLD MAGENTA "EPOLLIN ready, processing request data" RESET);
		
		// State machine approach
		switch (client->state) {
			case CLIENT_READING_HEADERS:
				DEBUG_PRINT(BOLD CYAN "State: CLIENT_READING_HEADERS" RESET);
				if (readClientHeaders()) {
					DEBUG_PRINT("Headers read successfully, transitioning to CLIENT_PROCESSING_HEADERS");
					client->state = CLIENT_PROCESSING_HEADERS;
					// Fall through to process headers
				} else {
					DEBUG_PRINT("Need more header data");
					DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "existingClientConnection() existed" RESET);
					return; // Need more data
				}
				
			case CLIENT_PROCESSING_HEADERS: {
				DEBUG_PRINT(BOLD CYAN "State: CLIENT_PROCESSING_HEADERS" RESET);
				if (!parseHeadersAndCheckBodySize()) {
					DEBUG_PRINT("Header parsing failed, closing connection");
					if (current_fd_ > 0) 
						close(current_fd_);
					cleanHTTPElement(current_fd_);
					client->resetForNextRequest();
					DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "existingClientConnection() existed" RESET);
					return;
				}
				
				int content_length = client->current_request->getContentLength();
				DEBUG_PRINT("Content-Length: " << content_length);
				DEBUG_PRINT("Client error code: " << client->getLastStatusCode() << ", error_flag: " << error_flag);
				if (reinterpret_cast<uintptr_t>(client->current_request) < 0x1000) {
					DEBUG_PRINT("ERROR: Request pointer looks invalid: " << client->current_request);
				}
				if (client->getLastStatusCode() == 431 || client->getLastStatusCode() == 413) {
					client->state = CLIENT_READY_TO_RESPOND;
					DEBUG_PRINT("body or header too large, sending error to client, transitioning to CLIENT_READY_TO_RESPOND");
					// Server/location resolution happens in parseHeadersAndCheckBodySize
					// and response is prepared there for requests without body
					DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "existingClientConnection() existed" RESET);
					return;
				} else if (content_length > 0 && !error_flag) {
					client->expected_content_length = content_length;
					client->state = CLIENT_READING_BODY;
					DEBUG_PRINT("Transitioning to CLIENT_READING_BODY");
					// Fall through to read body
				} else {
					client->state = CLIENT_READY_TO_RESPOND;
					DEBUG_PRINT("No body to send, transitioning to CLIENT_READY_TO_RESPOND");
					// Server/location resolution happens in parseHeadersAndCheckBodySize
					// and response is prepared there for requests without body
					DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "existingClientConnection() existed" RESET);
					return;
				}
			}
				
			case CLIENT_READING_BODY: {
				DEBUG_PRINT(BOLD CYAN "State: CLIENT_READING_BODY" RESET);
				if (reinterpret_cast<uintptr_t>(client->current_request) < 0x1000) {
					DEBUG_PRINT("ERROR: Request pointer looks invalid: " << client->current_request);
				}	
				int result = readRequestBody(client->expected_content_length, client->max_body_size);
				DEBUG_PRINT("readRequestBody() returned: " << result);
				if (result == 1) {
					// Body complete - response preparation happens in readRequestBody
					client->state = CLIENT_READY_TO_RESPOND;
					DEBUG_PRINT("Body reading complete, transitioning to CLIENT_READY_TO_RESPOND");
				} else if (result == -1) {
					// Error
					DEBUG_PRINT("Error reading body, closing connection");
					if (current_fd_ > 0) 
						close(current_fd_);
					cleanHTTPElement(current_fd_);
					client->resetForNextRequest();
					DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "existingClientConnection() existed" RESET);
					return;
				} else
					DEBUG_PRINT("Need more body data");
				break;
			}
				
			case CLIENT_READY_TO_RESPOND:
				DEBUG_PRINT(BOLD CYAN "State: CLIENT_READY_TO_RESPOND - should not reach here with EPOLLIN" RESET);
				// Should not reach here with EPOLLIN
				break;
		}
	} else {
		DEBUG_PRINT(BOLD "No EPOLLIN event, nothing to process" RESET);
	}
	DEBUG_PRINT(BOLD UNDERLINE BG_BLUE BLACK "existingClientConnection() existed" RESET);
}

bool ServerManager::readClientHeaders() {
	DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "readClientHeaders() called for fd: " << current_fd_ << RESET);
	
	Client* client = clients_list_[current_fd_];
	if (!client) {
		DEBUG_PRINT("No client found for fd: " << current_fd_);
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "readClientHeaders() exited" RESET);
		return false;
	}
	
	// CRITICAL: Only read if epoll says data is available
	if (!(events_[client_id_].events & EPOLLIN)) {
		DEBUG_PRINT("No EPOLLIN event, cannot read");
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "readClientHeaders() exited" RESET);
		return false;
	}

	DEBUG_PRINT("Attempting to read headers from socket");
	ssize_t byte_received = recv(current_fd_, received_, sizeof(received_) - 1, 0);    
	DEBUG_PRINT("recv() returned: " BOLD << byte_received << RESET " bytes");
	
	if (byte_received > 0 && client && client->getLastStatusCode() == 413) {
		DEBUG_PRINT("Client sending data after 413 error - closing connection");
		cleanClient(current_fd_);
		return false;
	}

	if (byte_received <= 0) {
		if (byte_received == 0) {
			DEBUG_PRINT("Client disconnected gracefully - cleaning up connection");
		} else {
			DEBUG_PRINT("Error during header reading - cleaning up connection");
		}
		cleanClient(current_fd_);
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "readClientHeaders() exited" RESET);
		return false; // Connection closed or error
	}
	
	received_[byte_received] = '\0';
	DEBUG_PRINT("Header buffer size before append: " BOLD << client->header_buffer.size() << RESET);
	client->header_buffer.append(received_, byte_received);
	DEBUG_PRINT("Header buffer size after append: " BOLD << client->header_buffer.size() << RESET);
	DEBUG_PRINT("Header received: " << client->header_buffer);
	
	// Check for end of headers
	size_t header_end = client->header_buffer.find("\r\n\r\n");
	if (header_end != std::string::npos) {
		DEBUG_PRINT("Found end of headers at position: " BOLD << header_end << RESET);
		client->headers_string = client->header_buffer.substr(0, header_end + 4);
		size_t body_start = header_end + 4;
		if (body_start < client->header_buffer.length()) {
			client->body_buffer = client->header_buffer.substr(body_start);
			client->body_bytes_read = client->body_buffer.length();
			DEBUG_PRINT("Found " BOLD << client->body_bytes_read << RESET " body bytes in header buffer");
		}
		client->header_completed = true;
		DEBUG_PRINT("Headers completed successfully");
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "readClientHeaders() exited" RESET);
		return true;
	}
	
	// Header size check 
	if (client->header_buffer.size() > MAX_HEADER_SIZE) {
		DEBUG_PRINT("Header size exceeded: " BOLD << client->header_buffer.size() << RESET " > " << MAX_HEADER_SIZE);
		client->setLastStatusCode(431);
		error_flag = true;
		client->header_completed = true;
		DEBUG_PRINT("Headers too large -- sending HTTP error");
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "readClientHeaders() exited" RESET);
		return true;
	}
	
	DEBUG_PRINT("Headers not complete yet, need more data");
	DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "readClientHeaders() exited" RESET);
	return false; // Need more header data
}

bool ServerManager::parseHeadersAndCheckBodySize() {
	DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "parseHeadersAndCheckBodySize() called for fd: " << current_fd_ << RESET);
	
	Client* client = clients_list_[current_fd_];
	if (!client) {
		DEBUG_PRINT("No client found for fd: " << current_fd_);
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "parseHeadersAndCheckBodySize() exited" RESET);
		return false;
	}
	HTTPRequest* http_request = new HTTPRequest();
	client->setRequest(http_request);
	client->current_request->parseRequest(client->headers_string);
	
	
	// Server/location resolution - done once here
	DEBUG_PRINT(BOLD GREEN "URL MATCHING INFORMATION FOR HEADER SENDING" RESET);
	std::string server_IP = getServerIPPort(current_fd_);
	DEBUG_PRINT("Server IP: " << server_IP);
	
	default_server_ = servers_list_.front();
	Server *server_requested = getCurrentServer((*client->current_request), *this, server_IP);
	Location *location_requested = getCurrentLocation((*client->current_request), *server_requested);
	
	DEBUG_PRINT("Server requested: " << (server_requested ? "found" : "null"));
	DEBUG_PRINT("Location requested: " << (location_requested ? "found" : "null"));
	
	if (location_requested == NULL && (client->current_request->getPath() == "/")) {
		location_requested = servers_list_.front()->getLocationsList().front();
		DEBUG_PRINT("Using default location for root path");
	}
	
	int max_body_size = maxBodySizeLocation(servers_list_.front(), server_requested, location_requested);
	int content_length = client->current_request->getContentLength();
	
	DEBUG_PRINT("Max body size: " << max_body_size);
	DEBUG_PRINT("Content length: " << content_length);
	DEBUG_PRINT(BOLD GREEN "URL MATCHING INFORMATION HEADER SENDING" RESET);
	
	if (content_length > 0) {
		if (max_body_size > 0 && content_length > max_body_size) {
			DEBUG_PRINT("Request body size " BOLD << content_length << RESET " exceeds limit " BOLD << max_body_size << RESET);
			client->setLastStatusCode(413);
			error_flag = true;
			DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "parseHeadersAndCheckBodySize() exited" RESET);
			processAndSendResponse(server_requested, location_requested);
			return true;
		}
		// Store max_body_size for later use in body reading
		client->max_body_size = max_body_size;
		DEBUG_PRINT("Stored max_body_size: " << max_body_size << " for body reading");
	} else {
		// No body - process response immediately
		DEBUG_PRINT("No body to read, processing response immediately");
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "parseHeadersAndCheckBodySize() exited" RESET);
		processAndSendResponse(server_requested, location_requested);
	}
	
	DEBUG_PRINT("parseHeadersAndCheckBodySize() completed successfully");
	DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "parseHeadersAndCheckBodySize() exited" RESET);
	if (reinterpret_cast<uintptr_t>(client->current_request) < 0x1000) {
		DEBUG_PRINT("ERROR: Request pointer looks invalid: " << client->current_request);
	}	
	return true;
}

int ServerManager::readRequestBody(size_t content_length, size_t max_body_size) {
	DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "readRequestBody(() called" RESET);
	
	Client* client = clients_list_[current_fd_];
	if (!client) {
		DEBUG_PRINT("No client found for fd: " << current_fd_);
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "readRequestBody(() exited" RESET);
		return -1;
	}
	
	// Use stored max_body_size from parseHeadersAndCheckBodySize
	max_body_size = client->max_body_size;
	DEBUG_PRINT("Max_body_size: " BOLD << max_body_size << RESET);
	DEBUG_PRINT("Content_length of file: " BOLD << content_length << RESET);
	DEBUG_PRINT("client body byte before reading: " BOLD << client->body_bytes_read << RESET);
	
	if (client->body_bytes_read < content_length) {
		// CRITICAL: Only read if epoll says data is available
		if (!(events_[client_id_].events & EPOLLIN)) {
			DEBUG_PRINT("No EPOLLIN event, cannot read body data");
			DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "readRequestBody(() exited" RESET);
			return 0; // No data available, try again later
		}
		
		size_t remaining = content_length - client->body_bytes_read;
		size_t to_read = std::min(sizeof(received_) - 1, remaining);
		DEBUG_PRINT("Attempting to read " BOLD << to_read << RESET " bytes from socket");
		ssize_t byte_received = recv(current_fd_, received_, to_read, 0);
		DEBUG_PRINT("remaining: " BOLD << remaining << RESET ", to_read: " BOLD << to_read << RESET ", byte_received: " BOLD << byte_received << RESET);
		
		// DON'T CHECK ERRNO - per requirements
		if (byte_received <= 0) {
			if (byte_received == 0) {
				DEBUG_PRINT("Client disconnected gracefully - cleaning up connection");
			} else {
				DEBUG_PRINT("Error during header reading - cleaning up connection");
			}
			cleanClient(current_fd_);
			DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "readRequestBody(() exited" RESET);
			return false; // Connection closed or error
		}
		
		client->body_bytes_read += byte_received;
		DEBUG_PRINT("client body bytes after reading: " BOLD << client->body_bytes_read << RESET);
		
		if (max_body_size > 0 && client->body_bytes_read > max_body_size) {
			DEBUG_PRINT("Body size exceeded during reading: " BOLD << client->body_bytes_read << RESET " > " BOLD << max_body_size << RESET);
			error_code_ = 413;
			DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "readRequestBody(() exited" RESET);
			return -1;
		}
		
		DEBUG_PRINT("old client body buffer size: " BOLD << client->body_buffer.size() << RESET);
		received_[byte_received] = '\0';
		client->body_buffer.append(received_, byte_received);
		DEBUG_PRINT("new client body buffer size: " BOLD << client->body_buffer.size() << RESET);
		DEBUG_PRINT("bytes added this chunk: " BOLD << byte_received << RESET);
	}
	
	if (client->body_bytes_read >= content_length) {
		DEBUG_PRINT("Body reading complete");
		if (reinterpret_cast<uintptr_t>(client->current_request) < 0x1000) {
			DEBUG_PRINT("ERROR: Request pointer looks invalid: " << client->current_request);
		}
		DEBUG_PRINT("body_buffer size to confirm: " BOLD << client->body_buffer.size() << RESET);
		client->current_request->parseContent(client->body_buffer);
		DEBUG_PRINT(BOLD GREEN"URL MATCHING INFORMATION FOR BODY SENDING" RESET);
		std::string server_IP = getServerIPPort(current_fd_);
		default_server_ = servers_list_.front();
		Server *server_requested = getCurrentServer((*client->current_request), *this, server_IP);
		Location *location_requested = getCurrentLocation((*client->current_request), *server_requested);
		DEBUG_PRINT(BOLD GREEN"URL MATCHING INFORMATION FOR DOBY SENDING" RESET);
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "readRequestBody(() exited" RESET);
		processAndSendResponse(server_requested, location_requested);
		return 1; // Complete
	}
	
	DEBUG_PRINT("Body reading incomplete, need more data");
	DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "readRequestBody(() exited" RESET);
	return 0; // Need more data
}

void ServerManager::processAndSendResponse(Server *server_requested, Location *location_requested) {
	DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "processAndSendResponse() called" RESET);
	
	Client* client = clients_list_[current_fd_];
	if (!client) {
		DEBUG_PRINT("No client found for fd: " << current_fd_);
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "processAndSendResponse() exited" RESET);
		return;
	}
	
	DEBUG_PRINT("Creating HTTP response");
	HTTPResponse* http_response = new HTTPResponse((*client->current_request), default_server_, server_requested, location_requested, error_flag, client->getLastStatusCode());
	client->setResponse(http_response);
	DEBUG_PRINT("status code of response: " << http_response->getStatusCode());
	client->setLastStatusCode(client->current_response->getStatusCode());
	error_flag = false;
	DEBUG_PRINT("Client status code saved before sending: " << client->getLastStatusCode());
	
	client->header_buffer = client->current_response->getResponse();
	DEBUG_PRINT("Response headers size: " << client->header_buffer.size());
	
	if (current_fd_ < 0) {
		// std::cerr << "Error, current_fd_ socket is already closed");
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "processAndSendResponse() exited" RESET);
		return;
	}
	if (client->header_buffer.empty()) {
		// std::cerr << "Error, response is empty");
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "processAndSendResponse() exited" RESET);
		return;
	}
	
	// CRITICAL: Check if socket is ready for writing before send()
	if (!(events_[client_id_].events & EPOLLOUT)) {
		DEBUG_PRINT(BOLD MAGENTA "Socket not ready for writing, modifying epoll for EPOLLOUT" RESET);
		// Modify epoll to wait for EPOLLOUT
		struct epoll_event event;
		event.events = EPOLLOUT;
		event.data.fd = current_fd_;
		int epoll_result = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, current_fd_, &event);
		DEBUG_PRINT("epoll_ctl result: " << epoll_result);
		(void)epoll_result;
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "processAndSendResponse() exited" RESET);
		return; // Will be called again when socket is ready
	}
	
	DEBUG_PRINT("Socket ready for writing, sending " BOLD << client->header_buffer.length() << RESET " bytes");
	ssize_t bytes_sent = send(current_fd_, client->header_buffer.c_str(), client->header_buffer.length(), 0);
	//DEBUG_PRINT("Header sent to client: " << client->header_buffer);
	DEBUG_PRINT("send() returned: " BOLD << bytes_sent << RESET " bytes");
	
	// DON'T CHECK ERRNO - per requirements
	if (bytes_sent <= 0) {
		DEBUG_PRINT("Error when sending response to client");
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "processAndSendResponse() exited" RESET);
		return; // Error or would block
	}
	
	if (bytes_sent < static_cast<ssize_t>(client->header_buffer.length())) {
		// Partial send - store remaining data and wait for next EPOLLOUT
		DEBUG_PRINT("Partial send: " BOLD << bytes_sent << RESET "/" BOLD << client->header_buffer.length() << RESET " bytes sent");
		client->pending_response = client->header_buffer.substr(bytes_sent);
		DEBUG_PRINT("Stored " BOLD << client->pending_response.size() << RESET " bytes for next send");
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "processAndSendResponse() exited" RESET);
		return;
	}
	
	if (client->current_response->isReady()) {
		DEBUG_PRINT("Response has been sent, resetting client for next request");
		// Switch back to EPOLLIN for next request
		DEBUG_PRINT(BOLD YELLOW "Switching epoll back to EPOLLIN" RESET);
		struct epoll_event event;
		event.events = EPOLLIN;
		event.data.fd = current_fd_;
		int epoll_result = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, current_fd_, &event);
		DEBUG_PRINT("epoll_ctl result: " << epoll_result);
		(void)epoll_result;
		client->resetForNextRequest();
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "processAndSendResponse() exited" RESET);
		return;
	}
	
	if (!client->current_response->getBodyFilename().empty()) {
		DEBUG_PRINT("Response has body file: " << client->current_response->getBodyFilename());
		client->file_sending_complete = false;
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "processAndSendResponse() exited" RESET);
		sendResponseBodyFile();
	} else {
		DEBUG_PRINT("No body file, resetting client for next request");
		client->resetForNextRequest();
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "processAndSendResponse() exited" RESET);
	}
}

bool ServerManager::sendResponseBodyFile() {
	DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "sendResponseBodyFile() called" RESET);
	
	Client* client = clients_list_[current_fd_];
	if (!client) {
		DEBUG_PRINT("No client found for fd: " << current_fd_);
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "sendResponseBodyFile() exited" RESET);
		return false;
	}
	
	std::string body_file = client->current_response->getBodyFilename();
	DEBUG_PRINT("Body file: " << body_file);
	
	// Initialize file reading state if first call
	if (!client->file_stream.is_open()) {
		DEBUG_PRINT("Opening file for reading");
		client->file_stream.open(body_file.c_str(), std::ios::binary);
		if (!client->file_stream.is_open()) {
			std::cerr << "Failed to open body file: " << body_file;
			error_code_ = 500;
			DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "sendResponseBodyFile() exited" RESET);
			return false;
		}
		client->file_sending_complete = false;
		DEBUG_PRINT("File opened successfully");
	}
	
	// CRITICAL: Only send if epoll says socket is ready for writing
	if (!(events_[client_id_].events & EPOLLOUT)) {
		DEBUG_PRINT(BOLD MAGENTA "Socket not ready for writing, requesting EPOLLOUT" RESET);
		// Request EPOLLOUT notification
		struct epoll_event event;
		event.events = EPOLLOUT;
		event.data.fd = current_fd_;
		int epoll_result = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, current_fd_, &event);
		DEBUG_PRINT("epoll_ctl result: " << epoll_result);
		(void)epoll_result;
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "sendResponseBodyFile() exited" RESET);
		return false; // Will be called again when ready
	}
	
	// Read chunk from file
	if (!client->file_stream.eof()) {
		DEBUG_PRINT("Reading chunk from file");
		client->file_stream.read(buffer_, sizeof(buffer_));
		std::streamsize bytes_read = client->file_stream.gcount();
		DEBUG_PRINT("Read " BOLD << bytes_read << " bytes from file" RESET);
		
		if (bytes_read > 0) {
			DEBUG_PRINT("Sending " BOLD << bytes_read << RESET " bytes to client");
			ssize_t bytes_sent = send(current_fd_, buffer_, bytes_read, 0);
			std::string debug_content(buffer_, bytes_read);
			//DEBUG_PRINT("Body sent to client: " << debug_content);
			DEBUG_PRINT("send() returned: " BOLD << bytes_sent << RESET " bytes");
			
			if (bytes_sent <= 0) {
				DEBUG_PRINT("Send would block or error occurred");
				// Would block or error - seek back and try again later
				client->file_stream.seekg(-bytes_read, std::ios::cur);
				DEBUG_PRINT("Seeked back " BOLD << bytes_read << RESET " bytes in file");
				DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "sendResponseBodyFile() exited" RESET);
				return false;
			} else if (bytes_sent < bytes_read) {
				DEBUG_PRINT("Partial send: " BOLD << bytes_sent << RESET "/" BOLD << bytes_read << RESET " bytes");
				// Partial send - seek back to unsent position
				client->file_stream.seekg(bytes_sent - bytes_read, std::ios::cur);
				DEBUG_PRINT("Seeked back " BOLD << (bytes_read - bytes_sent) << RESET " bytes in file");
				DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "sendResponseBodyFile() exited" RESET);
				return false;
			}
			DEBUG_PRINT("All bytes sent successfully");
		}
		
		if (client->file_stream.bad()) {
			// std::cerr << "Error reading from file: " << body_file);
			client->file_stream.close();
			DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "sendResponseBodyFile() exited" RESET);
			return false;
		}
	}
	
	// Check if file sending is complete
	if (client->file_stream.eof()) {
		DEBUG_PRINT("File sending complete, closing file");
		client->file_stream.close();
		client->file_sending_complete = true;
		
		// Clean up auto-generated file if needed
		if (client->current_response->isAutoIndex()) {
			DEBUG_PRINT("Removing auto-generated file: " << body_file);
			if (std::remove(body_file.c_str()) == 0) {
				client->current_response->setAutoIndex(false);
				DEBUG_PRINT("Auto-generated file removed successfully");
			} else {
				DEBUG_PRINT("Failed to remove auto-generated file");
			}
		}
		// Switch back to EPOLLIN for next request
		DEBUG_PRINT(BOLD YELLOW "Switching epoll back to EPOLLIN" RESET);
		struct epoll_event event;
		event.events = EPOLLIN;
		event.data.fd = current_fd_;
		int epoll_result = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, current_fd_, &event);
		(void)epoll_result;
		DEBUG_PRINT("epoll_ctl result: " << epoll_result);
		
		if (error_code_ > 0) {
			error_code_ = -1;
		}
		if (client->getLastStatusCode() != 200)
			cleanClient(current_fd_);
		else
			client->resetForNextRequest();
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "sendResponseBodyFile() exited" RESET);
		return true;
	}
	
	DEBUG_PRINT("File sending not complete yet");
	DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "sendResponseBodyFile() exited" RESET);
	return false;
}

bool ServerManager::cleanClient(int current_fd_) {
	DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "cleanClient() called for fd: " << current_fd_ << RESET);
	
	if (current_fd_ <= 0) {
		DEBUG_PRINT("Invalid fd: " << current_fd_);
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "cleanClient() exited" RESET);
		return false;
	}
	Client* delete_client = clients_list_[current_fd_];
	if (delete_client) {
		DEBUG_PRINT("Found client for fd: " << current_fd_ << ", cleaning up");
		int epoll_result = epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, current_fd_, NULL);
		DEBUG_PRINT("epoll_ctl DEL result: " << epoll_result);
		if (epoll_result == -1) {
			std::cerr << BOLD RED "Error: epoll_ctl DEL failed: " << strerror(errno) << RESET;
		}
		cleanHTTPElement(current_fd_);
		delete delete_client;
		clients_list_.erase(current_fd_);
		close(current_fd_);
		DEBUG_PRINT("Client cleanup complete, remaining clients: " << clients_list_.size());
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "cleanClient() exited" RESET);
		return true;
	} else {
		DEBUG_PRINT("No client found for fd: " << current_fd_);
		DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "cleanClient() exited" RESET);
		return false;
	}
	DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "cleanClient() exited" RESET);
	return true;
}

void ServerManager::send408ErrorResponse(int fd) {
    const char* response = 
        "HTTP/1.1 408 Request Timeout\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Connection: close\r\n"
        "Content-Length: 62\r\n"
        "\r\n"
        "<html><body><h1>408 Request Timeout</h1></body></html>\r\n";
    ssize_t bytes_sent = send(fd, response, strlen(response), 0);
    if (bytes_sent == -1) {
        DEBUG_PRINT("Failed to send 408 response: " << strerror(errno));
    } else {
        DEBUG_PRINT("Sent 408 Response to client " << fd << " (" << bytes_sent << " bytes)");
    }
}

void ServerManager::checkClientTimeouts() {
    time_t now = time(NULL);
    std::vector<int> clients_to_remove;
    
    DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "checkClientTimeout() called" RESET);
    DEBUG_PRINT("CLIENT_TIMEOUT value: " << CLIENT_TIMEOUT);

    std::map<int, Client*>::iterator it;
    for (it = clients_list_.begin(); it != clients_list_.end(); ++it) {
        int fd = it->first;
        Client* client = it->second;
        if (client != NULL) {
            time_t inactive_time = now - client->last_activity;
            if (inactive_time >= CLIENT_TIMEOUT) {
                DEBUG_PRINT("Client " BOLD << fd << RESET " TIMED OUT (inactive for " UNDERLINE
                           << inactive_time << RESET " seconds, limit: " UNDERLINE << CLIENT_TIMEOUT << RESET ")");
                send408ErrorResponse(fd);
                clients_to_remove.push_back(fd);
            } else {
                DEBUG_PRINT("Client " << fd << " still active (inactive for " 
                           << inactive_time << "/" << CLIENT_TIMEOUT << " seconds)");
            }
        } else {
            DEBUG_PRINT("Client " << fd << " is NULL");
        }
    }
    DEBUG_PRINT("Clients to remove: " << clients_to_remove.size());
    for (std::vector<int>::iterator it = clients_to_remove.begin(); 
         it != clients_to_remove.end(); ++it) {
        DEBUG_PRINT("Cleaning up timed out client");
        cleanClient(*it);
    }
    DEBUG_PRINT(BOLD UNDERLINE BG_WHITE BLACK "checkClientTimeout() exited" RESET);
}

void ServerManager::shutdown() {
	cleanShutdown(*this);
};

/* BLOCKING PROBLEMATIC CLIENT ++ OPTION COUNTING STRIKE ATTEMPT*/
bool ServerManager::isBlocked(const void *IP) {
	return blocked_clients_list_.find(IP) != blocked_clients_list_.end();
}

void ServerManager::cleanHTTPElement(int current_fd_) {
	Client* client = clients_list_[current_fd_];
	if (client->current_request) {
		delete client->current_request;
		client->current_request = NULL;
	}
	if (client->current_response) {
		delete client->current_response;
		client->current_response = NULL;
	}
}