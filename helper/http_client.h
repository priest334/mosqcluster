#pragma once

#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#include <stdlib.h>

enum _HttpMethod{
	HTTP_METHOD_DEFAULT = 0,
	HTTP_METHOD_GET = HTTP_METHOD_DEFAULT,
	HTTP_METHOD_POST = 1,
	HTTP_METHOD_MIMEPOST = 2,
	HTTP_METHOD_MAX = HTTP_METHOD_MIMEPOST
};

typedef void(*HttpCallback)(void*, long, const char*, const char*, size_t);

int HttpStartup();
void* HttpCreateSession(void* user_ptr, HttpCallback cb, int method = HTTP_METHOD_DEFAULT, long timeout = 30L);
int HttpDestroySession(void* session);
int HttpSetProxy(void* session, const char* proxy);
int HttpSetProxyPort(void* session, long port);
int HttpSetProxyAuthType(void* session, const char* proxy);
int HttpSetProxyUserPwd(void* session, const char* userpwd);
int HttpSetHeader(void* session, const char* header);
int HttpSetPostData(void* session, const char* data, long size);
int HttpOpen(void* session, const char* url, char** resp = 0, char** data = 0);
int HttpGet(void* user_ptr, const char* url, const char* headers, HttpCallback cb, const char* proxy = 0, const char* proxy_userpwd = 0);
int HttpPost(void* user_ptr, const char* url, const char* headers, const char* data, long size, HttpCallback cb, const char* proxy = 0, const char* proxy_userpwd = 0);
int HttpShutdown();
void HttpFreeData(char** data);
int HttpSendFile(void* user_ptr, const char* url, const char* headers, const char* file, char** resp, char** data, const char* proxy = 0, const char* proxy_userpwd = 0);


#endif // __HTTP_CLIENT_H__

