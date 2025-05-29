/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chuleung <chuleung@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 10:19:29 by cofische          #+#    #+#             */
/*   Updated: 2025/05/29 13:42:53 by chuleung         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSER_HPP
#define WEBSER_HPP

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

#include <sys/stat.h> //for checking the stat of a direct (used for post upload)

#endif