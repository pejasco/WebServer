/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Content.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chuleung <chuleung@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 15:01:27 by chuleung          #+#    #+#             */
/*   Updated: 2025/05/05 20:26:58 by chuleung         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ServerManager.hpp"

class Content {
        public:
                Content();
                ~Content();


        private:
                std::string content_type_;
                /*
        *       **Text and WebContent
        *       text/plain
        *       text/html
        *       text/css
        *       text/javascript
        *       application/javascript
        *
        *      **Form Submissions
        *      application/x-ww-form-urlenchoded (Default form submission like name=alice&age=20)
        *      multipart/form-data (Forms with file uploads (uses boundaries)) (it may also include parsing multipart/form-data in POST request and handling uploads of .txt, .csv; binary files: .zip, .pdf, .png), etc and storing them appropriately on the server.
        *       application/json 
        *      
        *      **Files and Media
        *      application/pdf
        *      image/jepg
        *      image/png
        *      audio/mpeg
        *      video/mp4
        *      
        *       */
                std::string boundary;



};































