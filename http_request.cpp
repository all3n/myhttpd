#include "http_request.h"
#include "utils.h"
#include <string>
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <sstream>

#define MAX_HEADER_SIZE 100
using namespace std;

http_request::http_request(int _client): client(_client)
{
}

void http_request::recv_line(string &line, const char &end = '\n')
{
    ostringstream ostr;
    char c = 0;
    ssize_t t = -1;
    while (c != end)
    {
        t = recv(client, &c, 1, 0);
        //no data
        if (t == 0 )
        {
            break;
        }
        ostr << c;
    }
    line = ostr.str();
}


void http_request::parseInfo()
{
    // 请求行
    // method path protocol
    // GET /favicon.ico HTTP/1.1\r\n
    string line;
    recv_line(line);
    if (line.empty())
    {
        return;
    }
    vector<string> s;
    utils::rstrip(line, string(CRLF));
    utils::split(line, string(" "), s);
    this->method = s[0];
    this->url = s[1];
    size_t queryCharExist = url.find("?");
    if (queryCharExist != string::npos)
    {
        this->file = url.substr(0, queryCharExist);
        if (url.length() > queryCharExist)
        {
            int start = queryCharExist + 1;
            int size = url.length() - start;
            cout << "url substr:" << start << ":" << size << endl;
            string paramsQuery = url.substr(start, size);
            parseArgs(paramsQuery);
        }
    }
    else
    {
        this->file = url;
    }
    this->protocol = s[2];
}

void http_request::parseHeader()
{
    string line;
    int i = 0;
    while (true)
    {
        vector<string> s;
        recv_line(line);
        if (is_crlf(line))
        {
            break;
        }
        if (i++ > MAX_HEADER_SIZE)
        {
            cerr << "header parse " << MAX_HEADER_SIZE << endl;
            break;
        }
        utils::rstrip(line, string(CRLF));
        utils::split(line, string(HEADER_SEP), s);
        if (s.size() == 2)
        {
            header[s[0]] = s[1];
        }
    }
}
void http_request::parseArgs(string args)
{
    vector<string> paramsVec;
    utils::split(args, string("&"), paramsVec);
    for (vector<string>::iterator i = paramsVec.begin();
            i < paramsVec.end();
            i++ )
    {
        vector<string> paramItemVec;
        utils::split(*i, string("="), paramItemVec);
        if (paramItemVec.size() == 2)
        {
            params[paramItemVec[0]] = paramItemVec[1];
        }
    }
}
void http_request::parseBody()
{
    map<string, string>::iterator iter = header.find(HEADER_CONTENT_LENGTH);
    if (iter != header.end())
    {
        cout << "find" << iter->second << endl;
        int contentLength = atoi(iter->second.c_str());
        if (contentLength < MAX_CONTENT_LENGTH)
        {
            char * buf = new char[contentLength];
            recv(client, buf, contentLength, 0);
            map<string, string>::iterator iter2 = header.find(HEADER_CONTENT_TYPE);
            if (iter2 == header.end())
            {
                return;
            }
            string contentType = iter2->second;
            if (contentType.compare(CONTENT_TYPE_FORM) == 0)
            {
                parseArgs(body);
            }
            delete [] buf;
            cout << body << endl;
        }
    }
}

void  http_request::printHeader()
{
    cout << "headers:" << endl;
    for (map<string, string>::iterator i = header.begin();
            i != header.end();
            i++)
    {
        cout << i->first << ":" << i->second << endl;
    }
}

void http_request::debug()
{
    cout << "METHOD:" << method << endl;
    cout << "FILE:" << file << endl;
    cout << "PROTOCOL:" << protocol << endl;

    printHeader();
    // print args
    for (map<string, string>::iterator i = params.begin();
            i != params.end();
            i++)
    {
        cout << i->first << ":" << i->second << endl;
    }
}
