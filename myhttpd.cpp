#include "myhttpd.h"
#include "http_protocol.h"
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <cstdio>
#include <dirent.h>
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

    // 请求行
    // method path protocol
    // GET /favicon.ico HTTP/1.1\r\n
    line = MyHttpd::recv_line(client);
    http_protocol hp;
    cout << "REQUEST:" << line << endl;
    if (line.empty())
    {
        return NULL;
    }
    hp.parseInfo(line);
    cout << "method:" << hp.method << endl;

    // 消息报头
    bool end = false;
    while (!end)
    {
        line = MyHttpd::recv_line(client);
        hp.parseHeader(line);
        if (line.compare(CRLF) == 0)
        {
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
    // 请求正文
    if (hp.method.compare("POST") == 0)
    {
        cout << "request body" << endl;
        map<string, string>::iterator iter = hp.header.find(HEADER_CONTENT_LENGTH);
        if (iter != hp.header.end())
        {
            cout << "find" << iter->second << endl;
            int contentLength = atoi(iter->second.c_str());
            if (contentLength < MAX_CONTENT_LENGTH)
            {
                char * buf = new char[contentLength];
                recv(client, buf, contentLength, 0);
                hp.parseBody(string(buf));
                delete [] buf;
                cout << hp.body << endl;
            }
        }
    }

    for (map<string, string>::iterator i = hp.params.begin();
            i != hp.params.end();
            i++)
    {
        cout << i->first << ":" << i->second << endl;
    }

    //response
    send_line(client, string("HTTP/1.0 200 OK\r\n"));
    send_line(client, string("Server: myhttpd/1.0.0\r\n"));
    send_line(client, string("Content-Type: text/html\r\n"));
    send_line(client, string("\r\n"));

    string rf = "htdoc/";
    if (hp.file == "/")
    {
        rf += "index.html";
    }
    else
    {
        rf += hp.file;
    }
    struct stat st;
    cout << "stat:" << rf << endl;

    if (hp.file.compare(string("/list")) == 0)
    {
        DIR *dir;
        struct dirent *ent;
        cout << "list files" << endl;
        if ((dir = opendir ("htdoc/")) != NULL)
        {
            /* print all the files and directories within directory */
            while ((ent = readdir (dir)) != NULL)
            {
                printf ("%s\n", ent->d_name);
                send_line(client, string(ent->d_name) + "<br/>\r\n");
            }
            closedir (dir);
        }
        else
        {

        }
    }
    else if (stat(rf.c_str(), &st) == -1)
    {
        cout << rf << " not found" << endl;
        //not found
    }
    else
    {
        FILE * resource = fopen(rf.c_str(), "r");
        char buf[1024];
        fgets(buf, sizeof(buf), resource);
        while (!feof(resource))
        {
            send(client, buf, strlen(buf), 0);
            fgets(buf, sizeof(buf), resource);
        }
        fclose(resource);
    }

    cout << "close client" << client << endl;
    cout << "------end ------" << endl;
    close(client);
    return NULL;
}

void MyHttpd::start()
{
    if (listen(server_socket, 5) < 0)
    {
        throw runtime_error("listen fail");
    }
    cout << "listen port " << port << endl;

    int client_sock = -1;
    struct sockaddr_in client_name;
    socklen_t  client_name_len = sizeof(client_name);
    pthread_t newthread;

    //loop
    while (1)
    {
        cout << "wait" << endl;
        client_sock = accept(server_socket,
                             (struct sockaddr *)&client_name,
                             &client_name_len);
        if (client_sock == -1)
        {
            cerr << "accept fail" << endl;
        }
        if (pthread_create(&newthread, NULL, MyHttpd::handle, (void *)(intptr_t)client_sock) != 0)
        {
            perror("pthread_create");
        }
    }
}


MyHttpd::~MyHttpd()
{
    close(server_socket);
}
