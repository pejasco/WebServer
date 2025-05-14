/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 10:19:29 by cofische          #+#    #+#             */
/*   Updated: 2025/05/14 15:26:20 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSER_HPP
#define WEBSER_HPP

#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <csignal>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "MET.hpp"
#include "Colors.hpp"
#include "Utils.hpp"

#endif