/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 10:19:29 by cofische          #+#    #+#             */
/*   Updated: 2025/06/03 08:52:43 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSER_HPP
#define WEBSER_HPP

#include <dirent.h>
#include <sys/stat.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <string>
#include <cctype>
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