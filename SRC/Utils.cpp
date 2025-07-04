/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 16:24:47 by cofische          #+#    #+#             */
/*   Updated: 2025/07/04 12:41:06 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/ServerManager.hpp"
#include "../INC/utils/Utils.hpp"

std::string safeSubstrAfter(const std::string& str, size_t pos, size_t offset) {
    if (pos == std::string::npos || pos + offset >= str.size())
        return "";
    std::string result = str.substr(pos + offset);
    
    // Trim whitespace (restore this functionality!)
    size_t start = result.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";  // Only whitespace found
    }
    size_t end = result.find_last_not_of(" \t\n\r");
    return result.substr(start, end - start + 1);
}

// Helper for extracting values after colon with +2 offset
std::string extractValueAfterColon(const std::string& line, bool use_rfind) {
    size_t pos = 0;
	if (use_rfind)
		pos = line.rfind(":");
	else
		pos = line.find(":");
    return safeSubstrAfter(line, pos, 2);
}

void printLocation(Location &location) {
	/*BASIC INFO*/
	DEBUG_PRINT(BOLD MAGENTA "Location" RESET);
	DEBUG_PRINT(BOLD "Path: " RESET << location.getName());
	DEBUG_PRINT(BOLD "root: " RESET << location.getRoot());
	//method display
	if (!location.getMethod().empty()) {
		const std::vector<MET>& met = location.getMethod();
		std::vector<MET>::const_iterator itMet = met.begin();
		std::vector<MET>::const_iterator iteMet = met.end();
		for (; itMet != iteMet; ++ itMet) {
			if (*itMet == 0)
				DEBUG_PRINT(BOLD "methods: " RESET "GET");
			else if (*itMet == 1)
				DEBUG_PRINT(BOLD "methods: " RESET "POST");
			else if (*itMet == 2)
				DEBUG_PRINT(BOLD "methods: " RESET "DELETE");
			else
				DEBUG_PRINT(BOLD "methods: " RESET "UNKNOWN");
		}
	}
	
	DEBUG_PRINT(BOLD "index: " RESET << location.getIndex());
	DEBUG_PRINT(BOLD "directories listing: " RESET << location.isAutoIndex());
	
	/*UPLAOD INFO*/
	DEBUG_PRINT(BOLD "- is upload enable: " RESET << location.isUpload());
	DEBUG_PRINT(BOLD "upload storage folder: " RESET << location.getUploadDir());
	DEBUG_PRINT(BOLD "upload Max body size: " RESET << location.getMaxSize());

	/*CGI INFO*/
	DEBUG_PRINT(BOLD "- is CGI enable: " RESET << location.isCGI());
	//cgi extension list
	if (!location.getCGIExt().empty()) {
		const std::vector<std::string>& extensions = location.getCGIExt();  // Single call
		std::vector<std::string>::const_iterator itExt = extensions.begin();
		std::vector<std::string>::const_iterator iteExt = extensions.end();
		for (; itExt != iteExt; ++itExt) {
			DEBUG_PRINT(BOLD "extension: " RESET << *itExt);
		}
	}
	/*REDIRECTION INFO*/
	DEBUG_PRINT(BOLD "- is redirection enable: " RESET << location.getRedirect());
	DEBUG_PRINT(BOLD "redirection code: " RESET << location.getRedirectCode());
	DEBUG_PRINT(BOLD "redirection URL: " RESET << location.getRedirectURL());
	
}

void printServer(Server &server) {
	/*SERVER NAMES*/
	DEBUG_PRINT(BOLD YELLOW "SERVER" RESET);
	if (!server.getServerNames().empty()) {
		const std::vector<std::string>& names = server.getServerNames();  // Also use reference
		std::vector<std::string>::const_iterator it = names.begin();
		std::vector<std::string>::const_iterator ite = names.end();
		for (; it != ite; ++it)
			DEBUG_PRINT(BOLD "server name: " RESET << *it);  // Fixed: *it not *ite
	}
	/*PORT AND HOST*/
	DEBUG_PRINT(BOLD "server host: " RESET << server.getIP());
	if (!server.getPort().empty()) {
		const std::vector<std::string>& ports = server.getPort();
		std::vector<std::string>::const_iterator itp = ports.begin();
		std::vector<std::string>::const_iterator itep = ports.end();
		for (; itp != itep; ++itp)
			DEBUG_PRINT(BOLD "server ports: " RESET << *itp);
	}
	/*ERROR PAGES CODE LIST*/
	if (!server.getErrorList().empty()) {
		const std::map<int, std::string>& errors = server.getErrorList();
		std::map<int, std::string>::const_iterator itr = errors.begin();
		std::map<int, std::string>::const_iterator iter =	errors.end();
		for (; itr != iter; ++itr)
			DEBUG_PRINT(BOLD "error page: " RESET << itr->first << " " << itr->second);
	}
	/*MAX BODY SIZE*/
	DEBUG_PRINT(BOLD "server maxsize: " RESET << server.getMaxBodySize());
	DEBUG_PRINT(BOLD "server keep alive: " RESET << server.isKeepAlive());
	
	/*LOCATIONS BLOCKS*/
	if (!server.getLocationsList().empty()) {
		const std::vector<Location*>& loc = server.getLocationsList();
		std::vector<Location*>::const_iterator itL = loc.begin();
		std::vector<Location*>::const_iterator iteL = loc.end();
		for (; itL != iteL; ++itL) {
			printLocation(**itL);
			DEBUG_PRINT("");
		}
		DEBUG_PRINT("");
	}
}

bool isConfigFileOK(ServerManager &server_manager) {
	Server *temp_server = NULL;
	Location *temp_location = NULL;
	if (server_manager.getServers().empty())
		return false;
	else {
		temp_server = server_manager.getServers().front();
		if (temp_server->getIP().empty())
			return false;
		else if (temp_server->getPort().empty())
			return false;
		else if (temp_server->getLocationsList().empty())
			return false;
	}
	temp_location = temp_server->getLocationsList().front();
	if (temp_location->getMethod().empty())
		return false;
	else if (temp_location->getRoot().empty())
		return false;
	else if (temp_location->getIndex().empty())
		return false;
	else 
		return true;
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
	if (!master_server.getSockets().empty()) {
		std::vector<Socket*>::iterator begSo = master_server.getSockets().begin();
		std::vector<Socket*>::iterator endSo = master_server.getSockets().end();
		for (; begSo != endSo; ++begSo) {
			int fd = (*begSo)->getSocketFd();
    	    if (isFdOpen(fd)) {
    	        DEBUG_PRINT("Closing fd: " << fd);
    	    } else {
    	        DEBUG_PRINT("fd " << fd << " already closed or invalid");
    	    }
			if (*begSo != NULL)
				delete *begSo;
		}
	}
	//closing and deleting client info
	if (!master_server.getClients().empty()) {
		std::map<int,Client*>::iterator begCl = master_server.getClients().begin();
		std::map<int,Client*>::iterator endCl = master_server.getClients().end();
		for (; begCl != endCl; ++begCl) {
			int fd = begCl->first;
    	    if (isFdOpen(fd)) {
    	        DEBUG_PRINT("Closing fd: " << fd);
    	    } else {
    	        DEBUG_PRINT("fd " << fd << " already closed or invalid");
    	    }
			if ((begCl)->second) {
				if (begCl->second->current_request) {
					delete begCl->second->current_request;
					begCl->second->current_request = NULL;
				}
				if (begCl->second->current_response) {
					delete begCl->second->current_response;
					begCl->second->current_response = NULL;
				}
				delete begCl->second;
			}
		}
	}	
	//freeing the struct server
	if (!master_server.getServers().empty()) {
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
		if (master_server.getEpollFd() != -1)
			close(master_server.getEpollFd());
	}
}

bool isFdOpen(int fd) {
    return fcntl(fd, F_GETFD) != -1;
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
	status_messages.insert(std::make_pair(413, "Payload Too Large"));
	status_messages.insert(std::make_pair(415, "Unsupported Media Type"));
	status_messages.insert(std::make_pair(431, "Request Header Fields Too Large"));
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
    struct stat buffer;
    bool exists = (stat(filename.c_str(), &buffer) == 0);
    DEBUG_PRINT("Does file " << filename << " exist? " << exists);
    return exists;
}

int URILength(const std::string &uri) {
	if (uri.length() > MAX_URI_LENGTH) {
        DEBUG_PRINT("URI too long: " << uri.length() << " characters (max: " << MAX_URI_LENGTH << ")");
        return 414;
    }
	return 200;
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

std::string urlDecoder(std::string &url_string) {
	std::map<std::string, char> url_code;
	DEBUG_PRINT("ulrDecoder() called");
	if (url_code.empty()) {
		url_code["%20"] = ' ';   // Space (most important!)
        url_code["%21"] = '!';   // Exclamation mark
        url_code["%22"] = '"';   // Double quote
        url_code["%23"] = '#';   // Hash/pound
        url_code["%25"] = '%';   // Percent sign
        url_code["%26"] = '&';   // Ampersand
        url_code["%27"] = '\'';  // Single quote/apostrophe
        url_code["%28"] = '(';   // Left parenthesis
        url_code["%29"] = ')';   // Right parenthesis
        url_code["%2B"] = '+';   // Plus sign
        url_code["%2C"] = ',';   // Comma
        url_code["%2F"] = '/';   // Forward slash
        url_code["%3A"] = ':';   // Colon
        url_code["%3B"] = ';';   // Semicolon
        url_code["%3D"] = '=';   // Equal sign
        url_code["%3F"] = '?';   // Question mark
        url_code["%40"] = '@';   // At symbol
        url_code["%5B"] = '[';   // Left square bracket
        url_code["%5D"] = ']';   // Right square bracket
        url_code["%7B"] = '{';   // Left curly brace
        url_code["%7D"] = '}';   // Right curly brace
	}
	DEBUG_PRINT("url_code map -- checking size and 1st line: " BOLD << url_code.size() << RESET ", " << url_code.begin()->first);
	std::string ascii_string;
    for (size_t i = 0; i < url_string.length(); ++i) {
    	if (url_string[i] == '%' && i + 2 < url_string.length()) {
    	    std::string code = url_string.substr(i, 3);
    	    if (code.length() == 3) {
    	        code[1] = toupper(code[1]);
    	        code[2] = toupper(code[2]);
    	    }
    	    std::map<std::string, char>::const_iterator it = url_code.find(code);
    	    if (it != url_code.end()) {
    	        ascii_string += it->second;
    	        i += 2;
    	    } else
    	        ascii_string += url_string[i];
    	} else if (url_string[i] == '+')
    	    ascii_string += ' ';
    	else
    	    ascii_string += url_string[i];
    }
	DEBUG_PRINT("ascii_string: " << ascii_string);
	DEBUG_PRINT("ulrDecoder() exited");
    return ascii_string;
}

int checkExtensions(Location *current_location, std::string &script_name) {
	std::string extension_requested;
	size_t pos = 0;
	if ((pos = script_name.find(".")) != std::string::npos) {
		extension_requested = script_name.substr(pos, (script_name.size() - pos));
		DEBUG_PRINT("extension identify in script_name: " << extension_requested);
	}
	if (!current_location->getCGIExt().empty()) {
		std::vector<std::string>::iterator begEx = current_location->getCGIExt().begin();
		std::vector<std::string>::iterator endEx = current_location->getCGIExt().end();
		for (; begEx != endEx; ++ begEx) {
			if (*begEx == extension_requested) {
				DEBUG_PRINT("Extension requested is in the list of authorize extension");
				return 200;
			}
		}
		if (begEx == endEx) {
			DEBUG_PRINT("Extension requested not in the list -- return error code");
			return 405;
		}
	} else {
		DEBUG_PRINT("No extension available -- CGI wrong configuration or bad URI");
		return 405;
	}
	DEBUG_PRINT("Error in the function -- return 500");
	return 500;
}

std::string getDefautlErrorPage(int code) {
	switch (code) {
	case 400:
		return "HTTP/1.1 400 Bad Request\r\n\
				Content-Type: text/html; charset=UTF-8\r\nContent-Length: 148\r\nConnection: close\r\n\r\n\
				<!DOCTYPE html><html><head><title>400 Bad Request</title></head><body><h1>400 Bad Request\
				</h1><p>The request could not be understood due to malformed syntax.</p></body></html>";
	case 401:
		return "HTTP/1.1 401 Unauthorized\r\n\
				Content-Type: text/html; charset=UTF-8\r\nContent-Length: 154\r\nConnection: close\r\n\r\n\
				<!DOCTYPE html><html><head><title>401 Unauthorized</title></head><body><h1>401 Unauthorized\
				</h1><p>Authentication is required to access this resource.</p></body></html>";
	case 403:
		return "HTTP/1.1 403 Forbidden\r\n\
				Content-Type: text/html; charset=UTF-8\r\nContent-Length: 142\r\nConnection: close\r\n\r\n\
				<!DOCTYPE html><html><head><title>403 Forbidden</title></head><body><h1>403 Forbidden\
				</h1><p>You don't have permission to access this resource.</p></body></html>";	
	case 404:
		return "HTTP/1.1 404 Not Found\r\n\
				Content-Type: text/html; charset=UTF-8\r\nContent-Length: 134\r\nConnection: close\r\n\r\n\
				<!DOCTYPE html><html><head><title>404 Not Found</title></head><body><h1>404 Not Found\
				</h1><p>The requested resource could not be found.</p></body></html>";
	case 405:
		return "HTTP/1.1 405 Method Not Allowed\r\n\
				Content-Type: text/html; charset=UTF-8\r\nContent-Length: 159\r\nConnection: close\r\n\r\n\
				<!DOCTYPE html><html><head><title>405 Method Not Allowed</title></head><body><h1>405 Method Not Allowed\
				</h1><p>The request method is not allowed for this resource.</p></body></html>";
	case 413:
		return "HTTP/1.1 413 Payload Too Large\r\n\
				Content-Type: text/html; charset=UTF-8\r\nContent-Length: 152\r\nConnection: close\r\n\r\n\
				<!DOCTYPE html><html><head><title>413 Payload Too Large</title></head><body><h1>413 Payload Too Large\
				</h1><p>The request payload is too large to process.</p></body></html>";
	case 500:
		return "HTTP/1.1 500 Internal Server Error\r\n\
				Content-Type: text/html; charset=UTF-8\r\nContent-Length: 167\r\nConnection: close\r\n\r\n\
				<!DOCTYPE html><html><head><title>500 Error</title></head><body><h1>500 Internal Server Error\
				</h1><p>The server encountered an error and could not complete your request.</p></body></html>";
	default:
		return "";
	}
	
}

std::string getServerIPPort(int socket_fd) {
	struct sockaddr_in server_addr;
	socklen_t server_addr_len = sizeof(server_addr);
	
	// Get the server socket's address information
	if (getsockname(socket_fd, (struct sockaddr*)&server_addr, &server_addr_len) == 0) {
		char ipStr[INET_ADDRSTRLEN];
		if (inet_ntop(AF_INET, &server_addr.sin_addr, ipStr, INET_ADDRSTRLEN)) {
			int port = ntohs(server_addr.sin_port);
            std::string ip_port = ipStr;
			ip_port += ":" + convertToStr(port);
			return ip_port;
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
	std::ifstream body_file(filename.c_str(), std::ios::binary);
	if (body_file.is_open()) {
		pos = body_file.tellg();
		body_file.seekg(0, std::ios::end);
		pos = body_file.tellg() - pos;
		body_file.close();
	} else
		std::cerr << BOLD RED "Error in calculateFileSize(): " << strerror(errno) << RESET << std::endl;
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
    std::string host_header = input_request.getHost();
    std::string host;
    std::string request_port;
	
    DEBUG_PRINT("IP-Port server: " << server_IP);
    size_t colon_pos = server_IP.find(':');
    if (colon_pos != std::string::npos) {
        // Host header contains port (e.g., "localhost:8080")
        host = server_IP.substr(0, colon_pos);           // "localhost"
        request_port = server_IP.substr(colon_pos + 1);
	}
	colon_pos = host_header.find(':');
	if (colon_pos != std::string::npos) {
        // Host header contains port (e.g., "localhost:8080")
        host_header = host_header.substr(0, colon_pos);           // "localhost"
	}
	DEBUG_PRINT("host_header: " << host_header << ", host: " << host << ", request_port: " << request_port);
    // Step 1: Collect all servers that match IP and port
    std::vector<Server*> matching_servers;
    std::vector<Server*>::iterator begSe = server_manager.getServers().begin();
    std::vector<Server*>::iterator endSe = server_manager.getServers().end();
    
    for (; begSe != endSe; ++begSe) {
        if ((*begSe)->getIP() == host) {
            DEBUG_PRINT("Checking server IP: " << (*begSe)->getIP());
            
            std::vector<std::string> server_ports = (*begSe)->getPort();
            std::vector<std::string>::iterator begPo = server_ports.begin();
            std::vector<std::string>::iterator endPo = server_ports.end();
            
            for (; begPo != endPo; ++begPo) {
                DEBUG_PRINT("Checking port: " << *begPo << " against request port: " << request_port);
                if (request_port == *begPo) {
                    DEBUG_PRINT("Found matching IP:port - adding to candidates");
                    matching_servers.push_back(*begSe);
                    break; // Found matching port for this server, no need to check other ports
                }
            }
        }
    }
    
    // If no servers match IP:port, return the first server as fallback
    if (matching_servers.empty()) {
        DEBUG_PRINT("No matching servers found, returning first server");
        return server_manager.getServers().front();
    }
    
    // Step 2: Among matching IP:port servers, find one with matching server_name
	std::vector<Server*>::iterator matching_it = matching_servers.begin();
    std::vector<Server*>::iterator matching_end = matching_servers.end();
    
    for (; matching_it != matching_end; ++matching_it) {
        // Get server names for this server (you'll need to implement getServerNames())
        if (!(*matching_it)->getServerNames().empty()) {
			std::vector<std::string> server_names = (*matching_it)->getServerNames();
        	std::vector<std::string>::iterator name_it = server_names.begin();
        	std::vector<std::string>::iterator name_end = server_names.end();
	
        	for (; name_it != name_end; ++name_it) {
        	    DEBUG_PRINT("Checking server_name: " << *name_it << " against hostname: " << host_header);
        	    if (*name_it == host_header) {
        	        DEBUG_PRINT("Found exact server_name match!");
        	        return *matching_it;
        	    }
        	}
		}
    }
    
    // Step 3: No server_name match found, return FIRST server for this IP:port (default behavior)
    DEBUG_PRINT("No server_name match, using default (first) server for " << server_IP << ":" << request_port);
    return matching_servers.front();
}

Location *getCurrentLocation(const HTTPRequest &input_request, Server &current_server) {
	DEBUG_PRINT("is locationList empty? " << (current_server.getLocationsList().empty() ? "is empty" : "is not empty"));
		 
	std::string request_path = input_request.getPath();
	DEBUG_PRINT("URL to look for: " << request_path);
	// Handle root request specially if needed
	if (request_path == "/") {
		// Look for exact root location match first
		// DEBUG_PRINT("it is a default location\n";
		return current_server.getLocationsList().front();
		// DEBUG_PRINT("is returning the default location\n";
	}
	Location *best_location_name = NULL;
	size_t name_length_track = 0;
	bool isValidMatch = false;
	std::vector<Location*>::iterator begLo = current_server.getLocationsList().begin();
	std::vector<Location*>::iterator endLo = current_server.getLocationsList().end();
	for (; begLo != endLo; ++begLo) {
		std::string location_path = (*begLo)->getName();
		DEBUG_PRINT("Request: " << request_path << ", server location: " << location_path);
		DEBUG_PRINT("Request size: " << request_path.size() << ", server location size: " << location_path.size());
		// Check if request path starts with location path (prefix matching)
		DEBUG_PRINT("Prefix to compare: " << request_path.substr(0, location_path.length()) << ", size: " << request_path.substr(0, location_path.length()).size());
		DEBUG_PRINT("Check char by char the location string: ");
		for (size_t i = 0; i < location_path.length(); ++i) {
			DEBUG_PRINT("[" << i << "] '" << location_path[i] << "' ");
		}
		DEBUG_PRINT("Check char by char the request string: ");
		for (size_t i = 0; i < (request_path.substr(0, location_path.length())).length(); ++i) {
			DEBUG_PRINT("[" << i << "] '" << (request_path.substr(0, location_path.length()))[i] << "' ");
		}
		if (request_path.length() >= location_path.length() && 
			request_path.substr(0, location_path.length()) == location_path) {
			DEBUG_PRINT("Prefix match found: " << request_path.substr(0, location_path.length()));
			if (location_path.length() == request_path.substr(0, location_path.length()).length()) {
				// Exact match
				isValidMatch = true;
			} else if (location_path[location_path.length() - 1] == '/') {
				isValidMatch = true;
			}
			if (isValidMatch) {
				// Keep track of longest matching location (most specific)
				if (location_path.length() > name_length_track) {
					best_location_name = *begLo;
					name_length_track = location_path.length();
					DEBUG_PRINT("New best match: " << best_location_name->getName() << " -> " << best_location_name->getRoot());
				}
			}
		}
	}
	// Check if we found a match before accessing it
	if (best_location_name != NULL) {
		DEBUG_PRINT("Final best match: " << best_location_name->getName() << " -> " << best_location_name->getRoot());
		return best_location_name;
	}
	// If no specific location matched, return default (first location or NULL)
	DEBUG_PRINT("No location match found, using default");
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
		if (location_requested->getMaxSize() != 0)
			return location_requested->getMaxSize();
	} else {
		if (server_requested->getMaxBodySize() != 0)
			return server_requested->getMaxBodySize();
		else if (default_server->getMaxBodySize() != 0)
			return default_server->getMaxBodySize();
	}
	return 1048576;
}

std::string trimString(const std::string& s){
	size_t start = s.find_first_not_of(" \t\r\n");
	size_t end = s.find_last_not_of(" \t\r\n");
	return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);	
}

int fileDeletable(const std::string &body_filename, Location *location, Location *default_location) {
	DEBUG_PRINT("file to check before deleting: " << body_filename);
	DEBUG_PRINT("location requested initially: " << location->getName());
	DEBUG_PRINT("Root folder on location: " << location->getRoot());
	DEBUG_PRINT("default location: " << default_location->getName());
	// we accidentally arrive on the defualt server/default location, avoid deleting any delete file from this section
	if (location && default_location) {
		if (location->getName() == default_location->getName()) {
			DEBUG_PRINT("We are in the default location");
			std::vector<MET>::iterator begM = location->getMethod().begin();
			std::vector<MET>::iterator endM = location->getMethod().end();
			for (; begM != endM; ++begM) {
				if (*begM == DELETE) {
					if (getFilenameFromPath(body_filename) == default_location->getIndex()) {
						DEBUG_PRINT("Trying to delete a root file -- unauthorise: 403");
						return 403;
					}
					DEBUG_PRINT("File requested is in a DELETE environment: 200");
					return 200;
					
				}
				DEBUG_PRINT("File requested isn't in a DELETE environment: 405");
				return 405;
			}
		}
		if (body_filename.find(location->getName()) != std::string::npos)
			return 200;
	}
	DEBUG_PRINT("No default location saved -- returning error per security: 403");
	return 403;	
}
