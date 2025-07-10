/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prepEnv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 17:49:28 by ssottori          #+#    #+#             */
/*   Updated: 2025/07/10 09:44:42 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../INC/CGI/prepEnv.hpp"
#include "../../INC/utils/Utils.hpp"
#include "../../INC/utils/Debug.hpp"
#include "../../INC/utils/Colors.hpp"

EnvBuilder::EnvBuilder() {}

EnvBuilder::EnvBuilder(const RequestData& request) { initFromRequest(request); }

EnvBuilder::EnvBuilder(const EnvBuilder& other) { _env = other._env; }

EnvBuilder& EnvBuilder::operator=(const EnvBuilder& other)
{
	if (this != &other)
		_env = other._env;
	return *this;
}

EnvBuilder::~EnvBuilder() {}

void EnvBuilder::initFromRequest(const RequestData& request) //building map of cgi env vars using RequestData
{
	_env["REQUEST_METHOD"] = request.getMethod();
	_env["SCRIPT_FILENAME"] = request.getPath();
	_env["SCRIPT_NAME"] = request.getPath();
	_env["QUERY_STRING"] = request.getQueryString();
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["GATEWAY_INTERFACES"] = "CGI/1.1"; //expected constants according to the CGI/1.1 spec

	//handle headers
	std::map<std::string, std::string> headers = request.getHeaders();
	if (headers.find("Content-Type") != headers.end()) //cgI requires CONTENT_TYPE if the request has a body
		_env["CONTENT_TYPE"] = headers["Content-Type"];
 	std::string body = request.getBody();
    if (!body.empty()) {
        _env["CONTENT_LENGTH"] = convertToStr(body.length());
        // Set default content type if not provided
        if (headers.find("Content-Type") == headers.end()) {
            _env["CONTENT_TYPE"] = "application/x-www-form-urlencoded";
        }
    } else {
        _env["CONTENT_LENGTH"] = "0";
    }

	// DEBUG_PRINT("=== ENVIRONMENT VARIABLES ===");
	for (std::map<std::string, std::string>::const_iterator it = _env.begin(); it != _env.end(); ++it) {
		// DEBUG_PRINT("-" << it->first << "=" << it->second);
	}
	// DEBUG_PRINT("==============================");

}

std::map<std::string, std::string> EnvBuilder::getEnvMap() const { return _env; }

char** EnvBuilder::buildEnvArray() const // converting map into char array for execve()
{
	char** envp = new char*[_env.size() + 1];
	size_t i = 0;

//thos allows us to lpp thru every KEY=value pair in the _env map
	for (std::map<std::string, std::string>::const_iterator it = _env.begin(); it != _env.end(); ++it)
	{
		std::string entry = it->first + "=" + it->second;
		envp[i] = new char[entry.length() + 1];
		std::strcpy(envp[i], entry.c_str());
		i++;
	}
	envp[i] = NULL;
	return envp; //return the prepped envp array that execve is going to use in cgi script executor
}

void EnvBuilder::freeEnvArray(char** env) const
{
	for (size_t i = 0; env[i]; ++i)
		delete[] env[i];
	delete[] env;
}

//C++ strings (std::string) are higher-level objects.
//But POSIX C functions (like execve and pipe or fork)expect raw char* — so c_str() is the safe bridge between them
