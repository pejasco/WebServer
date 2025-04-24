/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Errors.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 15:27:30 by cofische          #+#    #+#             */
/*   Updated: 2025/04/24 16:28:08 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORS_HPP
#define ERRORS_HPP

#include "Webserv.hpp"

class Errors {
	public:
		Errors(int inputErrCode, const std::string &inputDir);
		~Errors();

		std::string &getDirectory();
		int getErrorCode();
		
	private:
		std::string directory;
		int error_code;
		
};

std::ostream &operator<<(std::ostream &os, Errors &error_page);

#endif