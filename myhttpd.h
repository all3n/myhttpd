#ifndef _MY_HTTPD_H_
#define _MY_HTTPD_H_
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>

class MyHttpd{
    private:
        u_short port;
        u_int32_t addr;
        int server_socket;
        std::string htdoc;
    public:
        MyHttpd(u_short _port,std::string htdoc);
        ~MyHttpd();
        void init();
        void start();
        static void * handle(void *);
        static std::string recv_line(int socket,char end);
        static void send_line(int socket,std::string line);
};

#endif
