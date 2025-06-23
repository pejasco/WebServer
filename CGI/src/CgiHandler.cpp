/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 20:48:58 by ssottori          #+#    #+#             */
/*   Updated: 2025/06/23 09:41:43 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"
#include "../../INC/utils/Debug.hpp"
#include "../../INC/utils/Colors.hpp"

CgiHandler::CgiHandler(const RequestData &request, const std::string &scriptPath)
	: _request(request), _scriptPath(scriptPath) {}

std::string CgiHandler::run() {
	// DEBUG_PRINT(BOLD UNDERLINE BG_BLUE WHITE "CGI HANDLER CALLED" RESET);
	ScriptExecutor executor(_scriptPath, _request);
	return executor.runScript();
}
