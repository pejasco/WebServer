/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 20:49:03 by ssottori          #+#    #+#             */
/*   Updated: 2025/07/08 22:35:29 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include "receiveRequest.hpp"
#include "executeScript.hpp"

class CgiHandler {
	private:
		const RequestData &_request;
		const std::string &_scriptPath;
		std::string &_body_filename;

	public:
		CgiHandler(const RequestData &request, const std::string &scriptPath, ServerManager *master_server, std::string &body_filename);
		std::string run(ServerManager *master_server);
};
