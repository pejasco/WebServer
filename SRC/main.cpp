/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 15:48:24 by cofische          #+#    #+#             */
/*   Updated: 2025/07/08 16:26:23 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/MasterHeader.hpp"

volatile std::sig_atomic_t g_sigStatus = 0;

void signal_handler(int signal) {
	DEBUG_PRINT("calling ctrl+c here");
	g_sigStatus = signal;
}

int main(int ac, char **av) {
	if (ac > 2) {
		std::cerr << BOLD RED "Error: invalid number of arguments\n" RESET;
		return (-1);
	}
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
    ServerManager* master_server = NULL;
	std::string config_file;
	try {
		if (ac > 1) {
			master_server = new ServerManager(av[1]);
		} else {
			master_server = new ServerManager("configuration/default.conf");
		}
    } catch (const std::exception& ex){
        std::cerr << "Error: " << ex.what() << std::endl;
		delete master_server;
        return EXIT_FAILURE;
    }
	master_server->serverMonitoring();
	if (g_sigStatus) {
		master_server->setRunning(g_sigStatus);
		delete master_server;
		return 0;
	}
	master_server->shutdown();
	delete master_server;
	return 0;
}
