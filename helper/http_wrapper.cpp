#include "http_wrapper.h"
#include "http_client.h"

HttpWrapper::HttpResponse::HttpResponse() {
}

long HttpWrapper::HttpResponse::status_code() const {
	return status_code_;
}

string HttpWrapper::HttpResponse::header(const string& key) const {
	return hlp::TripLeft(headers_.Find(key), ' ');
}

vector<string> HttpWrapper::HttpResponse::header_all(const string& key) const {
	vector<string> all = headers_.FindAll(key);
	for (vector<string>::iterator iter = all.begin(); iter != all.end(); ++iter) {
		*iter = hlp::TripLeft(*iter, ' ');
	}
	return all;
}


string HttpWrapper::HttpResponse::body() const {
	return body_;
}

void HttpWrapper::HttpRespHandler(void* user_ptr, long status_code, const char* headers, const char* content, size_t size) {
	HttpWrapper* pthis = reinterpret_cast<HttpWrapper*>(user_ptr);
	pthis->response_.status_code_ = status_code;
	pthis->response_.body_.assign(content, size);
	pthis->response_.headers_.Split(headers, "\r\n", ":");
}

HttpWrapper::HttpWrapper() {
}

void HttpWrapper::SetHeader(const string& key, const string& value) {
	headers_.Append(key, value);
}

bool HttpWrapper::Get(const string& url) {
	string headers = headers_.ToString("\r\n", ":");
	HttpGet(this, url.c_str(), headers.c_str(), HttpRespHandler);
	return (response_.status_code() == 200);
}

bool HttpWrapper::Post(const string& url, const void* data, size_t size) {
	string headers = headers_.ToString("\r\n", ":");
	HttpPost(this, url.c_str(), headers.c_str(), (const char*)data, size, HttpRespHandler);
	return (response_.status_code() == 200);
}

bool HttpWrapper::Post(const string& url, const string& data) {
	return Post(url, data.c_str(), data.size());
}

HttpWrapper::HttpResponse const& HttpWrapper::response() const {
	return response_;
}

string HttpWrapper::Text() const {
	return response_.body();
}


