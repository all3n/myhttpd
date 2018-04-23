#ifndef _HTTP_PROTOCOL_H_
#define _HTTP_PROTOCOL_H_
#include <string>
#include <map>

#define CRLF "\r\n"
#define HEADER_SEP ": "
#define HEADER_CONTENT_TYPE "content-type"
#define HEADER_CONTENT_LENGTH "content-length"


#define CONTENT_TYPE_FORM "application/x-www-form-urlencoded"

#define MAX_CONTENT_LENGTH 4096000

using namespace std;

class http_protocol{
    public:
        string method;
        string url;
        string file;
        string protocol;
        map<string,string> header;
        map<string,string> params;
        string body;


        bool inline is_crlf(string line){
            return line.compare(CRLF) == 0;
        }
        void parseInfo(string line);
        void parseHeader(string line);
        void parseBody(string line);
        void parseArgs(string line);
};
#endif
