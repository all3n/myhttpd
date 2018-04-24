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

using namespace std;

MyHttpd::MyHttpd(u_short _port, string _htdoc):
    port(_port),
    addr(INADDR_ANY),
    htdoc(_htdoc)
{
    cout << this->port << endl;
    cout << this->addr << endl;
    cout << this->htdoc << endl;
    init();
}

void MyHttpd::init()
{
    std::cout << "init" << std::endl;
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
        cerr << "bind fail" << port << endl;
        throw runtime_error("bind fail");
    }
}

string MyHttpd::recv_line(int client, char end = '\n')
{
    ostringstream ostr;
    char c = 0;
    ssize_t t = -1;
    while (c != end && t != 0)
    {
        t = recv(client, &c, 1, 0);
        ostr << c;
    }
    return ostr.str();
}

void MyHttpd::send_line(int client, string line)
{
    send(client, line.c_str(), line.length(), 0);
}

void * MyHttpd::handle(void *p)
{
    int client = (intptr_t)p;
    cout << "------start------" << endl;
    cout << "accept client" << client << endl;
    string line;
    // 请求行、消息报头、请求正文

    http_request hp(client);
    http_response res(client);
    // 请求行
    // method path protocol
    // GET /favicon.ico HTTP/1.1\r\n
    line = MyHttpd::recv_line(client);
    cout << "REQUEST:" << line << endl;
    if (line.empty()){
        return NULL;
    }
    hp.parseInfo(line);
    cout << "method:" << hp.method << endl;

    // 消息报头
    bool end = false;
    while (!end){
        line = MyHttpd::recv_line(client);
        hp.parseHeader(line);
        if (line.compare(CRLF) == 0){
            end = true;
        }
    }

    cout << "headers:" << endl;
    for (map<string, string>::iterator i = hp.header.begin();
            i != hp.header.end();
            i++)
    {
        cout << i->first << ":" << i->second << endl;
    }
    if (hp.method.compare("POST") == 0){
        hp.parseBody();
    }

    for (map<string, string>::iterator i = hp.params.begin();
            i != hp.params.end();
            i++){
        cout << i->first << ":" << i->second << endl;
    }

    if (hp.file.compare(string("/list")) == 0){
        res.ok().send_header();
        res.list("htdoc/");
    }else{
        string rf = "htdoc/";
        if (hp.file == "/"){
            rf += "index.html";
        }else{
            rf += hp.file;
        }
        struct stat st;
        if (stat(rf.c_str(), &st) == -1){
            res.not_found().send_header();
            res.body(string(rf)+" not found");
            cout << rf << " not found" << endl;
        }else{
            res.ok().send_header();
            FILE * resource = fopen(rf.c_str(), "r");
            res.static_file(resource);
            fclose(resource);
        }
    }

    cout << "close client" << client << endl;
    cout << "------end ------" << endl;
    close(client);
    return NULL;
}

void MyHttpd::start()
{
    if (listen(server_socket, 5) < 0){
        throw runtime_error("listen fail");
    }
    cout << "listen port " << port << endl;

    int client_sock = -1;
    struct sockaddr_in client_name;
    socklen_t  client_name_len = sizeof(client_name);
    pthread_t newthread;

    //loop
    while (1){
        cout << "wait" << endl;
        client_sock = accept(server_socket,
                (struct sockaddr *)&client_name,
                &client_name_len);
        if (client_sock == -1){
            cerr << "accept fail" << endl;
        }
        if (pthread_create(&newthread, NULL, MyHttpd::handle, (void *)(intptr_t)client_sock) != 0){
            perror("pthread_create");
        }
    }
}


MyHttpd::~MyHttpd()
{
    close(server_socket);
}
