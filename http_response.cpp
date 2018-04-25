/*
 * http_response.cpp
 * Distributed under terms of the MIT license.
 */

#include "http_response.h"
#include <netinet/in.h>
#include <dirent.h>
#include <cstring>
using namespace std;

http_response::http_response(int _client): client(_client)
{
}


http_response & http_response::ok()
{
    SET_STATUS(STATUS_CODE_OK);
    return *this;
}

http_response & http_response::not_found()
{
    SET_STATUS(STATUS_CODE_NOT_FOUND);
    return *this;
}
http_response & http_response::not_implemented()
{
    SET_STATUS(STATUS_CODE_NOT_IMPLEMENTED);
    return *this;
}
http_response & http_response::server_error()
{
    SET_STATUS(STATUS_CODE_INTERNAL_SERVER_ERROR);
    return *this;
}


http_response & http_response::status(int statusCode, string statusStr)
{
    this->statusCode = statusCode;
    this->statusStr = statusStr;
    return *this;
}


http_response & http_response::static_file(FILE * resource)
{
    char buf[1024];
    fgets(buf, sizeof(buf), resource);
    while (!feof(resource))
    {
        send(client, buf, strlen(buf), 0);
        fgets(buf, sizeof(buf), resource);
    }
    return *this;
}



http_response & http_response::send_header()
{
    out << "HTTP/1.0 " << statusCode << " " << statusStr << CRLF;
    out << "Server: " << SERVER_NAME << "/" << VERSION << CRLF;
    out << "Content-Type: text/html" << CRLF;
    out << CRLF;
    const string header = out.str();
    send(client, header.c_str(), header.length(), 0);
    out.clear();
    return *this;
}
http_response & http_response::body(const string &s)
{
    out << s << CRLF;
    const string cs = out.str();
    send(client, cs.c_str(), cs.length(), 0);
    out.clear();
    return *this;
}

http_response & http_response::list(const char * scandir)
{
    DIR * dir;
    struct dirent *ent;
    out.clear();
    out.str("");
    out << "<html><head></head><body><ul>" << CRLF;
    if ((dir = opendir(scandir)) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            if (strcmp(ent->d_name, ".") == 0)
            {
                continue;
            }
            out << "<li>" << CRLF;
            out << "<a href='/" << ent->d_name << "'>" << ent->d_name << "</a>" << CRLF;
            out << "</li>" << CRLF;
        }
        closedir (dir);
    }
    out << "</ul></body></html>" << CRLF;
    const string s = out.str();
    send(client, s.c_str(), s.length(), 0);
    return *this;
}
