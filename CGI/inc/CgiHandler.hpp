/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 20:49:03 by ssottori          #+#    #+#             */
/*   Updated: 2025/07/08 13:48:00 by cofische         ###   ########.fr       */
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

	public:
		CgiHandler(const RequestData &request, const std::string &scriptPath, ServerManager *master_server);
		std::string run(ServerManager *master_server);
};
