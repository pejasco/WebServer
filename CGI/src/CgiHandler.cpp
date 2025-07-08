/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 20:48:58 by ssottori          #+#    #+#             */
/*   Updated: 2025/07/08 13:48:05 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"
#include "../../INC/utils/Debug.hpp"
#include "../../INC/utils/ServerManager.hpp"
#include "../../INC/utils/Colors.hpp"

CgiHandler::CgiHandler(const RequestData &request, const std::string &scriptPath, ServerManager *master_server)
	: _request(request), _scriptPath(scriptPath) {
		run(master_server);
	}

std::string CgiHandler::run(ServerManager *master_server) {
	// DEBUG_PRINT(BOLD UNDERLINE BG_BLUE WHITE "CGI HANDLER CALLED" RESET);
	ScriptExecutor executor(_scriptPath, _request, master_server);
	return executor.runScript();
}
