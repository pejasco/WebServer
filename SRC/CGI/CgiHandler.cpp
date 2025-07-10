/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 20:48:58 by ssottori          #+#    #+#             */
/*   Updated: 2025/07/10 09:44:54 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../INC/CGI/CgiHandler.hpp"
#include "../../INC/utils/Debug.hpp"
#include "../../INC/utils/ServerManager.hpp"
#include "../../INC/utils/Colors.hpp"

CgiHandler::CgiHandler(const RequestData &request, const std::string &scriptPath, ServerManager *master_server, std::string &body_filename, HTTPResponse *http)
	: _request(request), _scriptPath(scriptPath), _body_filename(body_filename) {
		run(master_server, http);
	}

std::string CgiHandler::run(ServerManager *master_server, HTTPResponse *http) {
	// DEBUG_PRINT(BOLD UNDERLINE BG_BLUE WHITE "CGI HANDLER CALLED" RESET);
	// std::cerr << "CGIHandler run in HTTPResponse method -> Allocated HTTPResponse of size " << sizeof(HTTPResponse) << " at " << http << ", size of client response: " << sizeof(http) << std::endl;
	ScriptExecutor executor(_scriptPath, _request, master_server);
	return executor.runScript(_body_filename, http);
}
