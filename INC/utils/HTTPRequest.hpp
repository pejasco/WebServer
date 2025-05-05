/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chuleung <chuleung@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:18:48 by chuleung          #+#    #+#             */
/*   Updated: 2025/05/05 13:07:57 by chuleung         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ServerManager.hpp"
#include "HTTPResponse.hpp"

//**define in Server
// enum MET {
// 	GET,
// 	POST,
// 	DELETE
// };

// METHOD PATH HTTP/VERSION
// Header1: Value1
// Header2: Value2
// ...
// Host: example.com

// [optional body data]


class HTTPRequest {
	public:
		HTTPRequest();
		~HTTPRequest();
	
	private:
		MET method;
		










};
