/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 10:31:14 by cofische          #+#    #+#             */
/*   Updated: 2025/04/10 11:39:05 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/ServerManager.hpp"

//Going to be a member function of the ServerManager class 
int	readFile(std::fstream &configFile, std::vector<Server*> &servers) {
	// pointer to server object that will be used to parse coming lines. If the object is new, the pointer will be replace in the if statement ?
	Server *currentServer =  nullptr;
	std::string line;
	while (std::getline(configFile, line)) {
		if (line.find("server") != std::string::npos)
			servers.push_back(newServerObj()); // newServerObj will return a pointer to a new Server object that is created
			currentServer = &servers.last();
			// create a new server object that will parse each line in a sub ServerConfig object.
		else
			parseLine(line, currentServer);
		//continue parsing the coming lines in the same server object	
	}
}


int main(int ac, char **av) {
	if (ac > 2) {
		std::cerr << "ERROR\n";
		return (-1);
	}
	std::string filename;
	if (ac < 1)
		filename = "../configuration/default.conf";
	else
		filename = av[1];
	//Start the ServerManager Object that will handle all the HTTP connections
	ServerManager masterServer(filename);
	
	// --> this will be placed in a ServerManager class that will read and parse info from the filename
	std::fstream configFile;
	configFile.open(filename.c_str(), std::fstream::in);
	//Read the file
	std::vector<Server*> servers;
	readFile(configFile, servers);
	//end of this function, I got a vector gathering all server object that have been parsed with their config info
	
	//start_servers

	return 0;
}