#include "corp_info.h"

#include "app_info.h"

CorpInfo::CorpInfo() {
}

CorpInfo::CorpInfo(const string& corpid) : corp_id_(corpid){
}

CorpInfo::~CorpInfo() {
}

void CorpInfo::SetCorpId(const string& corpid) {
	corp_id_ = corpid;
}

string const& CorpInfo::CorpId() const {
	return corp_id_;
}

bool CorpInfo::CreateApplication(const string& name, const string& appid, const string& secret) {
	map<string, AppInfo*>::const_iterator iter = apps_.find(name);
	if (iter != apps_.end()) {
		return false;
	}	
	apps_[name] = new AppInfo(this, appid, secret);
	return true;
}


string CorpInfo::GetAccessToken(const string& name) {
	map<string, AppInfo*>::const_iterator iter = apps_.find(name);
	if (iter != apps_.end()) {
		AppInfo* app = iter->second;
		return app->GetAccessToken();
	}
	return "";
}



