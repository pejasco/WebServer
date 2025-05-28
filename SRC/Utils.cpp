/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssottori <ssottori@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 16:24:47 by cofische          #+#    #+#             */
/*   Updated: 2025/05/28 03:13:34 by ssottori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/ServerManager.hpp"
#include "../INC/utils/Utils.hpp"
#include <cstring>

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
		body_size = convertToNb<int>(request.substr(pos + 16));
		// std::cout << "body_size: " << body_size << std::endl;
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

std::string getStatusStr(int status_code) {
	//Harcode of the table for status code and status string
	std::map<int, std::string> statusMessages;
	statusMessages.insert(std::make_pair(200, "OK"));
	statusMessages.insert(std::make_pair(204, "No Content"));
	statusMessages.insert(std::make_pair(400, "Bad Request"));
	statusMessages.insert(std::make_pair(401, "Unauthorized"));
	statusMessages.insert(std::make_pair(404, "Not Found"));
	statusMessages.insert(std::make_pair(405, "Method Not Allowed"));
	statusMessages.insert(std::make_pair(415, "Unsupported Media Type"));
	statusMessages.insert(std::make_pair(500, "Internal Server Error"));
	statusMessages.insert(std::make_pair(502, "Bad Gateway"));
	
	std::map<int, std::string>::iterator beg = statusMessages.begin();
	std::map<int, std::string>::iterator end = statusMessages.end();
	for (; beg != end; ++beg) {
		if (status_code == beg->first)
			return beg->second;
	}
	return "error";
}

bool fileExists(const std::string& filename) {
	std::ifstream file(filename.c_str());
	return file.good();
}

std::string getContentType(const std::string &inputExtension) {
	std::string extension = toLowerCase(inputExtension);
	std::map<std::string, std::string> contentType;
   // HTML/Text files
	contentType.insert(std::make_pair(".html", "text/html; charset=UTF-8"));
	contentType.insert(std::make_pair(".htm", "text/html; charset=UTF-8"));
	contentType.insert(std::make_pair(".css", "text/css"));
	contentType.insert(std::make_pair(".js", "application/javascript"));
	contentType.insert(std::make_pair(".txt", "text/plain; charset=UTF-8"));
	contentType.insert(std::make_pair(".csv", "text/csv"));
	contentType.insert(std::make_pair(".xml", "application/xml"));
	contentType.insert(std::make_pair(".json", "application/json"));
	
	// Image files
	contentType.insert(std::make_pair(".jpg", "image/jpeg"));
	contentType.insert(std::make_pair(".jpeg", "image/jpeg"));
	contentType.insert(std::make_pair(".png", "image/png"));
	contentType.insert(std::make_pair(".gif", "image/gif"));
	contentType.insert(std::make_pair(".svg", "image/svg+xml"));
	contentType.insert(std::make_pair(".ico", "image/x-icon"));
	contentType.insert(std::make_pair(".webp", "image/webp"));
	contentType.insert(std::make_pair(".bmp", "image/bmp"));
	contentType.insert(std::make_pair(".tiff", "image/tiff"));
	contentType.insert(std::make_pair(".tif", "image/tiff"));
	
	// Audio/Video files
	contentType.insert(std::make_pair(".mp3", "audio/mpeg"));
	contentType.insert(std::make_pair(".wav", "audio/wav"));
	contentType.insert(std::make_pair(".ogg", "audio/ogg"));
	contentType.insert(std::make_pair(".mp4", "video/mp4"));
	contentType.insert(std::make_pair(".webm", "video/webm"));
	contentType.insert(std::make_pair(".mpeg", "video/mpeg"));
	contentType.insert(std::make_pair(".mpg", "video/mpeg"));
	
	// Document files
	contentType.insert(std::make_pair(".pdf", "application/pdf"));
	contentType.insert(std::make_pair(".doc", "application/msword"));
	contentType.insert(std::make_pair(".docx", "application/msword"));
	contentType.insert(std::make_pair(".xls", "application/vnd.ms-excel"));
	contentType.insert(std::make_pair(".xlsx", "application/vnd.ms-excel"));
	contentType.insert(std::make_pair(".ppt", "application/vnd.ms-powerpoint"));
	contentType.insert(std::make_pair(".pptx", "application/vnd.ms-powerpoint"));
	contentType.insert(std::make_pair(".odt", "application/vnd.oasis.opendocument.text"));
	
	// Archive files
	contentType.insert(std::make_pair(".zip", "application/zip"));
	contentType.insert(std::make_pair(".gz", "application/gzip"));
	contentType.insert(std::make_pair(".tar", "application/x-tar"));
	contentType.insert(std::make_pair(".rar", "application/vnd.rar"));
	contentType.insert(std::make_pair(".7z", "application/x-7z-compressed"));
	
	// Font files
	contentType.insert(std::make_pair(".ttf", "font/ttf"));
	contentType.insert(std::make_pair(".otf", "font/otf"));
	contentType.insert(std::make_pair(".woff", "font/woff"));
	contentType.insert(std::make_pair(".woff2", "font/woff2"));

	//CGI
	contentType.insert(std::make_pair(".py", "text/plain"));
	contentType.insert(std::make_pair(".sh", "text/plain"));
	
	std::map<std::string, std::string>::iterator beg = contentType.begin();
	std::map<std::string, std::string>::iterator end = contentType.end();
	for (; beg != end; ++beg) {
		if (extension == beg->first) {
			return beg->second;
		}
			
	}
	return "application/octet-stream";
}

std::string getServerIP(int socketFd) {
	struct sockaddr_in serverAddr;
	socklen_t serverAddrLen = sizeof(serverAddr);
	
	// Get the server socket's address information
	if (getsockname(socketFd, (struct sockaddr*)&serverAddr, &serverAddrLen) == 0) {
		char ipStr[INET_ADDRSTRLEN];
		if (inet_ntop(AF_INET, &serverAddr.sin_addr, ipStr, INET_ADDRSTRLEN)) {
			return std::string(ipStr);
		}
	}
	
	return "unknown";
}

std::string toLowerCase(const std::string& input) {
	std::string result = input;
	for (size_t i = 0; i < result.length(); ++i) {
		result[i] = std::tolower(result[i]);
	}
	return result;
}

int calculateFileSize(std::string &filename) {
	std::streampos pos = -1;
	std::ifstream body_file(filename.c_str(), std::ios::binary);
	if (body_file.is_open()) {
		pos = body_file.tellg();
		body_file.seekg(0, std::ios::end);
		pos = body_file.tellg() - pos;
		body_file.close();
	} else
		std::cout << "Error: " << strerror(errno) << std::endl;
	int length = static_cast<int>(pos);
	return length;
}

size_t getMaxSize(const std::string &inputSize) {
	//B -> size
	//KB -> size * 1024
	//MB -> size * 1024 * 1024
	//GB -> size * 1024 * 1024 * 1024
	size_t size = 0;
	char value = inputSize[-2];
	if (value == ' ')
		return size = convertToNb<size_t>(inputSize);
	else if (value == 'K' || value == 'k')
		return size = (convertToNb<size_t>(inputSize)) * 1024;
	else if (value == 'M' || value == 'm')
		return size = (convertToNb<size_t>(inputSize)) * 1024 * 1024;
	else if (value == 'G' || value == 'g')
		return size = (convertToNb<size_t>(inputSize)) * 1024 * 1024 * 1024;
	else {
		//std::cerr << " Error: size not recognized\n";
		return 0;
	}
		
}

Server *getCurrentServer(const HTTPRequest &inputRequest, ServerManager &serverManager, const std::string &serverIP) {
	std::vector<Server*>::iterator begSe = serverManager.getServers().begin();
	std::vector<Server*>::iterator endSe = serverManager.getServers().end();
	
	for (; begSe != endSe; ++begSe) {
		if ((*begSe)->getHost() == serverIP) {
			std::cout << "current server: " << (*begSe)->getHost() << ", request host: " << serverIP << std::endl;
			std::vector<std::string> tempPort = (*begSe)->getPort();
			std::vector<std::string>::iterator begPo = tempPort.begin();
			std::vector<std::string>::iterator endPo = tempPort.end();
			for (; begPo != endPo; ++ begPo) {
				std::cout << "current port " << *begPo << ", request port: " << inputRequest.getHost() << std::endl;
				if (inputRequest.getHost() == *begPo) {
					std::cout << "found the matching port, bye\n";
					return *begSe; 
				}
					
			}
		}
	}
	return serverManager.getServers().front();
}

Location *getCurrentLocation(const HTTPRequest &inputRequest, Server &currentServer) {
	if (currentServer.getLocation().empty())
		return NULL;
	else {
		std::vector<Location*>::iterator begLo = currentServer.getLocation().begin();
		std::vector<Location*>::iterator endLo = currentServer.getLocation().end();
		for (; begLo != endLo; ++begLo) {
			std::string requestPath = formatURL(inputRequest.getPath());
			std::cout << "Request: " << requestPath << ", request size: " << requestPath << ", server location: " << (*begLo)->getPath() << ", size: " << (*begLo)->getPath().size() << std::endl;
			if (requestPath == (*begLo)->getPath()) {
				std::cout << "found the matching location, bye\n";
				return *begLo;
			}		
		}
		if (inputRequest.getPath() == "/")
			return NULL;
		else 
			return currentServer.getLocation().front();
	}
}

std::string formatURL(const std::string &input) {
	std::cout << "input before processing: " << input << std::endl;
	if (input.empty() || input[0] != '/') {
		return input;
	}
	std::string::size_type secondSlashPos = input.find('/', 1);
	if (secondSlashPos != std::string::npos) {
		return input.substr(0, secondSlashPos);
	} else {
		return input;
	}
}

std::string getFilenameFromPath(const std::string& path) {
	std::string::size_type slashPos = path.rfind('/');
	if (slashPos != std::string::npos) {
		return path.substr(slashPos + 1);
	} else {
		return path;
	}
}

/*
///
/// Get me my file size in bytes (long long to support any file size supported by your OS.
///
long long Logger::getFileSize()
{
	std::streampos fsize = 0;

	std::ifstream myfile ("myfile.txt", ios::in);  // File is of type const char*

	fsize = myfile.tellg();         // The file pointer is currently at the beginning
	myfile.seekg(0, ios::end);      // Place the file pointer at the end of file

	fsize = myfile.tellg() - fsize;
	myfile.close();

	static_assert(sizeof(fsize) >= sizeof(long long), "Oops.");

	cout << "size is: " << fsize << " bytes.\n";
	return fsize;
}

*/