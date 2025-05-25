/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 15:48:24 by cofische          #+#    #+#             */
/*   Updated: 2025/05/07 16:02:40 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/MasterHeader.hpp"

volatile std::sig_atomic_t g_sigStatus = 0;

void signal_handler(int signal) {
	g_sigStatus = signal;
}

int main(int ac, char **av) {
	if (ac > 2) {
		std::cerr << "ERROR\n";
		return (-1);
	}
	std::string filename;
	if (ac > 1) {
		filename = av[1];
	} else
		filename = "configuration/default.conf";
	//Start catching signal for clean shutdown
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
	//Start the ServerManager Object that will handle all the HTTP connections
	// if (g_sigStatus != SIGINT || g_sigStatus != SIGTERM)
	ServerManager masterServer(filename);
	masterServer.serverMonitoring();
	if (g_sigStatus)
		masterServer.setRunning(g_sigStatus);
	masterServer.shutdown();

	return 0;
}

/**************************/
/********DEBUGGING*********/
/**************************/

// #include <string>
// #include <iostream>
// #include <sstream>

// int convertInt(const std::string &str) {
// 	int nb;
// 	std::istringstream (str) >> nb;
// 	//if no error
// 	return nb;
// }

// int main() {
// 	size_t pos = 0;
// 	std::string request = "POST /api/users HTTP/1.1\r\nHost: example.com\r\nContent-Type: application/json\r\nContent-Length: 44\r\n\r\n{\"name\":\"John Doe\",\"email\":\"john@example.com\"}";
// 	std::string temp_body_message;
// 	int body_size; 
// 	if ((pos = request.find("Content-Length: ")) != std::string::npos) {
// 		body_size = convertInt(request.substr(pos + 16));
// 		std::cout << "body_size: " << body_size << std::endl;
// 	}
// 	if ((pos = request.find("\r\n\r\n")) != std::string::npos) {
// 		temp_body_message = request.substr(pos + 4);
// 		std::cout << "temp: " << temp_body_message << std::endl;
// 	}
// 	else 
// 		std::cout << "not found\n" << request << std::endl;
	
// 	if (temp_body_message.size() < body_size)
// 		std::cout << "not completed\n" << temp_body_message.size() << std::endl << "45: " << temp_body_message[45];
// 	else
// 		std::cout << "completed\n";
// }