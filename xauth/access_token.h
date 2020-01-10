#pragma once

#include <string>

using std::string;


class CorpInfo;

class AccessToken {
public:
	AccessToken(CorpInfo* corp);
	~AccessToken();

	string Get(const string& app_name) const;

private:
	CorpInfo* corp_;
};

