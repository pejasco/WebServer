/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 16:24:47 by cofische          #+#    #+#             */
/*   Updated: 2025/06/10 11:28:48 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/ServerManager.hpp"
#include "../INC/utils/Utils.hpp"

void printLocation(Location &location) {
	/*BASIC INFO*/
	std::cout << BOLD MAGENTA "location Path: " RESET << location.getName() << std::endl;
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
	std::cout << BOLD MAGENTA "location directories listing: " RESET << location.isAutoIndex() << std::endl;
	
	/*UPLAOD INFO*/
	std::cout << BOLD MAGENTA "location - is upload enable: " RESET << location.isUpload() << std::endl;
	std::cout << BOLD MAGENTA "location upload storage folder: " RESET << location.getUploadDir() << std::endl;
	std::cout << BOLD MAGENTA "location upload Max body size: " RESET << location.getMaxSize() << std::endl;

	/*CGI INFO*/
	std::cout << BOLD MAGENTA "location - is CGI enable: " RESET << location.isCGI() << std::endl;
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
	std::vector<std::string>::iterator it = server.getServerNames().begin();
	std::vector<std::string>::iterator ite = server.getServerNames().end();
	std::cout << BOLD BLUE << "server name: " RESET;
	for (; it != ite; ++it)
		std::cout << *it << " ";
	std::cout << std::endl;
	
	/*PORT AND HOST*/
	std::cout << BOLD BLUE "server host: " << RESET << server.getIP() << std::endl;
	std::vector<std::string>::iterator itp = server.getPort().begin();
	std::vector<std::string>::iterator itep = server.getPort().end();
	std::cout << BOLD BLUE << "server ports: " RESET;
	for (; itp != itep; ++itp)
		std::cout << *itp << " ";
	std::cout << std::endl;

	/*ERROR PAGES CODE LIST*/
	std::map<int, std::string>::iterator itr = server.getErrorList().begin();
	std::map<int, std::string>::iterator iter = server.getErrorList().end();
	std::cout << BOLD BLUE << "error page: " RESET;
	for (; itr != iter; ++itr)
		std::cout << itr->first << " " << itr->second << std::endl;
	
	/*MAX BODY SIZE*/
	std::cout << BOLD BLUE "server maxsize: " RESET << server.getMaxBodySize() << std::endl;
	std::cout << BOLD BLUE "server keep alive: " RESET << server.isKeepAlive() << std::endl;
	
	/*LOCATIONS BLOCKS*/
	std::vector<Location*>::iterator itL = server.getLocationsList().begin();
	std::vector<Location*>::iterator iteL = server.getLocationsList().end();
	for (; itL != iteL; ++itL) {
		printLocation(**itL);
		std::cout << std::endl;
	}
}

bool is_file_empty(const std::string &config_file) {
	std::ifstream filename(config_file.c_str());
	return filename.tellg() == 0 && filename.peek() == std::ifstream::traits_type::eof();
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

void cleanShutdown(ServerManager &master_server) {

	//closing socket_fd from the servers and freeing the struct
	std::vector<Socket*>::iterator begSo = master_server.getSockets().begin();
	std::vector<Socket*>::iterator endSo = master_server.getSockets().end();
	for (; begSo != endSo; ++begSo) {
		if (*begSo != NULL)
			delete *begSo;
	}

	//closing and deleting client info
	std::map<int,Client*>::iterator begCl = master_server.getClients().begin();
	std::map<int,Client*>::iterator endCl = master_server.getClients().end();
	for (; begCl != endCl; ++begCl) {
		close(begCl->first);
		if ((begCl)->second != NULL)
			delete begCl->second;
	}

	//freeing the struct server
	std::vector<Server*>::iterator beg = master_server.getServers().begin();
	std::vector<Server*>::iterator end = master_server.getServers().end();

	for (; beg != end; ++beg) {
		std::vector<Location*>::iterator begLo = (*beg)->getLocationsList().begin();
		std::vector<Location*>::iterator endLo = (*beg)->getLocationsList().end();
		for (; begLo != endLo; ++begLo) {
			if (*begLo != NULL)
				delete *begLo;
		}
		if (*beg != NULL)
			delete *beg;
	}
	close(master_server.getEpollFd());
}

std::string getStatusStr(int status_code) {
	//Harcode of the table for status code and status string
	std::map<int, std::string> status_messages;
	status_messages.insert(std::make_pair(200, "OK"));
	status_messages.insert(std::make_pair(204, "No Content"));
	status_messages.insert(std::make_pair(400, "Bad Request"));
	status_messages.insert(std::make_pair(401, "Unauthorized"));
	status_messages.insert(std::make_pair(404, "Not Found"));
	status_messages.insert(std::make_pair(405, "Method Not Allowed"));
	status_messages.insert(std::make_pair(415, "Unsupported Media Type"));
	status_messages.insert(std::make_pair(500, "Internal Server Error"));
	status_messages.insert(std::make_pair(502, "Bad Gateway"));
	
	std::map<int, std::string>::iterator beg = status_messages.begin();
	std::map<int, std::string>::iterator end = status_messages.end();
	for (; beg != end; ++beg) {
		if (status_code == beg->first)
			return beg->second;
	}
	return "error";
}

bool fileExists(const std::string& filename) {
	std::ifstream file(filename.c_str());
	std::cout << "Does file " << filename << " exist? " << file.good() << std::endl;
	return file.good();
}

std::string getContentType(const std::string &input_extension) {
	std::string extension = toLowerCase(input_extension);
	std::map<std::string, std::string> content_type;
   // HTML/Text files
	content_type.insert(std::make_pair(".html", "text/html; charset=UTF-8"));
	content_type.insert(std::make_pair(".htm", "text/html; charset=UTF-8"));
	content_type.insert(std::make_pair(".css", "text/css"));
	content_type.insert(std::make_pair(".js", "application/javascript"));
	content_type.insert(std::make_pair(".txt", "text/plain; charset=UTF-8"));
	content_type.insert(std::make_pair(".csv", "text/csv"));
	content_type.insert(std::make_pair(".xml", "application/xml"));
	content_type.insert(std::make_pair(".json", "application/json"));
	
	// Image files
	content_type.insert(std::make_pair(".jpg", "image/jpeg"));
	content_type.insert(std::make_pair(".jpeg", "image/jpeg"));
	content_type.insert(std::make_pair(".png", "image/png"));
	content_type.insert(std::make_pair(".gif", "image/gif"));
	content_type.insert(std::make_pair(".svg", "image/svg+xml"));
	content_type.insert(std::make_pair(".ico", "image/x-icon"));
	content_type.insert(std::make_pair(".webp", "image/webp"));
	content_type.insert(std::make_pair(".bmp", "image/bmp"));
	content_type.insert(std::make_pair(".tiff", "image/tiff"));
	content_type.insert(std::make_pair(".tif", "image/tiff"));
	
	// Audio/Video files
	content_type.insert(std::make_pair(".mp3", "audio/mpeg"));
	content_type.insert(std::make_pair(".wav", "audio/wav"));
	content_type.insert(std::make_pair(".ogg", "audio/ogg"));
	content_type.insert(std::make_pair(".mp4", "video/mp4"));
	content_type.insert(std::make_pair(".webm", "video/webm"));
	content_type.insert(std::make_pair(".mpeg", "video/mpeg"));
	content_type.insert(std::make_pair(".mpg", "video/mpeg"));
	
	// Document files
	content_type.insert(std::make_pair(".pdf", "application/pdf"));
	content_type.insert(std::make_pair(".doc", "application/msword"));
	content_type.insert(std::make_pair(".docx", "application/msword"));
	content_type.insert(std::make_pair(".xls", "application/vnd.ms-excel"));
	content_type.insert(std::make_pair(".xlsx", "application/vnd.ms-excel"));
	content_type.insert(std::make_pair(".ppt", "application/vnd.ms-powerpoint"));
	content_type.insert(std::make_pair(".pptx", "application/vnd.ms-powerpoint"));
	content_type.insert(std::make_pair(".odt", "application/vnd.oasis.opendocument.text"));
	
	// Archive files
	content_type.insert(std::make_pair(".zip", "application/zip"));
	content_type.insert(std::make_pair(".gz", "application/gzip"));
	content_type.insert(std::make_pair(".tar", "application/x-tar"));
	content_type.insert(std::make_pair(".rar", "application/vnd.rar"));
	content_type.insert(std::make_pair(".7z", "application/x-7z-compressed"));
	
	// Font files
	content_type.insert(std::make_pair(".ttf", "font/ttf"));
	content_type.insert(std::make_pair(".otf", "font/otf"));
	content_type.insert(std::make_pair(".woff", "font/woff"));
	content_type.insert(std::make_pair(".woff2", "font/woff2"));

	//CGI
	content_type.insert(std::make_pair(".py", "text/plain"));
	content_type.insert(std::make_pair(".sh", "text/plain"));
	
	std::map<std::string, std::string>::iterator beg = content_type.begin();
	std::map<std::string, std::string>::iterator end = content_type.end();
	for (; beg != end; ++beg) {
		if (extension == beg->first) {
			return beg->second;
		}
			
	}
	return "application/octet-stream";
}

std::string getServerIP(int socket_fd) {
	struct sockaddr_in server_addr;
	socklen_t server_addr_len = sizeof(server_addr);
	
	// Get the server socket's address information
	if (getsockname(socket_fd, (struct sockaddr*)&server_addr, &server_addr_len) == 0) {
		char ipStr[INET_ADDRSTRLEN];
		if (inet_ntop(AF_INET, &server_addr.sin_addr, ipStr, INET_ADDRSTRLEN)) {
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

int calculateFileSize(std::string &filename) { //auto_index.html
	std::streampos pos = -1;
	std::cout << "inside calculateFilesSize: " << filename << std::endl;
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

size_t getMaxSize(const std::string &input_size) {
	size_t size = 0;
	if (input_size.find("B") != std::string::npos || input_size.find("b") != std::string::npos)
		return size = convertToNb<size_t>(input_size);
	else if (input_size.find("K") != std::string::npos || input_size.find("k") != std::string::npos)
		return size = (convertToNb<size_t>(input_size)) * 1024;
	else if (input_size.find("M") != std::string::npos || input_size.find("m") != std::string::npos)
		return size = (convertToNb<size_t>(input_size)) * 1024 * 1024;
	else if (input_size.find("G") != std::string::npos || input_size.find("g") != std::string::npos)
		return size = (convertToNb<size_t>(input_size)) * 1024 * 1024 * 1024;
	else {
		return 0; 
	}
		
}

Server *getCurrentServer(const HTTPRequest &input_request, ServerManager &server_manager, const std::string &server_IP) {
	std::vector<Server*>::iterator begSe = server_manager.getServers().begin();
	std::vector<Server*>::iterator endSe = server_manager.getServers().end();
	
	for (; begSe != endSe; ++begSe) {
		if ((*begSe)->getIP() == server_IP) {
			std::cout << "current server: " << (*begSe)->getIP() << ", request host: " << server_IP << std::endl;
			std::vector<std::string> tempPort = (*begSe)->getPort();
			std::vector<std::string>::iterator begPo = tempPort.begin();
			std::vector<std::string>::iterator endPo = tempPort.end();
			for (; begPo != endPo; ++ begPo) {
				std::cout << "current port " << *begPo << ", request port: " << input_request.getHost() << std::endl;
				if (input_request.getHost() == *begPo) {
					std::cout << "found the matching port, bye\n";
					return *begSe; 
				}
					
			}
		}
	}
	return server_manager.getServers().front();
}

Location *getCurrentLocation(const HTTPRequest &input_request, Server &current_server) {
	if (current_server.getLocationsList().empty()) {
		std::cout << "is NULL\n";
		return NULL; 
	} else 
		std::cout << "is not null\n";
		 
	std::string request_path = input_request.getPath();
	std::cout << "URL to look for: " << request_path << std::endl;
	// Handle root request specially if needed
	if (request_path == "/") {
		// Look for exact root location match first
		std::cout << "it is a default location\n";
		std::vector<Location*>::iterator begLo = current_server.getLocationsList().begin();
		std::vector<Location*>::iterator endLo = current_server.getLocationsList().end();
		for (; begLo != endLo; ++begLo) {
			if ((*begLo)->getName() == "/") {
				std::cout << "found begLo: " << (*begLo)->getName() << " - " << (*begLo)->getRoot() << std::endl;
				return *begLo;
			}
		}
		std::cout << "is returning the default location\n";
		return NULL; // or return default location if you prefer
	}
	Location *best_location_name = NULL;
	size_t name_length_track = 0;
	std::vector<Location*>::iterator begLo = current_server.getLocationsList().begin();
	std::vector<Location*>::iterator endLo = current_server.getLocationsList().end();
	for (; begLo != endLo; ++begLo) {
		std::string location_path = (*begLo)->getRoot();
		std::cout << "Request: " << request_path << ", server location: " << location_path << std::endl;
		// Check if request path starts with location path (prefix matching)
		if (request_path.length() >= location_path.length() && 
			request_path.substr(0, location_path.length()) == location_path) {
			std::cout << "Prefix match found: " << request_path.substr(0, location_path.length()) << std::endl;
			// Additional check: ensure we match complete path segments
			// /kapouet should match /kapouet/file but not /kapouetfile
			bool isValidMatch = false;
			if (location_path.length() == request_path.length()) {
				// Exact match
				isValidMatch = true;
			} else if (location_path[-1] == '/') {
				std::cout << "is seg fault there\n";
				isValidMatch = true;
			} else if (request_path[location_path.length()] == '/') {
				// Next character in request is '/', valid path segment boundary
				isValidMatch = true;
			}
			if (isValidMatch) {
				// Keep track of longest matching location (most specific)
				if (location_path.length() > name_length_track) {
					best_location_name = *begLo;
					name_length_track = location_path.length();
					std::cout << "New best match: " << best_location_name->getName() << " -> " << best_location_name->getRoot() << std::endl;
				}
			}
		}
	}
	// Check if we found a match before accessing it
	if (best_location_name != NULL) {
		std::cout << "Final best match: " << best_location_name->getName() << " -> " << best_location_name->getRoot() << std::endl;
		return best_location_name;
	}
	// If no specific location matched, return default (first location or NULL)
	std::cout << "No location match found, using default" << std::endl;
	return current_server.getLocationsList().empty() ? NULL : current_server.getLocationsList().front();
}

std::string getFilenameFromPath(const std::string& path) {
	std::string::size_type slash_pos = path.rfind('/');
	if (slash_pos != std::string::npos) {
		return path.substr(slash_pos + 1);
	} else {
		return path;
	}
}

size_t maxBodySizeLocation(Server *default_server, Server *server_requested, Location *location_requested) {
	if (location_requested) {
		if (location_requested->getMaxSize())
			return location_requested->getMaxSize();
	} else {
		if (server_requested->getMaxBodySize())
			return server_requested->getMaxBodySize();
		else if (default_server->getMaxBodySize())
			return default_server->getMaxBodySize();
	}
	return 0;
}

std::string trimString(const std::string& s){
	size_t start = s.find_first_not_of(" \t\r\n");
	size_t end = s.find_last_not_of(" \t\r\n");
	return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);	
}

