#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_
#include <string>
#include <map>
#include "http_constants.h"

using namespace std;

class http_request
{
public:
    string method;
    string url;
    string file;
    string protocol;
    map<string, string> header;
    map<string, string> params;
    string body;
    int client;
    http_request(int _client);

    bool inline is_crlf(string line)
    {
        return line.compare(CRLF) == 0;
    }
    void recv_line(string &line, const char & end);
    void parseInfo();
    void parseHeader();
    void parseBody();
    void parseArgs(string line);
    void printHeader();
    void debug();
};
#endif
