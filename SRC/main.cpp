/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 15:48:24 by cofische          #+#    #+#             */
/*   Updated: 2025/06/04 14:10:57 by cofische         ###   ########.fr       */
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
	std::string config_file;
	if (ac > 1) {
		config_file = av[1];
	} else
		config_file = "configuration/default.conf";
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
	ServerManager master_server(config_file);
	master_server.serverMonitoring();
	if (g_sigStatus)
		master_server.setRunning(g_sigStatus);
	master_server.shutdown();
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