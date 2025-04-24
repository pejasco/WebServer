/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 11:26:00 by cofische          #+#    #+#             */
/*   Updated: 2025/04/24 12:39:25 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/ServerManager.hpp"

int convertInt(const std::string &str) {
	int nb;
	std::istringstream (str) >> nb;
	//if no error
	return nb;
}


ServerManager::ServerManager(const std::string &inputFilename) {
	std::fstream configFile;
	configFile.open(inputFilename.c_str(), std::fstream::in);
	readFile(configFile);
	
	//Printing the server object to ensure they are well connected	
	std::vector<Server*>::iterator beg = servers.begin();
	std::vector<Server*>::iterator end = servers.end();
	for (; beg != end; ++beg)
		printServer(**beg);
	
	//start epoll for listening
	
	//stop epoll
	
	//clear all the resources

}

ServerManager::~ServerManager() {
	std::cout << BOLD RED "closing MasterServer\n" RESET;
}

int	ServerManager::readFile(std::fstream &configFile) {
	// pointer to server object that will be used to parse coming lines. If the object is new, the pointer will be replace in the if statement ?
	Server *currentServer =  NULL;
	std::string line;
	int serverCount = 1000; //starting at 1000 to avoid number confusion on other attribute (check if efficient on other code part)
	if (configFile.is_open()) {
		while (std::getline(configFile, line)) {
			// std::cout << line << std::endl;
			if (line.find("server {") != std::string::npos) {
				servers.push_back(new Server(serverCount++)); // newServerObj will return a pointer to a new Server object that is created
				currentServer = servers.back();
				// create a new server object that will parse each line in a sub ServerConfig object.
			} else {
				// std::cout << line << std::endl;
				parseLine(line, currentServer);
				// check validity of the info (ex: port in a correct range, forbidden char, etc...)  
			//continue parsing the coming lines in the same server object	
			}
		}
		configFile.close();
	} else {
		std::cout << BOLD RED "file incorrect\n" RESET;
	}
	return 0;
}

void ServerManager::parseLine(std::string &line, Server *currentServer) {
	// ignoring char
	/*
	std::string str = "Hello World";
	size_t pos;
	if ((pos = str.find("W")) != std::string::npos) {
		std::string sub_str = str.substr(++pos);
		std::cout  <<  "Substring after 'W': "  <<  sub_str  <<  std::endl;
	}
	*/
	size_t pos;
	if (line.find("host") != std::string::npos) {
		if ((pos = line.rfind(":")) != std::string::npos)
			currentServer->setHost(line.substr(pos + 2));
	} else if (line.find("port") != std::string::npos) {
		if ((pos = line.rfind(":")) != std::string::npos)
			currentServer->setPort(convertInt(line.substr(pos + 2))); // BUILD HELPING FUNCTION == CONVERT TO INT
	} else if (line.find("server_names") != std::string::npos) {
		//specific as the names will be on the same line
		if ((pos = line.rfind(":")) != std::string::npos) //need to tokenize to split the line between the name (separated by a single space)
			
			currentServer->addServerName(line.substr(pos + 2));
	} else if (line.find("error_pages") != std::string::npos) {
		//specific as the page error can be on several lines. Maybe Create a separate class for Error page management
		if ((pos = line.rfind(":")) != std::string::npos)
			currentServer->setErrorDir(line.substr(pos + 2));
	} else if (line.find("client_max_body_size") != std::string::npos) {
		if ((pos = line.rfind(":")) != std::string::npos)
			currentServer->setMaxSize(convertInt(line.substr(pos + 2)));
	} else if (line.find("location") != std::string::npos) {
		//function to create a location class and subclass 
	} else 
		return ;
}



