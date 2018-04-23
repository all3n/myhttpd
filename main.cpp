#include <iostream>
#include "myhttpd.h"

int main(int argc, char *argv[])
{
    MyHttpd httpd(8008,"htdoc");
    httpd.start();
    return 0;
}
