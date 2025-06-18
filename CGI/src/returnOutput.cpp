/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   returnOutput.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 22:08:37 by ssottori          #+#    #+#             */
/*   Updated: 2025/06/18 11:08:32 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "returnOutput.hpp"
#include "../../INC/utils/Debug.hpp"
#include "../../INC/utils/Colors.hpp"

void ReturnOutput::parseOutput()
{
	splitHeaderBody();
	parseHeaders();
}

void ReturnOutput::splitHeaderBody()
{
	size_t headerEnd = _rawOutput.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		headerEnd = _rawOutput.find("\n\n");
	
	if (headerEnd == std::string::npos)
		return;
	
	_headerSection = std::string(_rawOutput.begin(), _rawOutput.begin() + headerEnd);
	_body = std::string(_rawOutput.begin() + headerEnd + 3, _rawOutput.end());
}

void ReturnOutput::parseHeaders()
{
	std::stringstream stream(_headerSection);
	std::string line;

	while (std::getline(stream, line))
	{
		if (line.empty())
			continue;

		size_t colon = line.find(':');
		if (colon != std::string::npos)
		{
			std::string key = line.substr(0, colon);
			std::string val = line.substr(colon + 1);

			// ttrim any space after colon
			while (!val.empty() && val[0] == ' ')
				val.erase(0, 1);

			_headers[key] = val;
		}
	}
}

ReturnOutput::ReturnOutput(const std::string &output) : _rawOutput(output) {}

std::string ReturnOutput::getBody() const
{
	return _body;
}

std::map<std::string, std::string> ReturnOutput::getHeaders() const
{
	return _headers;
}
