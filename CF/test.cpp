#include "http.hpp"
#include <iostream>
#include <fstream>



int main(int ac, char **av){
    std::ifstream file(av[1]);
    std::string line;
    if (!file.is_open()) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }

    while (std::getline(file, line)) {
        std::cout << line << std::endl; // Process each line
    }

    file.close(); // Close the file
    return 0;
}


