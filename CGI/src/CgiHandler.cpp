/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssottori <ssottori@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 20:48:58 by ssottori          #+#    #+#             */
/*   Updated: 2025/05/28 02:15:43 by ssottori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

CgiHandler::CgiHandler(const RequestData &request, const std::string &scriptPath)
	: _request(request), _scriptPath(scriptPath) {}

std::string CgiHandler::run() {
	std::cerr << "[CGI] WE ARE IN CGI HANDLER: " << std::endl;
	ScriptExecutor executor(_scriptPath, _request);
	return executor.runScript();
}
