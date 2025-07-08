/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executeScript.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 16:27:11 by ssottori          #+#    #+#             */
/*   Updated: 2025/07/08 22:40:21 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <cerrno>

#include "prepEnv.hpp"
#include "receiveRequest.hpp"
#include "receiveRequest.hpp"
#include "../../INC/utils/ServerManager.hpp"
#include "../../INC/utils/Utils.hpp"

class ScriptExecutor
{
	private:
		const std::string &_scriptPath;
		const RequestData &_request;
		std::string _response;
		int _pipe[2];

		char** createArgv() const;
		std::string getInterpreter() const;
		void runChild(std::string &body_filename);
		std::string runParent(pid_t pid);
		void bodytoStdin(); //POST only
		void execveScript(std::string &body_filename);
		bool createOutPipe();
		std::string errorResponse();

	public:
		ScriptExecutor(const std::string &scriptPath, const RequestData &request, ServerManager *master_server);
		~ScriptExecutor();

		std::string runScript(std::string &body_filename);
		ServerManager *_master_server;
};
