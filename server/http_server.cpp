#include "http_server.h"

#include <errno.h>
#include "linux_dummy.h"
#include "http_request.h"
#include "routes.h"
#include "server.h"

namespace {

	class NextTask;

	NextTask* SendResponse(int sock, const mstr& buffer);

	class NextTask : public ThreadPool::Task {
	public:
		NextTask(int sock, const mstr& content) : sock_(sock), content_(content) {
		}

		void Run() {
			NextTask* nxt = SendResponse(sock_, content_);
			if (nxt) {
				Owner()->Push(nxt);
				return;
			}
			close(sock_);
		}

		void Finish() {
			delete this;
		}

	private:
		int sock_;
		mstr content_;
	};


	NextTask* SendResponse(int sock, const mstr& buffer) {
		int total = buffer.length();
		int len = send(sock, buffer.c_str(), total, 0);
		if (len > 0 && len < total) {
			return new NextTask(sock, buffer.substr(len));
		}
		else if (errno == EAGAIN || errno == EWOULDBLOCK) {
			return new NextTask(sock, buffer);
		}
		return NULL;
	}


	class HttpTask : public ThreadPool::Task {
	public:
		HttpTask(Routes* routes, HttpRequest* request) : routes_(routes),request_(request) {
		}

		~HttpTask() {
			delete request_;
		}

		void Run() {
			HttpResponse resp;
			if (routes_) {
				if (0 == routes_->Process(request_, &resp)) {
					mstr content = resp.Message();
					NextTask* nxt = SendResponse(request_->socket(), content);
					if (nxt) {
						Owner()->Push(nxt);
						return;
					}	
				}
				close(request_->socket());
			}
		}
		void Finish() {
			delete this;
		}
	private:
		Routes* routes_;
		HttpRequest* request_;
	};
}


static void* LoopMainThread(void* argp) {
	HttpServer* server = reinterpret_cast<HttpServer*>(argp);
	server->LoopMain();
	return (void*)0;
}


static int OnAccept(void* userptr, ClientContext* client) {
	HttpServer* server = reinterpret_cast<HttpServer*>(userptr);
	return server->OnAccept(&client->userptr_, client->fd_);
}

static int OnRead(void* userptr, ClientContext* client, const char* data, int size) {
	HttpServer* server = reinterpret_cast<HttpServer*>(userptr);
	return server->OnRead(client->userptr_, client->fd_, data, size);
}

static int OnClose(void* userptr, ClientContext* client) {
	HttpServer* server = reinterpret_cast<HttpServer*>(userptr);
	return server->OnClose(client->userptr_, client->fd_);
}

static int OnTimeOut(void* userptr, ClientContext* client) {
	HttpServer* server = reinterpret_cast<HttpServer*>(userptr);
	return server->OnTimeOut(client->userptr_, client->fd_);
}


HttpServer::HttpServer(Routes* routes/* = NULL*/) : handle_(new ServerHandle), routes_(routes){
	handle_->read_timeout_ = 15;
	handle_->write_timeout_ = 5;
	handle_->accept_cb = ::OnAccept;
	handle_->read_cb = ::OnRead;
	handle_->close_cb = ::OnClose;
	handle_->timeout_cb = ::OnTimeOut;
	handle_->userptr_ = this;
}

HttpServer::~HttpServer() {
}

void HttpServer::Start(int port, const char* ip/* = 0*/) {
	handle_->listen_socket_ = CreateServerSocket(ip, port);
	HttpRequestBase::Init();
	if (routes_) {
		routes_->InitTable();
	}
	thread_pool_ = new ThreadPool();
	thread_pool_->Create(20);
	pthread_create(&loop_, NULL, LoopMainThread, this);
	pthread_join(loop_, NULL);
}

void HttpServer::Stop() {
	StopServer(handle_);
}

void HttpServer::LoopMain() {
	StartServer(handle_);
}

int HttpServer::OnAccept(void** client, int sock) {
	*client = new HttpRequest(sock);
	return 0;
}

int HttpServer::OnRead(void* client, int sock, const char* data, int size) {
	HttpRequest* request = reinterpret_cast<HttpRequest*>(client);
	request->Append(data, size);
	if (request->Finished()) {
		thread_pool_->Push(new HttpTask(routes_, request));
		return -1;
	}
	return 0;
}

int HttpServer::OnClose(void* client, int sock) {
	return 0;
}

int HttpServer::OnTimeOut(void* client, int sock) {
	return 0;
}



