#include "access_token.h"
#include "corp_info.h"


AccessToken::AccessToken(CorpInfo* corp) : corp_(corp) {
}


AccessToken::~AccessToken() {
}


string AccessToken::Get(const string& app_name) const {
	return corp_->GetAccessToken(app_name);
}


