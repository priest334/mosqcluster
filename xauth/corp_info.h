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

	void SetCorpId(const string& corpid);
	string const& CorpId() const;

	bool CreateApplication(const string& name, const string& appid, const string& secret);
	string GetAccessToken(const string& name);

private:
	string corp_id_;
	map<string, AppInfo*> apps_;
};


