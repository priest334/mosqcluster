#pragma once

#include <time.h>
#include <string>
#include <map>
using std::string;
using std::map;

namespace hlp {
	class SimpleCache;
}

class CorpInfo;

class AppInfo {
public:
	AppInfo(CorpInfo* corp_info, const string& appid, const string& secret);
	~AppInfo();

	int AppId() const;

	void SetAppParam(const string& key, const string& value);
	string GetAppParam(const string& key) const;

	string GetAccessToken(bool force = false);

private:
	CorpInfo* corp_info_;
	string appid_;
	string secret_;
	string access_token_;
	time_t expires_in_;
	map<string, string> params_;
	hlp::SimpleCache* cache_;
};


