#pragma once

#include <string>
#include "helper/json_wrapper.h"

class CorpInfo;

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
	static string AsyncPost(const string& url, const string& data);

	static void SendAppTextMessage(CorpInfo* corp, const string& app, const string& touser, const string& content);
	static void SendAppTextMessage(const string& corp, const string& app, const string& touser, const string& content);

	static string GetExternalContact(const string& corp, const string& app, const string& external_userid);
};



