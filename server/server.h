#pragma once

struct ServerContext;
struct ClientContext {
	int fd_;
	void* userptr_;
};

struct ServerHandle {
	int(*accept_cb)(void* userptr, ClientContext* client);
	int(*read_cb)(void* userptr, ClientContext* client, const char* data, int size);
	int(*close_cb)(void* userptr, ClientContext* client);
	int(*timeout_cb)(void* userptr, ClientContext* client);
	int listen_socket_;
	int read_timeout_;
	int write_timeout_;
	void* userptr_;
	ServerContext* ctx_;
};


int CreateServerSocket(const char* ip, int port);
void StartServer(ServerHandle* handle);
void StopServer(ServerHandle* handle);

