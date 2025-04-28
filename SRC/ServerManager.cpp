/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 11:26:00 by cofische          #+#    #+#             */
/*   Updated: 2025/04/28 16:01:20 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/ServerManager.hpp"

ServerManager::ServerManager(const std::string &inputFilename) {
	std::fstream configFile(inputFilename.c_str());
	readFile(configFile);

	//check on error of the config file (ex: incorrect format, missing essential elements) 
	// --> can be place before the call of the object in main ??
	// --> Write an information on how to write a config file for our server (README.md?)

	/********DEBUGGING*********/
	//Printing the server object to ensure they are well connected	
	std::vector<Server*>::iterator beg = servers.begin();
	std::vector<Server*>::iterator end = servers.end();
	for (; beg != end; ++beg)
		printServer(**beg);

	
	// std::cout << line << std::endl;
	/********DEBUGGING*********/
	
	//start epoll for listening
	
	//stop epoll
	
	//clear all the resources

}

ServerManager::~ServerManager() {
	std::cout << BOLD RED "closing MasterServer\n" RESET;
}

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
			if (line.find("server {") != std::string::npos) {
				servers.push_back(new Server(serverCount++)); // newServerObj will return a pointer to a new Server object that is created
				currentServer = servers.back();
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
	size_t pos;
	if (line.find("host") != std::string::npos) {
		if ((pos = line.rfind(":")) != std::string::npos)
			currentServer->setHost(line.substr(pos + 2));
	} else if (line.find("port") != std::string::npos) {
		if ((pos = line.rfind(":")) != std::string::npos)
			currentServer->setPort(convertInt(line.substr(pos + 2)));
	} else if (line.find("server_names") != std::string::npos) {
		if ((pos = line.rfind(":")) != std::string::npos)
			currentServer->addServerName(line.substr(pos + 2));
	} else if (line.find("error_page") != std::string::npos) {
		if ((pos = line.rfind(":")) != std::string::npos)
			currentServer->setErrorDir(line.substr(pos + 2));
	} else if (line.find("client_max_body_size") != std::string::npos) { // limit for the HTTP request body info
		if ((pos = line.rfind(":")) != std::string::npos) {
			std::string newLine = line.substr(pos + 2);
			newLine.erase(newLine.end() - 1);
			currentServer->setMaxSize(convertInt(newLine));
		}
	} else if (line.find("location") != std::string::npos) {
		parseLocation(line, currentServer, configFile);
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
	while (std::getline(configFile, line) && line.find("}") == std::string::npos) {
		if (line.find("location") != std::string::npos) {
			if ((pos = line.rfind(":")) != std::string::npos)
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
		} else if (line.find("autoindex") != std::string::npos) {
			if (line.find("on") != std::string::npos)
				currentLocation->setAutoIndex(true);
				//////////////////////////////////
		} else if (line.find("upload_enable") != std::string::npos) {
			if (line.find("on") != std::string::npos)
				currentLocation->setUpload(true);
		} else if (line.find("upload_dir") != std::string::npos) {
			if ((pos = line.rfind(":")) != std::string::npos)
				currentLocation->setUploadDir(line.substr(pos + 2));
		} else if (line.find("max_body_size") != std::string::npos) {
			if ((pos = line.rfind(":")) != std::string::npos) {
				std::string newLine = line.substr(pos + 2);
				newLine.erase(newLine.end() - 1);
				currentLocation->setMaxBodySize(static_cast<size_t>(convertInt(newLine)));
			}
				/////////////////////////////////
		} else if (line.find("cgi_enable") != std::string::npos) {
			if (line.find("on") != std::string::npos)
				currentLocation->setCGI(true);
		} else if (line.find("cgi_path") != std::string::npos) {
			if ((pos = line.rfind(":")) != std::string::npos)
				currentLocation->setCGIPath(line.substr(pos + 2));
		} else if (line.find("cgi_extensions") != std::string::npos) {
			if ((pos = line.rfind(":")) != std::string::npos)
				currentLocation->setCGIExt(line.substr(pos + 2));
				/////////////////////////////////
		} else if (line.find("redirect_enable") != std::string::npos) {
			if (line.find("on") != std::string::npos)
				currentLocation->setRedirect(true);
		} else if (line.find("redirect_code") != std::string::npos) { // do I need a redirect code or can I simply redirect to a default webpage?
			if ((pos = line.rfind(":")) != std::string::npos)
				currentLocation->setRedirectCode(convertInt(line.substr(pos + 2)));
		} else if (line.find("redirect_url") != std::string::npos) {
			if ((pos = line.rfind(":")) != std::string::npos)
				currentLocation->setRedirectURL(line.substr(pos + 2));
		} else
			return; // PRINTING ERROR AS UNKNOW CONFIG ELEMENT FOUND
	}	
}		