#include "http.hpp"
#include <iostream>
#include <fstream>
#include <sstream>



int main(int ac, char **av){
    std::ifstream file(av[1]);
    std::string line;
    std::string method, path, format;
    http h;

    if (!file.is_open()) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }
    std::getline(file, line);
    if (!(line).empty())
    {
        std::stringstream iss(line);
        iss >> method >> path >> format;
        h.setMethod(method);
        h.setPath(path);
        h.setFormat(format);
    }
    while (std::getline(file, line)) {
        if (line.empty())
            continue;
        h.setHttp(line);
    
    }
    h.printHttp();
    (h.getContent()).printHttpContent();


    file.close();
    return 0;
}


