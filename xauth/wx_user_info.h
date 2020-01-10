#pragma once

#include <string>

using std::string;


class WxUserInfo {
public:
	WxUserInfo(const string& access_token, const string& code);
	string const& raw() const;
	string const& UserId() const;
	string const& DeviceId() const;
private:
	string user_id_;
	string device_id_;
	string raw_content_;
};

