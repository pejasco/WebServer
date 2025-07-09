/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executeScript.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssottori <ssottori@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 16:26:00 by ssottori          #+#    #+#             */
/*   Updated: 2025/07/09 20:30:27 by ssottori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executeScript.hpp"
#include "../../INC/utils/Debug.hpp"
#include "../../INC/utils/Colors.hpp"

ScriptExecutor::ScriptExecutor(const std::string &scriptPath, const RequestData &request, ServerManager *master_server) : _scriptPath(scriptPath), _request(request), _master_server(master_server) {}

ScriptExecutor::~ScriptExecutor() {}

std::string ScriptExecutor::runScript(std::string &body_filename, HTTPResponse *http)
{
	if (!createOutPipe())
		return errorResponse();
	pid_t pid = fork();
	if (pid < 0)
		return errorResponse();
	if (pid == 0)
		runChild(body_filename, http);
	else
		return runParent(pid);
	
	return errorResponse();
}

bool ScriptExecutor::createOutPipe()
{
	return pipe(_pipe) == 0;
}

char** ScriptExecutor::createArgv() const
{
	char **av = new char*[3];
	std::string interpreter = getInterpreter();
	av[0] = new char[interpreter.size() + 1];
	std::strcpy(av[0], interpreter.c_str());
	av[1] = new char[_scriptPath.size() + 1];
	std::strcpy(av[1], _scriptPath.c_str());
	av[2] = NULL;
	return av;
}

void ScriptExecutor::runChild(std::string &body_filename, HTTPResponse *http)
{
	if (_request.getMethod() == "POST") {
		// Create a pipe for sending POST body to stdin
		int postPipe[2];
		if (pipe(postPipe) == -1) {
			std::cerr << BOLD RED "Failed to create POST pipe\n" RESET;
			_request.cleanBodyStr();	
			body_filename.clear();
			std::string().swap(body_filename);
			if (http->getCurrentRequest()) {
				delete http->getCurrentRequest();
			}
			if (http) {
				delete http;
				http = NULL;
			}
			cleanShutdown(*_master_server);
			delete(_master_server);
			_exit(1);
		}
		pid_t grandchild = fork(); // for testing
		if (grandchild < 0) {
			std::cerr << BOLD RED "Failed to fork grandchild for CGI\n" RESET;
			_request.cleanBodyStr();	
			body_filename.clear();
			std::string().swap(body_filename);
			if (http->getCurrentRequest()) {
				delete http->getCurrentRequest();
			}
			if (http) {
				delete http;
				http = NULL;
			}
			cleanShutdown(*_master_server);
			delete(_master_server);
			_exit(1);
		}

		if (grandchild == 0) {
			// Grandchild: sets up stdin/stdout and runs script
			close(_pipe[0]);                    // stdout pipe read end
			dup2(_pipe[1], STDOUT_FILENO);      // CGI stdout
			close(_pipe[1]);

			close(postPipe[1]);                 // write end of stdin pipe
			dup2(postPipe[0], STDIN_FILENO);    // CGI stdin
			close(postPipe[0]);
			
			execveScript(body_filename, http);                     // run the script
			_request.cleanBodyStr();	
			body_filename.clear();
			std::string().swap(body_filename);
			if (http->getCurrentRequest()) {
				delete http->getCurrentRequest();
			}
			if (http) {
				delete http;
				http = NULL;
			}
			cleanShutdown(*_master_server);
			delete(_master_server);
			_exit(1);                           // only if execve fails
		} else {
			// Child: writes POST body into pipe, waits for grandchild
			close(postPipe[0]);
			std::string body = _request.getBody();
			write(postPipe[1], body.c_str(), body.size());
			close(postPipe[1]);
			waitpid(grandchild, NULL, 0);
			body.clear();
			std::string().swap(body);
			_request.cleanBodyStr();	
			body_filename.clear();
			std::string().swap(body_filename);
			if (http->getCurrentRequest()) {
				delete http->getCurrentRequest();
			}
			if (http) {
				delete http;
				http = NULL;
			}
			cleanShutdown(*_master_server);
			delete(_master_server);
			_exit(0);  // done
		}
	}
	else {
		// GET/HEAD: just hook up stdout
		dup2(_pipe[1], STDOUT_FILENO);
		close(_pipe[0]);
		close(_pipe[1]);
		execveScript(body_filename, http);
		_request.cleanBodyStr();	
		body_filename.clear();
		std::string().swap(body_filename);
		if (http->getCurrentRequest()) {
			delete http->getCurrentRequest();
		}
		if (http) {
			delete http;
			http = NULL;
		}
		cleanShutdown(*_master_server);
		delete(_master_server);
		_exit(1);
	}
}


// void ScriptExecutor::runChild()
// {
// 	dup2(_pipe[1], STDOUT_FILENO); // redirect stdout to pipe
// 	close(_pipe[0]);
// 	close(_pipe[1]);

// 	if (_request.getMethod() == "POST") {
// 		bodytoStdin(); // handles input + exec
// 	} else {
// 		execveScript();
// 	}

// 	std::cerr << "Execve failed\n";
// 	_exit(1);
// }

std::string ScriptExecutor::runParent(pid_t pid)
{
	close(_pipe[1]);

	int status;
	bool timeout = false;

	for (int i = 0; i < 50; ++i) // 100ms x 50 = 5 seconds
	{
		usleep(100000);
		if (waitpid(pid, &status, WNOHANG) != 0)
			break;
		if (i == 49) {
			kill(pid, SIGKILL); // timeout
			timeout = true;
			waitpid(pid, &status, 0); // ensure child is reaped
		}
	}

	if (timeout)
	{
		close(_pipe[0]); // always close
		return "HTTP/1.1 504 Gateway Timeout\r\n"
				"Content-Type: text/html\r\n\r\n"
				"<html><head><title>504 Gateway Timeout</title></head>"
				"<body><h1>504 Gateway Timeout</h1>"
				"<p>The CGI script took too long to respond.</p></body></html>";
	}

	char buff[4096];
	size_t bytesRead;
	while ((bytesRead = read(_pipe[0], buff, sizeof(buff) - 1)) > 0)
	{
		buff[bytesRead] = '\0';
		_response += buff;
	}

	close(_pipe[0]);
	waitpid(pid, NULL, 0);
	// DEBUG_PRINT(BOLD UNDERLINE BG_BLUE WHITE "CGI HANDLER CALLED" RESET);
	return _response;
}

void ScriptExecutor::execveScript(std::string &body_filename, HTTPResponse *http)
{
	EnvBuilder envBuilder(_request);
	char** envp = envBuilder.buildEnvArray();
	char** av = createArgv();
	// std::cerr << "ExecveScript run in HTTPResponse method -> Allocated HTTPResponse of size " << sizeof(HTTPResponse) << " at " << http << ", size of client response: " << sizeof(http) << std::endl;
	if (http->getCurrentRequest()) {
		delete http->getCurrentRequest();
	}
	if (http) {
		delete http;
		http = NULL;
	}
	cleanShutdown(*_master_server);
	delete (_master_server);
	execve(av[0], av, envp);
	std::cerr << BOLD RED "execve failed: " << strerror(errno) << RESET << std::endl;
	envBuilder.freeEnvArray(envp);
	for (int i = 0; av[i]; ++i)
		delete[] av[i];
	delete[] av;
	///might need to delete?? if i dinamically allocate av
	if (http->getCurrentRequest()) {
		delete http->getCurrentRequest();
	}
	if (http) {
		delete http;
		http = NULL;
	}
	_request.cleanBodyStr();
	body_filename.clear();
	std::string().swap(body_filename);
	cleanShutdown(*_master_server);
	delete (_master_server);
	_exit(1);
}

std::string ScriptExecutor::errorResponse()
{
	// DEBUG_PRINT(BOLD UNDERLINE BG_BLUE WHITE "CGI HANDLER CALLED" RESET);
	return "Status: 500 Internal Server Error\n\n";
}

std::string ScriptExecutor::getInterpreter() const
{
	size_t dot = _scriptPath.rfind('.');
	if (dot == std::string::npos)
		return "";

	std::string ext = _scriptPath.substr(dot);
	
	// DEBUG_PRINT(BOLD UNDERLINE BG_BLUE WHITE "CGI HANDLER EXITED" RESET);
	
	if (ext == ".py")
		return "/usr/bin/python3";
	if (ext == ".sh")
		return "/bin/bash";
	return "";
}

// void ScriptExecutor::bodytoStdin() // only for POST
// {
// 	int pfd[2];
// 	if (pipe(pfd) == -1)
// 	{
// 		std::cerr << "Failed to create pipe for stdin redirection.\n";
// 		_exit(1);
// 	}
// 	pid_t pid = fork();
// 	if (pid < 0)
// 	{
// 		std::cerr << "Fork failed in ---> bodytoStdin.\n";
// 		_exit(1);
// 	}
// 	if (pid == 0)
// 	{
// 		// Child: redirect stdin
// 		close(pfd[1]);
// 		dup2(pfd[0], STDIN_FILENO);
// 		close(pfd[0]);
// 		execveScript(); //running the scripppttt
// 		_exit(1);
// 	}
// 	else
// 	{
// 		// Parent: write body to child
// 		close(pfd[0]); // close read end
// 		std::string body = _request.getBody();
// 		write(pfd[1], body.c_str(), body.size());
// 		close(pfd[1]);
// 		//waitpid(pid, NULL, 0);
// 	}
// }
