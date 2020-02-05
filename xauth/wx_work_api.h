#pragma once

#include <string>
#include "helper/json_wrapper.h"

using std::string;

class WxResp {
public:
	WxResp();
	bool Parse(const string& content);

	string Get(const string& key);
	int GetInt(const string& key);
private:
	hlp::JsonDocument document_;
};

class WxApi {
public:
	static string Get(const string& url);
	static string Post(const string& url, const string& data);
};



