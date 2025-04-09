/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 15:48:24 by cofische          #+#    #+#             */
/*   Updated: 2025/04/09 10:59:40 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/Webserv.hpp"
#include "Parsing.cpp"

int main(int ac, char **av) {
	if (ac > 2) {
		std::cerr << "ERROR\n";
		return (-1);
	}
	std::string filename = av[1];
	readFile(filename);
	return 0;
}