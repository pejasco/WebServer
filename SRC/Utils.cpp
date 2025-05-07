/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 16:24:47 by cofische          #+#    #+#             */
/*   Updated: 2025/05/07 15:22:56 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/ServerManager.hpp"
#include "../INC/utils/Utils.hpp"

int convertInt(const std::string &str) {
	int nb;
	std::istringstream (str) >> nb;
	//if no error
	return nb;
}

void printLocation(Location &location) {
	/*BASIC INFO*/
	std::cout << BOLD MAGENTA "location Path: " RESET << location.getPath() << std::endl;
	std::cout << BOLD MAGENTA "location root: " RESET << location.getRoot() << std::endl;
	//method display
	std::cout << BOLD MAGENTA "location methods: " RESET;
	std::vector<MET>::iterator itMet = location.getMethod().begin();
	std::vector<MET>::iterator iteMet = location.getMethod().end();
	for (; itMet != iteMet; ++ itMet) {
		if (*itMet == 0)
			std::cout << "GET ";
		else if (*itMet == 1)
			std::cout << "POST ";
		else if (*itMet == 2)
			std::cout << "DELETE ";
		else
			std::cout << BOLD RED "Not recognized " RESET;
	}
	std::cout << std::endl;
	
	std::cout << BOLD MAGENTA "location index: " RESET << location.getIndex() << std::endl;
	std::cout << BOLD MAGENTA "location directories listing: " RESET << location.getAutoIndex() << std::endl;
	
	/*UPLAOD INFO*/
	std::cout << BOLD MAGENTA "location - is upload enable: " RESET << location.getUpload() << std::endl;
	std::cout << BOLD MAGENTA "location upload storage folder: " RESET << location.getUploadDir() << std::endl;
	std::cout << BOLD MAGENTA "location upload Max body size: " RESET << location.getMaxSize() << std::endl;

	/*CGI INFO*/
	std::cout << BOLD MAGENTA "location - is CGI enable: " RESET << location.getCGI() << std::endl;
	//cgi extension list
	std::cout << BOLD MAGENTA "location CGI extensions: " RESET;
	std::vector<std::string>::iterator itExt = location.getCGIExt().begin();
	std::vector<std::string>::iterator iteExt = location.getCGIExt().end();
	for (; itExt != iteExt; ++ itExt)
		std::cout << *itExt << " ";
	std::cout << std::endl;

	/*REDIRECTION INFO*/
	std::cout << BOLD MAGENTA "location - is redirection enable: " RESET << location.getRedirect() << std::endl;
	std::cout << BOLD MAGENTA "location redirection code: " RESET << location.getRedirectCode() << std::endl;
	std::cout << BOLD MAGENTA "location redirection URL: " RESET << location.getRedirectURL() << std::endl;
	
}

void printServer(Server &server) {
	/*SERVER NAMES*/
	std::vector<std::string>::iterator it = server.getServerName().begin();
	std::vector<std::string>::iterator ite = server.getServerName().end();
	std::cout << BOLD BLUE << "server name: " RESET;
	for (; it != ite; ++it)
		std::cout << *it << " ";
	std::cout << std::endl;
	
	/*PORT AND HOST*/
	std::cout << BOLD BLUE "server host: " << RESET << server.getHost() << std::endl;
	std::vector<std::string>::iterator itp = server.getPort().begin();
	std::vector<std::string>::iterator itep = server.getPort().end();
	std::cout << BOLD BLUE << "server ports: " RESET;
	for (; itp != itep; ++itp)
		std::cout << *itp << " ";
	std::cout << std::endl;

	/*ERROR PAGES CODE LIST*/
	std::map<int, std::string>::iterator itr = server.getErrorDir().begin();
	std::map<int, std::string>::iterator iter = server.getErrorDir().end();
	std::cout << BOLD BLUE << "error page: " RESET;
	for (; itr != iter; ++itr)
		std::cout << itr->first << " " << itr->second << std::endl;
	
	/*MAX BODY SIZE*/
	std::cout << BOLD BLUE "server maxsize: " RESET << server.getMaxSize() << std::endl;
	std::cout << BOLD BLUE "server keep alive: " RESET << server.getKeepAlive() << std::endl;
	
	/*LOCATIONS BLOCKS*/
	std::vector<Location*>::iterator itL = server.getLocation().begin();
	std::vector<Location*>::iterator iteL = server.getLocation().end();
	for (; itL != iteL; ++itL) {
		printLocation(**itL);
		std::cout << std::endl;
	}
}

bool isMessageCompleted(const std::string &request) {
	size_t pos = 0;
	std::string temp_body_message;
	int body_size = 0; 
	if ((pos = request.find("Content-Length: ")) != std::string::npos) {
		body_size = convertInt(request.substr(pos + 16));
		std::cout << "body_size: " << body_size << std::endl;
		if ((pos = request.find("\r\n\r\n")) != std::string::npos) {
			temp_body_message = request.substr(pos + 4);
			if (temp_body_message.size() < static_cast<unsigned long>(body_size))
				return false;
			else
				return true;
		}
	}
	return true;
}

void cleanShutdown(ServerManager &masterServer) {

	//closing socket_fd from the servers and freeing the struct
	std::vector<Socket*>::iterator begSo = masterServer.getSocket().begin();
	std::vector<Socket*>::iterator endSo = masterServer.getSocket().end();
	for (; begSo != endSo; ++begSo) {
		delete *begSo;
	}

	//closing and deleting client info
	std::map<int,Client*>::iterator begCl = masterServer.getClients().begin();
	std::map<int,Client*>::iterator endCl = masterServer.getClients().end();
	for (; begCl != endCl; ++begCl) {
		close(begCl->first);
		delete begCl->second;
	}

	//freeing the struct server
	std::vector<Server*>::iterator beg = masterServer.getServers().begin();
	std::vector<Server*>::iterator end = masterServer.getServers().end();

	for (; beg != end; ++beg) {
		std::vector<Location*>::iterator begLo = (*beg)->getLocation().begin();
		std::vector<Location*>::iterator endLo = (*beg)->getLocation().end();
		for (; begLo != endLo; ++begLo)
			delete *begLo;
		delete *beg;
	}

	close(masterServer.getEpollFd());
}
