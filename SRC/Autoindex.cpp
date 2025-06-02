/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Autoindex.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cofische <cofische@student.42london.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 09:19:15 by cofische          #+#    #+#             */
/*   Updated: 2025/06/02 12:19:52 by cofische         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <cstring>


	// DIR *dir = opendir("/var/www/tools/");
	// struct dirent *entry;

	// while ((entry = readdir(dir)) != NULL) {
	// 	struct stat file_stat;
	// 	stat(entry->d_name, &file_stat);
		
		// Collect file info:
		// - filename
		// - size (file_stat.st_size)
		// - modification time (file_stat.st_mtime)
		// - type (directory vs file)
	// }
	// closedir(dir);


// function to create the HTML index structure
static int addDirectories(std::ofstream &HTML, const std::string &dir_path, const std::string &sub_dir) {
	DIR *dir = NULL;
	struct dirent *entry = NULL;
	(void)sub_dir;
	
	dir = opendir((dir_path).c_str());
	if (dir == NULL) {
		std::cout << "Error in addDir: " << strerror(errno) << std::endl;
		closedir(dir);
		return 404;
	}
	while ((entry = readdir(dir)) != NULL) {
		/*NEED IT IF IT IS A SECOND CLICK TO DIRECTORY*/
		// if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) // change for a c++ function 
		// 	continue;
		/*NEED IT IF IT IS A SECOND CLICK TO DIRECTORY*/
		if (entry->d_type == DT_DIR) {
			HTML << "\t\t<tr>\n";
			HTML << "\t\t\t<td><a href=\"./" << entry->d_name << "/\">"<< entry->d_name << "</a></td>\n";
			HTML << "\t\t\t<td>--</td>\n";
			HTML << "\t\t\t<td>--</td>\n";
			HTML << "\t</tr>\n";	 
		}
	}
	closedir(dir);
	return 200;
}

static int addFiles(std::ofstream &HTML, const std::string &dir_path) {
	DIR *dir = NULL;
	struct dirent *entry = NULL;
	
	dir = opendir((dir_path).c_str());
	if (dir == NULL) {
		std::cout << "Error in addFiles: " << strerror(errno) << std::endl;
		closedir(dir);
		return 404;
	}
	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_type == DT_REG || entry->d_type == DT_UNKNOWN) {
			std::string full_path = dir_path + '/' + entry->d_name;
			struct stat file_info;

			if (stat(full_path.c_str(), &file_info) == 0) {
				HTML << "\t\t<tr>\n";
				HTML << "\t\t\t<td>" << entry->d_name << "</td>\n";
				HTML << "\t\t\t<td>" << ctime(&file_info.st_mtime) << "</td>\n";
				HTML << "\t\t\t<td>" << file_info.st_size << "</td>\n";
				HTML << "\t</tr>\n";	
			}
		}
	}
	closedir(dir);
	return 200;
}

// Function to prepare the HTML file
static void setupHTML(std::ofstream &HTML, const std::string &sub_dir) {
	HTML << "<!DOCTYPE html>\n";
	HTML << "<html lang=\"en\">\n";
	HTML << "<head>\n";
	HTML << "\t<meta charset=\"UTF-8\">\n";
	HTML << "\t<meta name=\"index\" content=\"width=device-width, initial-scale=1.0\">\n";
	HTML << "\t<title>Index of " << sub_dir << "</title>\n";
	HTML << "\t<base href=\"" << sub_dir << "\">\n";
	HTML << "</head>\n";
	HTML << "<body>\n";
	HTML << "\t<h1>Index of " << sub_dir << "</h1>\n";
	HTML << "\t<table>\n";
	HTML << "\t\t<tr>\n";
	HTML << "\t\t\t<th>Name</th>\n";
	HTML << "\t\t\t<th>Last Modified</th>\n";
	HTML << "\t\t\t<th>Size</th>\n";
	HTML << "\t</tr>\n";	
}

static void endSetupHTML(std::ofstream &HTML) {
	HTML << "\t</table>\n";
	HTML << "</body>\n";
	HTML << "</html>\n";		
}
// function to fill in the structure with index info 
int structureInfo(const std::string &dir_path, const std::string &sub_dir, const std::string &default_folder) {
	//default folder minus the '// in front
	
	std::cout << "location of HTML file: " << (default_folder) << std::endl;
	// start to write the HTML file 
	std::ofstream HTML((default_folder + "/auto_index.html").c_str());
	int status_code = 0;
	if (HTML.is_open()) {
		setupHTML(HTML, sub_dir);
		if ((status_code = addDirectories(HTML, dir_path, sub_dir)) != 200) {
			HTML.close();
			return status_code;
		}	
		if ((status_code = addFiles(HTML, dir_path)) != 200) {
			HTML.close();
			return status_code;
		}
		endSetupHTML(HTML);
		HTML.close();
	} else {
		std::cerr << "Error in structureInfo: " << strerror(errno) << std::endl;
		return 500;
	}	
	std::cout << "\nLEAVING DIRECTORY LOOKUP\n";
	return 200;
};



// function to return it as body_filename for send back response

// function to remove the file once sent ? 