/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Errors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 15:27:27 by cofische          #+#    #+#             */
/*   Updated: 2025/04/24 16:28:00 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/Errors.hpp"

Errors::Errors(int inputErrCode, const std::string &inputDir) : directory(inputDir), error_code(inputErrCode) {}
Errors::~Errors() {};

std::string &Errors::getDirectory() {
	return directory;
};

int Errors::getErrorCode() {
	return error_code;
};

std::ostream &operator<<(std::ostream &os, Errors &error_page) {
	os << error_page.getErrorCode();
	return os;
};