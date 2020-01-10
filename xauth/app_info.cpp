#include "app_info.h"

#include "global.h"
#include "helper/string_helper.h"
#include "wx_work_api.h"
#include "corp_info.h"


AppInfo::AppInfo(CorpInfo* corp_info, const string& appid, const string& secret)
	: corp_info_(corp_info), appid_(appid), secret_(secret), expires_in_(0){
}


AppInfo::~AppInfo() {
}


string AppInfo::GetAccessToken(bool force/* = false*/) {
	time_t now = time(NULL);
	if (!force && !access_token_.empty() && now < expires_in_) {
		return access_token_;
	}

	hlp::String url;
	url.Format("https://qyapi.weixin.qq.com/cgi-bin/gettoken?corpid=%s&corpsecret=%s", corp_info_->CorpId().c_str(), secret_.c_str());

	string content = WxApi::Get(url.str());
	logger::Info() << "token: " << content;

	WxResp resp;
	resp.Parse(content);
	access_token_ = resp.Get("access_token");
	time_t expires_in = resp.GetInt("expires_in");
	expires_in_ = now + expires_in;
	return access_token_;
}



