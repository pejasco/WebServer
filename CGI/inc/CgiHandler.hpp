/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssottori <ssottori@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 20:49:03 by ssottori          #+#    #+#             */
/*   Updated: 2025/05/23 18:18:02 by ssottori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include "receiveRequest.hpp"
#include "executeScript.hpp"

class CgiHandler {
	private:
		RequestData _request;
		std::string _scriptPath;

	public:
		CgiHandler(const RequestData &request, const std::string &scriptPath);
		std::string run();
};
