#include <iostream>

const char* ft_strstr(const char* haystack, const char* needle)
{
    if (!haystack)
        return NULL;
    if (!needle)
        return haystack;
 
    for (const char* p = haystack; *p; ++p)
    {
        const char* h = p;
        const char* n = needle;
        while (*h && *n && *h == *n)
        {
            h++;
            n++;
        }
        if (!*n)
            return p;        
    }
    return NULL;
}

int main()
{
    const char* a = "i love rikaaaa";
    const char* b = "rika";
    const char* c = NULL;

    c = ft_strstr(a, b);
    std::cout << c << "\n";

}
