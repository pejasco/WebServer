/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 15:48:24 by cofische          #+#    #+#             */
/*   Updated: 2025/06/17 10:09:56 by cofische         ###   ########.fr       */
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
	std::string config_file;
	if (ac > 1) {
		config_file = av[1];
	} else
		config_file = "configuration/default.conf";
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
	ServerManager master_server(config_file);
	master_server.serverMonitoring();
	if (g_sigStatus) {
		master_server.setRunning(g_sigStatus);
		return 0;
	}
	master_server.shutdown();
	return 0;
}
