/*
 * http_thread_args.cpp
 * Copyright (C) 2018 wanghch <wanghch@wanghch-PC>
 *
 * Distributed under terms of the MIT license.
 */

#include "http_thread_args.h"



http_thread_args::http_thread_args(int _client, myhttpd * _pserver):
    client(_client),
    pserver(_pserver)
{
}
