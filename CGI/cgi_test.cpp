/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_test.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssottori <ssottori@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 20:57:59 by ssottori          #+#    #+#             */
/*   Updated: 2025/06/04 20:34:20 by ssottori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "inc/executeScript.hpp"
// #include "inc/receiveRequest.hpp"
// #include "inc/prepEnv.hpp"
// #include <iostream>

// void testEnvMap(std::map<std::string, std::string> headers)
// {
// 	//testing CGI environment variable map
// 	//RequestData request("POST", "./www/cgi-bin/totally_not_a_virus.py", "download=yes", headers, "screaming=internally&chaos=100%");
// 	RequestData request("GET", "./www/cgi-bin/totally_not_a_virus.py", "download=yes", headers, "screaming=internally&chaos=100%");

// 	EnvBuilder envBuilder(request);
// 	char** envp = envBuilder.buildEnvArray();

// 	std::cout << "== CGI ENVIRONMENT VARIABLES ==" << std::endl;
// 	for (size_t i = 0; envp[i]; ++i)
// 		std::cout << envp[i] << std::endl;

// 	envBuilder.freeEnvArray(envp);
// }

// void testingExecutor(std::map<std::string, std::string> headers)
// {
// 	RequestData request("GET", "./cgi-bin/test.py", "", headers, "");
// 	//RequestData request("POST", "./cgi-bin/test.py", "", headers, "birthday=13-11");
// 	ScriptExecutor executor("./cgi-bin/test.py", request);

// 	std::string output = executor.runScript();
// 	std::cout << "[CGI OUTPUT] ===\n" << output << std::endl;
// }

// int main() {
// 	std::map<std::string, std::string> headers;
// 	headers["Content-Type"] = "application/testing-cgi-modules";
// 	headers["Content-Length"] = "42";

// 	//testEnvMap(headers);
// 	testingExecutor(headers);

// 	return 0;
// }

#include "inc/executeScript.hpp"
#include "inc/receiveRequest.hpp"
#include "inc/prepEnv.hpp"
#include <iostream>

void testBirthdayScriptGET(std::map<std::string, std::string> headers)
{
	RequestData request("GET", "./cgi-bin/birthday.py", "day=13&month=11", headers, "");
	ScriptExecutor executor("./cgi-bin/birthday.py", request);

	std::string output = executor.runScript();
	std::cout << "[CGI OUTPUT] ===\n" << output << std::endl;
}

void testBirthdayScriptPOST(std::map<std::string, std::string> headers)
{
	RequestData request("POST", "./cgi-bin/birthday.py", "", headers, "day=13&month=11");
	ScriptExecutor executor("./cgi-bin/birthday.py", request);

	std::string output = executor.runScript();
	std::cout << "[CGI OUTPUT] ===\n" << output << std::endl;
}

// int main() {
// 	std::map<std::string, std::string> headers;
// 	headers["Content-Type"] = "application/x-www-form-urlencoded";
// 	headers["Content-Length"] = "17"; // strlen("day=05&month=06")

// 	testBirthdayScriptGET(headers);
// 	//testBirthdayScriptPOST(headers);

// 	return 0;
// }
