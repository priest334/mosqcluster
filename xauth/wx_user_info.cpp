#include "wx_user_info.h"

#include "global.h"
#include "helper/string_helper.h"
#include "wx_work_api.h"


WxUserInfo::WxUserInfo(const string& access_token, const string& code) {
	hlp::String url;
	url.Format("https://qyapi.weixin.qq.com/cgi-bin/user/getuserinfo?access_token=%s&code=%s", access_token.c_str(), code.c_str());

	raw_content_ = WxApi::Get(url.str());
	logger::Info() << "userinfo: " << raw_content_;

	WxResp resp;
	resp.Parse(raw_content_);
	user_id_ = resp.Get("UserId");
	device_id_ = resp.Get("DeviceId");
}

string const& WxUserInfo::raw() const {
	return raw_content_;
}

string const& WxUserInfo::UserId() const {
	return user_id_;
}

string const& WxUserInfo::DeviceId() const {
	return device_id_;
}


