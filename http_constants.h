/*
 * http_constants.h
 * Copyright (C) 2018 wanghch <wanghch@wanghch-MacBookPro.local>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef HTTP_CONSTANTS_H
#define HTTP_CONSTANTS_H

#define CRLF "\r\n"

#define HEADER_SEP ": "
#define SERVER_NAME "myhttpd"
#define VERSION "1.0.0"

#define HEADER_CONTENT_TYPE "content-type"
#define HEADER_CONTENT_LENGTH "content-length"
#define CONTENT_TYPE_FORM "application/x-www-form-urlencoded"

#define MAX_CONTENT_LENGTH 4096000



#define STATUS_CODE_OK 200
#define STATUS_CODE_OK_STR "OK"

#define STATUS_CODE_NOT_FOUND 404
#define STATUS_CODE_NOT_FOUND_STR "NOT FOUND"


#define STATUS_CODE_NOT_IMPLEMENTED 501
#define STATUS_CODE_NOT_IMPLEMENTED_STR "Method Not Implemented"




#endif /* !HTTP_CONSTANTS_H */
