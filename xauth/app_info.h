#pragma once

#include <time.h>
#include <string>
using std::string;

class CorpInfo;

class AppInfo {
public:
	AppInfo(CorpInfo* corp_info, const string& appid, const string& secret);
	~AppInfo();

	string GetAccessToken(bool force = false);

private:
	CorpInfo* corp_info_;
	string appid_;
	string secret_;
	string access_token_;
	time_t expires_in_;
};


