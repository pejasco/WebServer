/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 11:26:00 by cofische          #+#    #+#             */
/*   Updated: 2025/06/11 15:18:09 by cofische         ###   ########.fr       */
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
    int loop_counter = 0;
    int max_loops = 100;
    std::cout << "=== SERVER MONITORING STARTED ===" << std::endl;
    
    while (running_ && loop_counter < max_loops) {
        loop_counter++;
        std::cout << "=== LOOP " << loop_counter << " - WAITING FOR EPOLL_WAIT ===" << std::endl;
        
        num_events_ = epoll_wait(epoll_fd_, events_, MAX_EVENTS, 1000);
        std::cout << "=== EPOLL_WAIT RETURNED: " << num_events_ << " events ===" << std::endl;
        
        if (num_events_ == -1) {
            std::cout << "EPOLL ERROR: " << strerror(errno) << std::endl;
            break;
        }
        
        if (num_events_ == 0) {
            std::cout << "=== TIMEOUT - NO EVENTS ===" << std::endl;
            continue;
        }
        
        // Process events - ADD THE MISSING LOGIC!
        for (client_id_ = 0; client_id_ < num_events_; client_id_++) {
            current_fd_ = events_[client_id_].data.fd;
            std::cout << "=== EVENT " << client_id_ << " FD " << current_fd_ 
                      << " EVENTS: " << events_[client_id_].events << " ===" << std::endl;
            
            // ADD THIS - Check if it's a new connection or existing client
            if (std::find(sockets_fd_list_.begin(), sockets_fd_list_.end(), current_fd_) != sockets_fd_list_.end()) {
                std::cout << "=== NEW CLIENT CONNECTION ===" << std::endl;
                createNewClientConnection();
            } else {
                std::cout << "=== EXISTING CLIENT CONNECTION ===" << std::endl;
                Client *current_client = clients_list_[current_fd_];
                if (current_client == NULL) {
                    std::cout << "=== ERROR: UNKNOWN CLIENT ===" << std::endl;
                    // Clean up unknown client
                    close(current_fd_);
                    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, current_fd_, NULL);
                } else {
                    existingClientConnection();
                }
            }
            std::cout << "=== FINISHED PROCESSING EVENT " << client_id_ << " ===" << std::endl;
        }
        std::cout << "=== FINISHED PROCESSING ALL EVENTS ===" << std::endl;
    }
    
    if (loop_counter >= max_loops) {
        std::cout << "=== STOPPED DUE TO LOOP LIMIT ===" << std::endl;
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
	if (new_client->getError()) {
		blocked_clients_list_.insert(new_client->getClientIP());
	} else if (isBlocked(new_client->getClientIP())) {
		close(new_client->getClientFd());
		std::cerr << BOLD RED "Error\n" RESET "Client has been blocked\n";	
	}
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

void ServerManager::existingClientConnection() {
    std::cout << ">>> ENTERING existingClientConnection for FD " << current_fd_ << std::endl;
    
    Client* client = clients_list_[current_fd_];
    if (!client) {
        std::cout << ">>> ERROR: No client found for FD " << current_fd_ << std::endl;
        return;
    }
    
    std::cout << ">>> Client found - file_sending_complete: " << client->file_sending_complete << std::endl;
    std::cout << ">>> Event type: " << events_[client_id_].events << std::endl;
    std::cout << ">>> EPOLLIN check: " << (events_[client_id_].events & EPOLLIN) << std::endl;
    
    // Handle file sending if in progress
    if (!client->file_sending_complete) {
        std::cout << ">>> File sending in progress" << std::endl;
        if (events_[client_id_].events & EPOLLOUT) {
            std::cout << ">>> EPOLLOUT detected" << std::endl;
            if (sendResponseBodyFile()) {
                client->resetForNextRequest();
            }
            return;
        } else if (events_[client_id_].events & EPOLLIN) {
            std::cout << ">>> ERROR: Got EPOLLIN while file sending in progress - resetting client" << std::endl;
            client->resetForNextRequest();
            // Fall through to handle the EPOLLIN event
        } else {
            std::cout << ">>> Waiting for EPOLLOUT" << std::endl;
            return;
        }
    }

    std::cout << ">>> ABOUT TO CHECK EPOLLIN CONDITION" << std::endl;
    
    // Handle new request data - THIS IS THE CRITICAL PART
    if (events_[client_id_].events & EPOLLIN) {
        std::cout << ">>> EPOLLIN CONDITION TRUE - ENTERING PROCESSING" << std::endl;
        HTTPRequest current_request;
        std::cout << "check EPOLLIN ready\n";
        
        // PHASE 1: Read headers if not complete
        if (!client->header_completed) {
            std::cout << ">>> CALLING readClientHeaders()" << std::endl;
            if (!readClientHeaders()) {
                std::cout << "check client header not completed\n";
                return; // Headers not complete yet or error
            }
            client->header_completed = true;
            std::cout << "check client header completed\n";
            if (!parseHeadersAndCheckBodySize(current_request)) {
                std::cout << "check parse header and body return false\n";
                if (current_fd_ > 0)  
                    close(current_fd_);
                cleanClient(current_fd_);
                std::cout << "check client is clean\n";
                return;
            }
        } else {
            std::cout << ">>> HEADERS ALREADY COMPLETED - PROCESSING NEXT REQUEST" << std::endl;
            // ADD THIS: Handle subsequent requests on keep-alive connection
            client->header_completed = false; // Reset for next request
            if (!readClientHeaders()) {
                std::cout << ">>> Next request headers not complete yet\n";
                return;
            }
            client->header_completed = true;
            if (!parseHeadersAndCheckBodySize(current_request)) {
                std::cout << ">>> Error processing next request\n";
                if (current_fd_ > 0)  
                    close(current_fd_);
                cleanClient(current_fd_);
                return;
            }
        }
    } else {
        std::cout << ">>> EPOLLIN CONDITION FALSE" << std::endl;
    }
    
    std::cout << "check all finished\n";
    std::cout << ">>> EXITING existingClientConnection" << std::endl;
}

bool ServerManager::readClientHeaders() {
    Client* client = clients_list_[current_fd_];
    if (!client) return false;
    
    if (!(events_[client_id_].events & EPOLLIN))
        return false;

    ssize_t byte_received = recv(current_fd_, received_, sizeof(received_) - 1, 0);    
    if (byte_received <= 0) {
        if (byte_received == 0)
            std::cout << "Client disconnect during header reading\n";
        else
            std::cerr << "Error reading headers: " << strerror(errno) << std::endl;
        return false;
    }
    
    received_[byte_received] = '\0';
    client->header_buffer.append(received_, byte_received);
    
    // Check for end of headers
    size_t header_end = client->header_buffer.find("\r\n\r\n");
    if (header_end != std::string::npos) {
        // Store headers and body separately
        client->headers_string = client->header_buffer.substr(0, header_end + 4);
        size_t body_start = header_end + 4;
        if (body_start < client->header_buffer.length()) {
            client->body_buffer = client->header_buffer.substr(body_start);
            client->body_bytes_read = client->body_buffer.length();
        }
        
        return true; // Headers complete
    }
    
    // Header size check 
    if (client->header_buffer.size() > MAX_HEADER_SIZE) {
        error_code_ = 400;
        return false;
    }
    
    return false; // Headers not complete yet
}

bool ServerManager::parseHeadersAndCheckBodySize(HTTPRequest& current_request) {
    Client* client = clients_list_[current_fd_];
    if (!client) return false;

    // Parse request using stored headers
    current_request.parseRequest(client->headers_string);
    
    std::cout << BOLD MAGENTA "\n#######HEADER RECEIVED##########\n\n" RESET 
              << client->headers_string << BOLD MAGENTA "\n#######END OF HEADER##########\n" RESET << std::endl; 

    std::cout << BOLD UNDERLINE GREEN "\n###### ENTERING SERVER//LOCATION DEBUGGING ######\n\n" RESET;
    std::string server_IP = getServerIP(current_fd_);
    default_server_ = servers_list_.front();
    Server *server_requested = getCurrentServer(current_request, *this, server_IP);
    Location *location_requested = getCurrentLocation(current_request, *server_requested);
    if (location_requested == NULL && (current_request.getPath() == "/"))
        location_requested = servers_list_.front()->getLocationsList().front();
    std::cout << BOLD UNDERLINE GREEN "\n###### LEAVING SERVER//LOCATION DEBUGGING ######\n\n" RESET;
    
    int max_body_size = maxBodySizeLocation(servers_list_.front(), server_requested, location_requested);
    std::cout << "max_body_size: " << max_body_size << std::endl;
    
    int content_length = current_request.getContentLength();
    std::cout << "Content-Length: " << content_length << std::endl;    
    
    if (content_length > 0) {
        if (max_body_size > 0 && content_length > max_body_size) {
            std::cout << "Request body size " << content_length 
                        << " exceeds limit " << max_body_size << std::endl;
            error_code_ = 413;
            processAndSendResponse(current_request, server_requested, location_requested);
            return true;
        } else {
            std::cout << "Reading request body\n";
            if (!readRequestBody(current_request, content_length, max_body_size)) {
                std::cout << "Body not complete yet\n";
                return false; // Wait for more body data
            }
        }
    }
    
    std::cout << "Processing request and sending response\n";
    processAndSendResponse(current_request, server_requested, location_requested);
    return true;
}

bool ServerManager::readRequestBody(HTTPRequest& current_request, size_t content_length, size_t max_body_size) {
    Client* client = clients_list_[current_fd_];
    if (!client) return false;
    
    // Continue reading body if not complete
    if (client->body_bytes_read < content_length) {
        size_t remaining = content_length - client->body_bytes_read;
        size_t to_read = std::min(sizeof(received_) - 1, remaining);
        ssize_t byte_received = recv(current_fd_, received_, to_read, 0);
        
        if (byte_received < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return false; // No more data available, try again later
            }
            std::cerr << "Error reading body: " << strerror(errno) << std::endl;
            return false;
        } else if (byte_received == 0) {
            std::cout << "Client disconnect during body reading\n";
            return false;
        }
        
        client->body_bytes_read += byte_received;
        if (max_body_size > 0 && client->body_bytes_read > max_body_size) {
            std::cerr << "Body size exceeded during reading\n";
            error_code_ = 413;
            return false;
        }
        
        received_[byte_received] = '\0';
        client->body_buffer.append(received_, byte_received);
    }
    
    // Check if body is complete
    if (client->body_bytes_read >= content_length) {
        std::cout << "<<sievdebug>> request_body received (" << client->body_buffer.size() << " bytes):\n";
        std::cout << "<<sievdebug>>" << client->body_buffer << "\n";
        std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << std::endl;
        
        current_request.parseContent(client->body_buffer);
        return true; // Body complete
    } 
    
    return false; // Need more data
}

void ServerManager::processAndSendResponse(HTTPRequest& current_request, Server *server_requested, Location *location_requested) {
    Client* client = clients_list_[current_fd_];
    if (!client) return;
    
    // Create response and store in client
    HTTPResponse* http_response = new HTTPResponse(current_request, default_server_, server_requested, location_requested, error_code_);
    client->setResponse(http_response);
    
    std::string response_headers = client->current_response->getResponse();
    std::cout << "response to send: " << response_headers << std::endl;
    
    if (current_fd_ < 0) {
        std::cerr << "Error, current_fd_ socket is already close" << std::endl;
        return;
    }
    if (response_headers.empty()) {
        std::cerr << "Error, response is empty" << std::endl;
        return;
    }
    
    ssize_t bytes_sent = send(current_fd_, response_headers.c_str(), response_headers.length(), 0);
    if (bytes_sent < 0) {
        std::cerr << "Error when sending response to client: " << strerror(errno) << std::endl;
        return;
    }

    if (client->current_response->isReady()) {
        std::cout << "Response is already ready (e.g. from CGI), skipping body file send\n";
        client->resetForNextRequest(); // IMPORTANT: Reset client state
        return;
    }
    
    if (!client->current_response->getBodyFilename().empty()) {
        client->file_sending_complete = false;
        sendResponseBodyFile();
    } else {
        std::cout << "SUCCESSFULLY COMPLETED RESPONSE (no body file)!\n";
        client->resetForNextRequest(); // IMPORTANT: Reset client state
    }
}

bool ServerManager::sendResponseBodyFile() {
    Client* client = clients_list_[current_fd_];
    if (!client) return false;
    
    std::string body_file = client->current_response->getBodyFilename();
    
    // Initialize file reading state if first call
    if (!client->file_stream.is_open()) {
        std::cout << "check body_file: " << body_file << std::endl;
        client->file_stream.open(body_file.c_str(), std::ios::binary);
        if (!client->file_stream.is_open()) {
            std::cerr << "Failed to open body file: " << body_file << std::endl;
            error_code_ = 500;
            return false;
        }
        client->file_sending_complete = false;
    }
    
    // Check if socket is ready for writing
    if (!(events_[client_id_].events & EPOLLOUT)) {
        // Need to wait for EPOLLOUT event
        struct epoll_event event;
        event.events = EPOLLOUT;
        event.data.fd = current_fd_;
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, current_fd_, &event) == -1) {
            std::cerr << "Error modifying epoll for EPOLLOUT: " << strerror(errno) << std::endl;
            return false;
        }
        return false; // Will continue when EPOLLOUT triggers
    }
    
    // Read chunk from file
    if (!client->file_stream.eof()) {
        client->file_stream.read(buffer_, sizeof(buffer_));
        std::streamsize bytes_read = client->file_stream.gcount();
        
        if (bytes_read > 0) {
            ssize_t bytes_sent = send(current_fd_, buffer_, bytes_read, 0);
            if (bytes_sent < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    // Socket buffer full, try again later
                    client->file_stream.seekg(-bytes_read, std::ios::cur); // Rewind
                    return false;
                }
                std::cerr << "Error sending file data: " << strerror(errno) << std::endl;
                client->file_stream.close();
                return false;
            } else if (bytes_sent < bytes_read) {
                // Partial send - rewind file position
                client->file_stream.seekg(bytes_sent - bytes_read, std::ios::cur);
                return false; // Continue on next EPOLLOUT
            }
        }
        
        if (client->file_stream.bad()) {
            std::cerr << "Error reading from file: " << body_file << std::endl;
            client->file_stream.close();
            return false;
        }
    }
    
    // Check if file sending is complete
    if (client->file_stream.eof()) {
        client->file_stream.close();
        client->file_sending_complete = true;
        
        // Clean up auto-generated file if needed
        if (client->current_response->isAutoIndex()) {
            if (std::remove(body_file.c_str()) == 0) {
                client->current_response->setAutoIndex(false);
            }
        }
        
        // Switch back to EPOLLIN for next request
        struct epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = current_fd_;
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, current_fd_, &event) == -1) {
            std::cerr << "Error modifying epoll back to EPOLLIN: " << strerror(errno) << std::endl;
        }
        
        std::cout << "SUCCESSFULLY COMPLETED FILE SENDING!\n";
        return true; // File sending complete
    }
    
    return false; // More data to send, continue on next EPOLLOUT
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

/* BLOCKING PROBLEMATIC CLIENT ++ OPTION COUNTING STRIKE ATTEMPT*/
bool ServerManager::isBlocked(const void *IP) {
	return blocked_clients_list_.find(IP) != blocked_clients_list_.end();
}

