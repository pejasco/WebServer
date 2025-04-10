/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 15:48:24 by cofische          #+#    #+#             */
/*   Updated: 2025/04/10 11:42:46 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/ServerManager.hpp"
#include "Parsing.cpp"

int main(int ac, char **av) {
	if (ac > 2) {
		std::cerr << "ERROR\n";
		return (-1);
	}
	return 0;
}