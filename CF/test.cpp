#include <iostream>
#include <string>
#include "../INC/utils/HTTPRequest.hpp"
#include "../INC/utils/ServerManager.hpp"


// int main()
// {
//     std::string org = "Host: localhost:8080";
//     size_t pos_begin = org.find(":");
//     pos_begin = org.find_first_not_of(" \t", pos_begin + 1);
//     std::string sub1 = org.substr(pos_begin, std::string::npos);
    
//     std::cout << sub1 << "\n";
//     return 0;
// }


// int main()
// {
//     std::string accept = "image/avif,image/webp,image/apng,image/svg+xml,image/,/*;q=0.8";
//     HTTPRequest req;
//     req.setAccept(accept); // Corrected method name
    
//     for (std::vector<Accept>::const_iterator it = req.getAccept().begin(); 
//          it != req.getAccept().end(); it++) {
//         std::cout << it->type_ << "\n";
//         std::cout << it->subtype_ << "\n";
//         std::cout << it->piority_ << "\n";
//     }
    
//     return (0);
// }


int main()
{
    std::string accept = "en-GB,en;q=0.9";
    HTTPRequest req;
    req.setAcceptLanguage(accept); // Corrected method name
    
    for (std::map<std::string, float>::const_iterator it = req.getAcceptLanguage().begin(); 
         it != req.getAcceptLanguage().end(); it++) {
        std::cout << it->first << "\n";
        std::cout << it->second << "\n";
        std::cout << "xxxxxxxxxxx" << "\n";
    }
    
    return (0);
}