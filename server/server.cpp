#include "server.h"

#include "linux_dummy.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

enum ServerStatus {
	SrvRunning,
	SrvStopped
};

struct SocketContext {
	int fd_;
	int index_;
	time_t read_timestamp_;
	time_t write_timestamp_;
	time_t read_timeout_;
	time_t write_timeout_;
	ClientContext cli_;
};

struct ServerContext {
	volatile int accept_size_;
	volatile int stop_;
	volatile int stat_;
};

typedef struct SocketContext* SocketContextPtr;

int ServerLoopMain(ServerHandle* server);


int SetNonBlocking(int fd, bool nio) {
	int opts = fcntl(fd, F_GETFL);
	if (nio)
		opts |= O_NONBLOCK;
	else
		opts &= ~O_NONBLOCK;
	return fcntl(fd, F_SETFL, opts);
}

int SetReuseAddr(int fd, bool reuse) {
	int opt = reuse ? 1 : 0;
	int oldopt = 0;
	socklen_t optlen = 0;
	getsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&oldopt, &optlen);
	return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
}

int SetLinger(int fd, bool on, int linger) {
	struct linger opt;
	opt.l_onoff = on ? 1 : 0;
	opt.l_linger = linger;
	return setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char*)&opt, sizeof(opt));
}

int CreateServerSocket(const char* ip, int port) {
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = ip ? inet_addr(ip) : INADDR_ANY;

	int listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (-1 == listen_socket) {
		return -1;
	}

	SetReuseAddr(listen_socket, true);
	SetNonBlocking(listen_socket, true);

	if (0 != bind(listen_socket, (const struct sockaddr*)&addr, sizeof(struct sockaddr))) {
		return -1;
	}

	if (0 != listen(listen_socket, 1024)) {
		return -1;
	}

	return listen_socket;
}


void StartServer(ServerHandle* handle) {
	handle->ctx_ = (ServerContext*)malloc(sizeof(ServerContext));
	handle->ctx_->stat_ = SrvStopped;
	ServerLoopMain(handle);
}

void StopServer(ServerHandle* handle) {
	ServerContext* ctx = handle->ctx_;
	ctx->stop_ = 1;
	int timeout = 5;
	while (ctx->stat_ != SrvStopped && timeout > 0) {
		timeout--;
		sleep(1);
	}
}

int ServerLoopMain(ServerHandle* server) {
	ServerContext* ctx = (ServerContext*)server->ctx_;
	void* userptr = server->userptr_;

	int max_events_count = 128 * 1024;

	int epfd = epoll_create(max_events_count);
	epoll_event* events = (epoll_event*)malloc(max_events_count * sizeof(epoll_event));

	SocketContext listen_socket;
	listen_socket.fd_ = server->listen_socket_;

	epoll_event ev;
	ev.data.ptr = &server->listen_socket_;
	ev.events = EPOLLET | EPOLLIN;
	epoll_ctl(epfd, EPOLL_CTL_ADD, listen_socket.fd_, &ev);

	int accept_size = 0;
	int max_accept_size = 1024 * 1024;
	SocketContextPtr* accept_queue = (SocketContextPtr*)malloc(max_accept_size * sizeof(SocketContextPtr));
	memset(accept_queue, 0, max_accept_size * sizeof(SocketContextPtr));

	int recvbuf_size = 16 * 1024;
	char* recvbuf = new char[recvbuf_size];
	while (!ctx->stop_) {
		time_t now = time(NULL);
		int count = epoll_wait(epfd, events, max_events_count, 1000);
		for (int i = 0; i < count; i++) {
			epoll_event& e = events[i];
			SocketContextPtr client = (SocketContextPtr)e.data.ptr;
			int sock = client->fd_;
			if (sock == listen_socket.fd_) {
				if (e.events & EPOLLIN) {
					for (;;) {
						struct sockaddr_in addr;
						socklen_t len = sizeof(struct sockaddr_in);
						int socket = accept(sock, (sockaddr*)&addr, &len);
						if (socket > 0) {
							SetNonBlocking(socket, true);
							accept_size++;
							if (accept_size > max_accept_size) {
								close(socket);
								accept_size--;
								break;
							}
							SocketContextPtr newclient = (SocketContextPtr)malloc(sizeof(SocketContext));
							newclient->read_timestamp_ = now;
							newclient->read_timeout_ = newclient->read_timestamp_ + server->read_timeout_;
							newclient->write_timestamp_ = -1;
							newclient->write_timeout_ = -1;
							newclient->fd_ = socket;
							newclient->cli_.fd_ = socket;
							newclient->cli_.userptr_ = NULL;
							for (int i = 0; i < accept_size; i++) {
								if (accept_queue[i] == NULL) {
									newclient->index_ = i;
									accept_queue[i] = newclient;
									break;
								}
							}
							server->accept_cb(userptr, &newclient->cli_);
							ev.events = EPOLLIN | EPOLLRDHUP;
							ev.data.ptr = newclient;
							epoll_ctl(epfd, EPOLL_CTL_ADD, socket, &ev);
							continue;
						}
						break;
					}
				}
				continue;
			}

			bool remove = false;
			if (e.events & EPOLLIN) {
				client->read_timestamp_ = now;
				client->read_timeout_ = client->read_timestamp_ + server->read_timeout_;
				while (true) {
					int size = recv(sock, recvbuf, recvbuf_size, 0);
					if (size > 0) {
						if (server->read_cb(userptr, &client->cli_, recvbuf, size) < 0) {
							epoll_ctl(epfd, EPOLL_CTL_DEL, client->fd_, NULL);
							client->fd_ = -1;
							client->read_timestamp_ = -1;
							client->read_timeout_ = -1;
							client->write_timestamp_ = now;
							client->write_timeout_ = client->write_timestamp_ + server->write_timeout_;
							remove = true;
						}
						continue;
					}
					if (size == 0 || (errno != EAGAIN && errno != EWOULDBLOCK)) {
						remove = true;
					}
					break;
				}
			}

			if ((e.events & (EPOLLHUP | EPOLLRDHUP)) || remove) {
				accept_queue[client->index_] = NULL;
				accept_size--;
				if (client->fd_ > 0) {
					epoll_ctl(epfd, EPOLL_CTL_DEL, client->fd_, NULL);
					server->close_cb(userptr, &client->cli_);
					close(client->fd_);
				}
				free(client);
			}
		}

		if (count < 0) {
			SocketContext* iter = NULL;
			int active_size = accept_size;
			for (int i = 0; i < accept_size; i++) {
				iter = accept_queue[i];
				if (iter != NULL &&
					((iter->read_timestamp_ > 0 && now > iter->read_timeout_) ||
					(iter->write_timestamp_ > 0 && now > iter->write_timeout_)))
				{
					server->timeout_cb(userptr, &iter->cli_);
					accept_queue[i] = NULL;
					active_size--;
					if (iter->fd_ > 0) {
						epoll_ctl(epfd, EPOLL_CTL_DEL, iter->fd_, NULL);
						server->close_cb(userptr, &iter->cli_);
						close(iter->fd_);
					}
					free(iter);
				}
			}
			accept_size = active_size;
			ctx->accept_size_ = accept_size;
		}

	}

	ctx->stat_ = SrvStopped;
	free(events);
	free(accept_queue);
	free(recvbuf);

	return 0;
}

