#include <iostream>
#include <string>

int main()
{
    std::string org = "Host: localhost:8080";
    size_t pos_begin = org.find(":");
    pos_begin = org.find_first_not_of(" \t", pos_begin + 1);
    std::string sub1 = org.substr(pos_begin, std::string::npos);
    
    std::cout << sub1 << "\n";
    return 0;
}