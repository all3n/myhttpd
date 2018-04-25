/*
 * http_thread_args.h
 * Copyright (C) 2018 wanghch <wanghch@wanghch-PC>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef HTTP_THREAD_ARGS_H
#define HTTP_THREAD_ARGS_H
#include "myhttpd.h"

class http_thread_args
{
public:
    int client;
    myhttpd * pserver;
    http_thread_args(int _client, myhttpd * _pserver);
};


#endif /* !HTTP_THREAD_ARGS_H */
