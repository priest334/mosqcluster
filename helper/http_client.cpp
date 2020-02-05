#include "http_client.h"

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

struct CurlInfo {
	bool initialized_;
};

struct CurlInfo* lib_curl_info = NULL;

struct HttpSession {
	HttpCallback cb_;
	int method_;
	void* user_ptr_;
	CURL* curl_;
	char* proxy_;
	long proxy_port_;
	char* proxy_userpwd_;
	long timeout_;
	char* post_data_;
	struct curl_slist* request_headers_;
};

struct MemBuffer {
	void* user_ptr_;
	char* buf_;
	size_t size_;
};

size_t MemoryCopyCallback(void* ptr, size_t size, size_t nmemb, void* stream) {
	size_t msize = size * nmemb;
	MemBuffer* mbuf = (MemBuffer*)stream;
	mbuf->buf_ = (char*)realloc(mbuf->buf_, mbuf->size_ + msize + 1);
	if (NULL == mbuf->buf_)
		return 0;

	memcpy(&(mbuf->buf_[mbuf->size_]), ptr, msize);
	mbuf->size_ += msize;
	mbuf->buf_[mbuf->size_] = '\0';
	return size * nmemb;
}

int HttpStartup() {
	if (!lib_curl_info)
		lib_curl_info = (struct CurlInfo*)malloc(sizeof(struct CurlInfo));
	if (!lib_curl_info->initialized_ && curl_global_init(CURL_GLOBAL_ALL) == CURLE_OK) {
		lib_curl_info->initialized_ = true;
	}
	return 0;
}

void* HttpCreateSession(void* user_ptr, HttpCallback cb, int method/* = HTTP_METHOD_DEFAULT*/, long timeout/* = 30L*/) {
	struct HttpSession* session = (struct HttpSession*)malloc(sizeof(HttpSession));
	session->cb_ = cb;
	session->method_ = method;
	if (method < HTTP_METHOD_DEFAULT || session->method_ > HTTP_METHOD_MAX)
		session->method_ = HTTP_METHOD_DEFAULT;
	session->user_ptr_ = user_ptr;
	session->curl_ = curl_easy_init();
	session->proxy_ = NULL;
	session->proxy_port_ = 0L;
	session->proxy_userpwd_ = NULL;
	session->timeout_ = timeout;
	session->post_data_ = NULL;
	session->request_headers_ = NULL;
	return session;
}

int HttpDestroySession(void* session) {
	if (!session)
		return -1;

	struct HttpSession* s = (struct HttpSession*)session;
	if (s->curl_)
		curl_easy_cleanup(s->curl_);

	free(s->proxy_);
	free(s->proxy_userpwd_);
	free(s->post_data_);

	if (s->request_headers_)
		curl_slist_free_all(s->request_headers_);
	
	free(s);

	return 0;
}

int HttpSetProxy(void* session, const char* proxy) {
	struct HttpSession* s = (struct HttpSession*)session;
	s->proxy_ = strdup(proxy);
	return 0;
}

int HttpSetProxyPort(void* session, long port) {
	struct HttpSession* s = (struct HttpSession*)session;
	s->proxy_port_ = port;
	return 0;
}

int HttpSetProxyAuthType(void* session, const char* proxy) {
	return 0;
}

int HttpSetProxyUserPwd(void* session, const char* userpwd) {
	struct HttpSession* s = (struct HttpSession*)session;
	s->proxy_userpwd_ = curl_easy_escape(s->curl_, s->proxy_userpwd_, strlen(s->proxy_userpwd_));
	return 0;
}

int HttpSetHeader(void* session, const char* header) {
	struct HttpSession* s = (struct HttpSession*)session;

	const char* begin = header;
	while (begin && (*begin != '\0')) {
		const char* end = strstr(begin, "\r\n");
		char line[1024] = { 0 };
		if (end) {
			strncpy(line, begin, end - begin);
			begin = end + 2;
		} else {
			strcpy(line, begin);
			begin = end;
		}
		if (strchr(line, ':')) {
			s->request_headers_ = curl_slist_append(s->request_headers_, line);
		}
	}
	
	return 0;
}

int HttpSetPostData(void* session, const char* data) {
	if (NULL == data)
		return -1;
	struct HttpSession* s = (struct HttpSession*)session;
	s->post_data_ = strdup(data);
	return 0;
}

int HttpOpen(void* session, const char* url) {
	struct HttpSession* s = (struct HttpSession*)session;

	curl_easy_setopt(s->curl_, CURLOPT_URL, url);
	if (HTTP_METHOD_POST == s->method_) {
		curl_easy_setopt(s->curl_, CURLOPT_POST, 1L);
		if (s->post_data_)
			curl_easy_setopt(s->curl_, CURLOPT_POSTFIELDS, s->post_data_);
	}

	if (s->proxy_) {
		curl_easy_setopt(s->curl_, CURLOPT_PROXY, s->proxy_);
		if (s->proxy_port_)
			curl_easy_setopt(s->curl_, CURLOPT_PROXYPORT, s->proxy_port_);
		if (s->proxy_userpwd_) {
			curl_easy_setopt(s->curl_, CURLOPT_PROXYUSERPWD, s->proxy_userpwd_);
		}
	}

	struct MemBuffer content, headers;
	content.user_ptr_ = s->user_ptr_;
	content.buf_ = (char*)malloc(32);
	content.size_ = 0;

	headers.user_ptr_ = s->user_ptr_;
	headers.buf_ = (char*)malloc(32);
	headers.size_ = 0;

	curl_easy_setopt(s->curl_, CURLOPT_HTTPHEADER, s->request_headers_);
	curl_easy_setopt(s->curl_, CURLOPT_ACCEPT_ENCODING, "");
	curl_easy_setopt(s->curl_, CURLOPT_HEADERFUNCTION, MemoryCopyCallback);
	curl_easy_setopt(s->curl_, CURLOPT_HEADERDATA, &headers);
	curl_easy_setopt(s->curl_, CURLOPT_WRITEFUNCTION, MemoryCopyCallback);
	curl_easy_setopt(s->curl_, CURLOPT_WRITEDATA, &content);
	curl_easy_setopt(s->curl_, CURLOPT_TIMEOUT, s->timeout_);
	curl_easy_setopt(s->curl_, CURLOPT_SSL_VERIFYPEER, 0);

	long status_code = -1;
	CURLcode retcode;
	do {
		retcode = curl_easy_perform(s->curl_);
		if (CURLE_OK != retcode) {
			break;
		}

		curl_easy_getinfo(s->curl_, CURLINFO_RESPONSE_CODE, &status_code);
		if (s->cb_)
			s->cb_(s->user_ptr_, status_code, headers.buf_, content.buf_, content.size_);
	} while (0);

	free(content.buf_);
	free(headers.buf_);

	return retcode;
}

int HttpGet(void* user_ptr, const char* url, const char* headers, HttpCallback cb, const char* proxy/* = 0*/, const char* proxy_userpwd/* = 0*/) {
	void* session = HttpCreateSession(user_ptr, cb, HTTP_METHOD_GET);
	HttpSetHeader(session, headers);
	if (proxy) {
		HttpSetProxy(session, proxy);
		if (proxy_userpwd)
			HttpSetProxyUserPwd(session, proxy_userpwd);
	}
	HttpOpen(session, url);
	HttpDestroySession(session);
	return 0;
}

int HttpPost(void* user_ptr, const char* url, const char* headers, const char* data, HttpCallback cb, const char* proxy/* = 0*/, const char* proxy_userpwd/* = 0*/) {
	void* session = HttpCreateSession(user_ptr, cb, HTTP_METHOD_POST);
	HttpSetHeader(session, headers);
	if (proxy) {
		HttpSetProxy(session, proxy);
		if (proxy_userpwd)
			HttpSetProxyUserPwd(session, proxy_userpwd);
	}
	HttpSetPostData(session, data);
	HttpOpen(session, url);
	HttpDestroySession(session);
	return 0;
}

int HttpShutdown() {
	if (!lib_curl_info)
		return 0;

	if (lib_curl_info->initialized_) {
		curl_global_cleanup();
		lib_curl_info->initialized_ = false;
	}

	free(lib_curl_info);
	lib_curl_info = NULL;
	return 0;
}

