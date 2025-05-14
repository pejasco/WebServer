/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 15:11:28 by cofische          #+#    #+#             */
/*   Updated: 2025/05/14 17:20:20 by cofische         ###   ########.fr       */
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
std::string convertStr(int nb);
std::string getStatusStr(int status_code);

#endif