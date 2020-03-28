#pragma once

#include <string>
#include <vector>
#include "string_helper.h"

using std::string;
using std::vector;

class HttpWrapper {
	class HttpResponse {
		friend class HttpWrapper;
	public:
		HttpResponse();
		long status_code() const;
		string header(const string& key) const;
		vector<string> header_all(const string& key) const;
		string body() const;

	private:
		long status_code_;
		string body_;
		hlp::StringMultiMap headers_;
	};
	static void HttpRespHandler(void* user_ptr, long status_code, const char* headers, const char* content, size_t size);
public:
	HttpWrapper();
	void SetHeader(const string& key, const string& value);
	bool Get(const string& url);
	bool Post(const string& url, const void* data, size_t size);
	bool Post(const string& url, const string& data);
	HttpResponse const& response() const;
	string Text() const;
	
private:
	hlp::StringMultiMap headers_;
	HttpResponse response_;
};

