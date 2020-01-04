#pragma once

#include <pthread.h>
#include "helper/thread_pool.h"

struct ServerHandle;
class Routes;

class HttpServer {
public:
	HttpServer(Routes* routes = NULL);
	virtual ~HttpServer();

	void Start(int port, const char* ip = 0);
	void Stop();
	void LoopMain();

	virtual int OnAccept(void** client, int sock);
	virtual int OnRead(void* client, int sock, const char* data, int size);
	virtual int OnClose(void* client, int sock);
	virtual int OnTimeOut(void* client, int sock);

private:
	ServerHandle* handle_;
	pthread_t loop_;
	ThreadPool* thread_pool_;
	Routes* routes_;
};


