#pragma once


#include <string>
#include <map>
#include <sstream>

#include "http_parser.h"
#include "helper/xstring.h"


//using mstr = std::string;

typedef hlp::xstring mstr;
using std::map;
using std::ostringstream;

class HttpRequestBase {
public:
	static http_parser_settings* settings_;
	static void Init();
	static void Cleanup();
	HttpRequestBase();
	virtual ~HttpRequestBase();

	int OnUrl(const char* at, size_t length);
	int OnHeadersComplete();
	int OnBody(const char* at, size_t length);
	int OnHeaderField(const char* at, size_t length);
	int OnHeaderValue(const char* at, size_t length);
	int OnMessageBegin();
	int OnMessageComplete();
	int OnQueryField(const char* at, size_t length);
	int OnQueryValue(const char* at, size_t length);
	int OnFragment(const char* at, size_t length);

public:
	bool finished_;
	http_parser parser_;
	mstr tempkey_;
	mstr tempval_;
	mstr url_;
	int method_;
	mstr body_;
	mstr fragment_;
	map<mstr, mstr> headers_;
	map<mstr, mstr> queries_;
};

class HttpRequest : private HttpRequestBase {
public:
	HttpRequest(int socket);
	~HttpRequest();

	bool Finished() const;
	bool IsTimeOut(time_t now, time_t timeout) const;
	size_t Append(const char* buffer, size_t size);

	int socket() const;
	mstr url() const;
	int method() const;
	mstr fragment() const;
	mstr body() const;
	mstr header(const mstr& name) const;
	mstr query(const mstr& key) const;
	bool HasHeader(const mstr& name) const;
	bool HasQuery(const mstr& key) const;

	void data(const mstr& str);
	mstr const& data() const;

private:
	int socket_;
	time_t timestamp_;
	mstr data_;
};


class HttpResponse {
public:
	HttpResponse();
	~HttpResponse();

	void SetStatus(int code);
	void SetHeader(const mstr& name, const mstr& value);
	void SetContent(const mstr& content);

	friend ostringstream& operator<<(ostringstream& oss, const HttpResponse& src);

	mstr Message() const;

private:
	int status_code_;
	map<mstr, mstr> headers_;
	mstr content_;
};

