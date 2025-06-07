/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Content.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssottori <ssottori@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 15:01:27 by chuleung          #+#    #+#             */
/*   Updated: 2025/06/07 04:32:20 by ssottori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONTENT_HPP
#define CONTENT_HPP

#include "Webserv.hpp"
// POST /upload HTTP/1.1
// Host: example.com
// Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryXYZ
// Content-Length: 2050

// ------WebKitFormBoundaryXYZ
// Content-Disposition: form-data; name="file"; filename="example.txt"
// Content-Type: text/plain

// [Binary data of the file goes here]
// ------WebKitFormBoundaryXYZ--



// P OST /upload HTTP/1.1
// Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryXYZ

// ------WebKitFormBoundaryXYZ
// Content-Disposition: form-data; name="username"

// john_doe
// ------WebKitFormBoundaryXYZ
// Content-Disposition: form-data; name="file"; filename="example.txt"
// Content-Type: text/plain

// ...file contents...
// ------WebKitFormBoundaryXYZ--



struct ContentDisposition_
{
        ContentDisposition_();
        ~ContentDisposition_();

        void printHttpCD(); //new
        

        //variables (public by default)
        static int global_index_;
        int instance_index_;
        std::string CD_type_;
        std::string name_;
        std::string filename_;
        std::string inner_content_type_;
        std::string content_;
        std::string file_content_;
};

enum CD_header{
        ContentDisposition,
        InterContentType,
        Cont,
        FileContent
};

class Content {
        public:
                Content();
                ~Content();

                //setters
                void setContentType(const std::string& buffer);
                void setBoundary(const std::string& buffer);
                void setOpenBoundary(const std::string& boundary);
                void setCloseBoundary(const std::string& boundary);

                void setContentLength(const std::string& buffer);
                void setBodyWithNoCD(const std::string& buffer);
                void setCDs(const std::string& buffer, CD_header header, int index);
                void addContentDisposition(const ContentDisposition_& cd); //need to take a look
                void addCDs(); //new 

                //getters
                const std::pair<std::string, std::string>& getContentType();
                const std::string& getBoundary();
                const std::string& getOpenBoundary();
                const std::string& getCloseBoundary();
                int getContentLength();
                const std::map<std::string, std::string> getBody(); //no use
                std::vector<ContentDisposition_>& getCDs(); //modified
                std::string& getBodyWithNoCD(); //modified
                //const std::map<std::string, std::string> getBodyconst() const;
                void printCDsList() const;


        private:

                static int global_index_; //dun know if it is useful
		int instance_index_;
                std::pair<std::string, std::string> content_type_; //Content-Type: application/x-www-form-urlencoded (!!!this is outer content-type for the whole body)
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
                // bool body_flag_;
                // bool cd_flag_;
                int content_length_; //Content-Length: 13
                std::string boundary_; //Content-Type: multipart/form-dat; boundary=--WebKitFormBoundaryXYZ
                // std::map<std::string, std::string> body_;
                std::string open_boundary_; //new http_content
                std::string close_boundary_;//new http_content

                std::string body_with_no_cd_;
                std::vector<ContentDisposition_> CDs_list_;
};

#endif
