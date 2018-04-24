#include "http_protocol.h"
#include "utils.h"
#include <string>
#include <iostream>
using namespace std;

void http_protocol::parseInfo(string line)
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

void http_protocol::parseHeader(string line)
{
    vector<string> s;
    line = utils::rstrip(line, string(CRLF));
    utils::split(line, string(HEADER_SEP), s);
    if (s.size() == 2)
    {
        header[s[0]] = s[1];
    }
}
void http_protocol::parseArgs(string args)
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
void http_protocol::parseBody(string _body)
{
    this->body = _body;
    map<string, string>::iterator iter = header.find(HEADER_CONTENT_TYPE);
    if (iter == header.end())
    {
        return;
    }
    string contentType = iter->second;
    if (contentType.compare(CONTENT_TYPE_FORM) == 0)
    {
        parseArgs(body);
    }
}

