#include "http_request.h"
#include "utils.h"
#include <string>
#include <iostream>
#include <sys/socket.h>
using namespace std;

http_request::http_request(int _client):client(_client){
}

void http_request::parseInfo(string line)
{
    vector<string> s;
    line = utils::rstrip(line, string(CRLF));
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
            string paramsQuery = url.substr(start, url.length() - start);
            cout << "paramQuery:" << paramsQuery << endl;
            parseArgs(paramsQuery);
        }
    }
    else
    {
        this->file = url;
    }
    cout << "file:" << file << endl;



    this->protocol = s[2];
}

void http_request::parseHeader(string line)
{
    vector<string> s;
    line = utils::rstrip(line, string(CRLF));
    utils::split(line, string(HEADER_SEP), s);
    if (s.size() == 2)
    {
        header[s[0]] = s[1];
    }
}
void http_request::parseArgs(string args)
{
    vector<string> paramsVec;
    utils::split(args, string("&"), paramsVec);
    for (vector<string>::const_iterator i = paramsVec.cbegin();
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
    cout << "request body" << endl;
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

