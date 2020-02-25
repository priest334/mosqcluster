#include "app_info.h"

#include "global.h"
#include "helper/string_helper.h"
#include "wx_work_api.h"
#include "corp_info.h"
#include "helper/cache.h"
#include "helper/lock_impl_posix.h"


namespace {
	time_t decrement_expires_in(time_t origin, time_t value) {
		return (origin > value) ? (origin - value) : origin;
	}
}

AppInfo::AppInfo(CorpInfo* corp_info, const string& appid, const string& secret)
	: corp_info_(corp_info), appid_(appid), secret_(secret), expires_in_(0), cache1_(0), cache2_(0){
	hlp::String str;
	str.Format("file://./%s", appid_.c_str());
	hlp::SimpleCacheEngineFactory factory;
	cache1_ = new hlp::SimpleCache(factory.Create("memory://"), new LockImplPosix());
	cache2_ = new hlp::SimpleCache(factory.Create(str.str()), new LockImplPosix());
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

string AppInfo::GetParamFromCache(const string& key) {
	time_t expires_in;
	string data = cache1_->Get(key, expires_in);
	if (data.empty()) {
		data = cache2_->Get(key, expires_in);
	}
	return data;
}

void AppInfo::SetParamToCache(const string& key, const string& data, time_t expires_in) {
	cache1_->Set(key, data, expires_in);
	cache2_->Set(key, data, expires_in);
}

string AppInfo::GetAccessToken(bool force/* = false*/) {
	string access_token;
	if (!force) {
		access_token = GetParamFromCache("access_token");
		if (access_token.empty())
			return access_token;
	}

	hlp::String url;
	url.Format("https://qyapi.weixin.qq.com/cgi-bin/gettoken?corpid=%s&corpsecret=%s", corp_info_->CorpId().c_str(), secret_.c_str());
	string content = WxApi::Get(url.str());
	logger::Debug() << "content: " << content;

	WxResp resp;
	resp.Parse(content);
	access_token = resp.Get("access_token");
	if (!access_token.empty()) {
		time_t expires_in = resp.GetInt("expires_in");
		SetParamToCache("access_token", access_token, decrement_expires_in(expires_in, 200));
	}
	return access_token;
}

string AppInfo::GetCorpJsapiTicket(bool force/* = false*/) {
	string ticket;
	if (!force) {
		ticket = GetParamFromCache("corp_jsapi_ticket");
		if (ticket.empty())
			return ticket;
	}

	hlp::String url;
	url.Format("https://qyapi.weixin.qq.com/cgi-bin/get_jsapi_ticket?access_token=%s", GetAccessToken(force).c_str());
	string content = WxApi::Get(url.str());
	logger::Debug() << "content: " << content;

	WxResp resp;
	resp.Parse(content);
	ticket = resp.Get("ticket");
	if (!ticket.empty()) {
		time_t expires_in = resp.GetInt("expires_in");
		SetParamToCache("corp_jsapi_ticket", ticket, decrement_expires_in(expires_in, 200));
	}
	return ticket;
}

string AppInfo::GetAppJsapiTicket(bool force/* = false*/) {
	string ticket;
	if (!force) {
		ticket = GetParamFromCache("app_jsapi_ticket");
		if (ticket.empty())
			return ticket;
	}

	hlp::String url;
	url.Format("https://qyapi.weixin.qq.com/cgi-bin/ticket/get?access_token=%s&type=agent_config", GetAccessToken(force).c_str());
	string content = WxApi::Get(url.str());
	logger::Debug() << "content: " << content;

	WxResp resp;
	resp.Parse(content);
	ticket = resp.Get("ticket");
	if (!ticket.empty()) {
		time_t expires_in = resp.GetInt("expires_in");
		SetParamToCache("app_jsapi_ticket", ticket, decrement_expires_in(expires_in, 200));
	}
	return ticket;
}

string AppInfo::GetJsapiTicket(bool force/* = false*/) {
	string corp_jsapi_ticket = GetCorpJsapiTicket(force);
	string app_jsapi_ticket = GetAppJsapiTicket(force);
	hlp::String content;
	content.Format("{\"corp_jsapi_ticket\": \"%s\", \"app_jsapi_ticket\": \"%s\"}", corp_jsapi_ticket.c_str(), app_jsapi_ticket.c_str());
	return content.str();
}


