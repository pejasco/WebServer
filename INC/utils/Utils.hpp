/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 15:11:28 by cofische          #+#    #+#             */
/*   Updated: 2025/05/14 14:35:45 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "ServerManager.hpp"

class Server;
class Location;
class ServerManager;

int convertInt(const std::string &str);
void printServer(Server &server);
void printLocation(Location &location);
bool isMessageCompleted(const std::string &request);
void cleanShutdown(ServerManager &masterServer);
int checkFile(const std::string &Path);

#endif