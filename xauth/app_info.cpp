#include "app_info.h"

#include "global.h"
#include "helper/string_helper.h"
#include "wx_work_api.h"
#include "corp_info.h"
#include "helper/cache.h"
#include "helper/lock_impl_posix.h"


AppInfo::AppInfo(CorpInfo* corp_info, const string& appid, const string& secret)
	: corp_info_(corp_info), appid_(appid), secret_(secret), expires_in_(0), cache_(0){
	hlp::String str;
	str.Format("file://./%s", appid_.c_str());
	hlp::SimpleCacheEngineFactory factory;
	cache_ = new hlp::SimpleCache(factory.Create(str.str()), new LockImplPosix());
}

AppInfo::~AppInfo() {
}

int AppInfo::AppId() const {
	if (appid_.empty())
		return -1;
	return atoi(appid_.c_str());
}

void AppInfo::SetAppParam(const string& key, const string& value) {
	params_[key] = value;
}

string AppInfo::GetAppParam(const string& key) const {
	map<string, string>::const_iterator iter = params_.find(key);
	if (iter != params_.end()) {
		return iter->second;
	}
	return string();
}

string AppInfo::GetAccessToken(bool force/* = false*/) {
	if (access_token_.empty()) {
		access_token_ = cache_->Get("access_token", expires_in_);
	}
	time_t now = time(NULL);
	if (!force && !access_token_.empty() && now < expires_in_) {
		return access_token_;
	}

	hlp::String url;
	url.Format("https://qyapi.weixin.qq.com/cgi-bin/gettoken?corpid=%s&corpsecret=%s", corp_info_->CorpId().c_str(), secret_.c_str());

	string content = WxApi::Get(url.str());
	logger::Info() << "content: " << content;

	WxResp resp;
	resp.Parse(content);
	access_token_ = resp.Get("access_token");
	time_t expires_in = resp.GetInt("expires_in");
	expires_in_ = now + expires_in;
	cache_->Set("access_token", access_token_, expires_in - 200);
	return access_token_;
}

