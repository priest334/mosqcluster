#pragma once

#include <string>
#include <map>

using std::string;
using std::map;

class AppInfo;


class CorpInfo {
public:
	CorpInfo();
	CorpInfo(const string& corpid);
	virtual ~CorpInfo();

	AppInfo* GetApp(const string& name) const;
	int AppId(const string& name) const;

	void SetCorpId(const string& corpid);
	string const& CorpId() const;
	void SetAppParam(const string& app, const string& key, const string& value);
	string GetAppParam(const string& app, const string& key) const;

	bool CreateApplication(const string& name, const string& appid, const string& secret);
	string GetAccessToken(const string& name);

	string GetJsapiTicket(const string& name);
private:
	string corp_id_;
	map<string, AppInfo*> apps_;
};


