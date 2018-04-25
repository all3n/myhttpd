#ifndef _MY_HTTPD_H_
#define _MY_HTTPD_H_
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include "http_request.h"
#include "http_response.h"

class myhttpd
{
private:
    u_short port;
    u_int32_t addr;
    int server_socket;
    std::string htdoc;
public:
    myhttpd(u_short _port, std::string htdoc);
    ~myhttpd();
    void init();
    void start();
    void handle(int client, http_request & req, http_response & res);
    std::string recv_line(int socket, char end);
    void send_line(int socket, std::string line);

    static void * handle_request(void * args);
};

#endif
