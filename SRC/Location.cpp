/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 11:37:06 by cofische          #+#    #+#             */
/*   Updated: 2025/04/24 17:06:41 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/utils/Location.hpp"

Location::Location() {};
Location::~Location() {};

//Setter
void Location::setRoot(const std::string &inputRoot) {
	root = inputRoot;
};
void Location::setMethod(const MET inputMethod) {
	method = inputMethod;
};
void Location::setIndex(const std::string &inputIndex) {
	index = inputIndex;
};
void Location::setAutoIndex(bool inputAIndex) {
	autoindex = inputAIndex;
};

//getter
std::string &Location::getRoot() {
	return root;
};
MET Location::getMethod() { //need to change for a vector as several mathod can exist
	return method;
};
std::string &Location::getIndex() {
	return index;
};
bool Location::getAutoIndex() {
	return autoindex;
};