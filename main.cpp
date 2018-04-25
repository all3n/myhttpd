#include <iostream>
#include "myhttpd.h"
#include <iostream>

int main(int argc, char *argv[])
{
    try
    {
        myhttpd httpd(8008, "htdoc");
        httpd.start();
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
