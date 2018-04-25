#include "myhttpd.h"
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <cstdio>

#include "http_request.h"
#include "http_response.h"
#include "http_thread_args.h"

using namespace std;

myhttpd::myhttpd(u_short _port, string _htdoc):
    port(_port),
    addr(INADDR_ANY),
    htdoc(_htdoc)
{
    cout << "port:" << this->port;
    cout << "\taddr:" <<  this->addr;
    cout << "\thtdoc:" << this->htdoc << endl;
    init();
}

void myhttpd::init()
{
    // domain type protocol
    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        throw runtime_error("init socket error");
    }
    sockaddr_in name;
    memset(&name, 0, sizeof(name));
    name.sin_family = AF_INET;
    name.sin_addr.s_addr = htonl(addr);
    name.sin_port = htons(port);
    int on = 1;

    if ((setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
    {
        throw runtime_error("setsockopt error");
    }

    if (bind(server_socket, (sockaddr *)&name, sizeof(name)) < 0)
    {
        ostringstream err;
        err << "bind fail " << port;
        throw runtime_error(err.str());
    }
}

void myhttpd::send_line(int client, string line)
{
    send(client, line.c_str(), line.length(), 0);
}

void myhttpd::handle(int client, http_request & req, http_response & res)
{
    // 请求行、消息报头、请求正文
    req.parseInfo();
    req.parseHeader();

    if (req.method.compare("POST") == 0)
    {
        req.parseBody();
    }
    //hp.printHeader();
    if (req.file.compare(string("/list")) == 0)
    {
        res.ok().send_header();
        res.list("htdoc/");
    }
    else
    {
        string rf = "htdoc/";
        if (req.file == "/")
        {
            rf += "index.html";
        }
        else
        {
            rf += req.file;
        }
        struct stat st;
        if (stat(rf.c_str(), &st) == -1)
        {
            res.not_found().send_header();
            res.body(string(rf) + " not found");
            cout << rf << " not found" << endl;
        }
        else
        {
            res.ok().send_header();
            FILE * resource = fopen(rf.c_str(), "r");
            res.static_file(resource);
            fclose(resource);
        }
    }
    close(client);
}
void * myhttpd::handle_request(void * args)
{
    http_thread_args * a = static_cast<http_thread_args *>(args);
    http_request req(a->client);
    http_response res(a->client);
    try
    {
        a->pserver->handle(a->client, req, res);
    }
    catch (exception &e)
    {
        req.debug();
        http_response err_res(a->client);
        err_res.server_error().send_header().body(string("server error"));
        cerr << "Req ERROR:" << e.what() << endl;
    }
    return NULL;
}

void myhttpd::start()
{
    if (listen(server_socket, 5) < 0)
    {
        throw runtime_error("listen fail");
    }
    int client_sock = -1;
    struct sockaddr_in client_name;
    socklen_t  client_name_len = sizeof(client_name);
    pthread_t newthread;
    while (1)
    {
        client_sock = accept(server_socket,
                             (struct sockaddr *)&client_name,
                             &client_name_len);
        if (client_sock == -1)
        {
            cerr << "accept fail" << endl;
        }
        // call static
        http_thread_args targs(client_sock, this);
        if (pthread_create(&newthread, NULL, myhttpd::handle_request, (void *)&targs) != 0)
        {
            perror("pthread_create");
        }
    }
}


myhttpd::~myhttpd()
{
    close(server_socket);
}
