/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chuleung <chuleung@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 12:19:03 by chuleung          #+#    #+#             */
/*   Updated: 2025/05/05 13:13:54 by chuleung         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// // Construct response
// std::string response = "HTTP/1.1 200 OK\r\n";
// response += "Content-Type: text/html\r\n";
// response += "Content-Length: " + std::to_string(body.length()) + "\r\n";
// response += "\r\n";  // End of headers
// response += body;    // Response body

// // Send it over the socket
// send(client_fd, response.c_str(), response.length(), 0);