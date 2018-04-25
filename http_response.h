/*
 * http_response.h
 * Copyright (C) 2018 wanghch <wanghch@wanghch-PC>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H
#include <sstream>
#include "http_constants.h"
#include <string>
#define SET_STATUS(STATUS_CODE) \
    this->status(STATUS_CODE,string(STATUS_CODE##_STR))

using namespace std;
class http_response
{
private:
    int client;
    ostringstream out;
    int statusCode;
    string statusStr;
public:
    http_response(int _client);
    http_response & ok();
    http_response & server_error();
    http_response & not_found();
    http_response & not_implemented();
    http_response & status(int statusCode, string statusStr);
    http_response & send_header();
    http_response & body(const string &s);
    http_response & static_file(FILE * resource);
    http_response & list(const char * scandir);
};


#endif /* !HTTP_RESPONSE_H */
