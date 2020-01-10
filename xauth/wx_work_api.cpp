#include "wx_work_api.h"

#include "helper/http_client.h"

namespace {
	struct ApiResp {
		long status_code_;
		string content_;
	};

	void ApiRespHandler(void* user_ptr, long status_code, const char* headers, const char* content, size_t size) {
		struct ApiResp* resp = (struct ApiResp*)user_ptr;
		resp->status_code_ = status_code;
		resp->content_.assign(content, size);
	}
}


WxResp::WxResp() {
}

bool WxResp::Parse(const string& content) {
	return json_wrapper_.Parse(content.c_str());
}

string WxResp::Get(const string& key) {
	string value;
	json_wrapper_.Get(key.c_str(), value);
	return value;
}

int WxResp::GetInt(const string& key) {
	int value;
	json_wrapper_.Get(key.c_str(), value);
	return value;
}


string WxApi::Get(const string& url) {
	struct ApiResp resp;
	int code = HttpGet(&resp, url.c_str(), NULL, ApiRespHandler);
	if (0 == code && resp.status_code_ == 200) {
		return resp.content_;
	}
	return "";
}

string WxApi::Post(const string& url, const string& data) {
	struct ApiResp resp;
	int code = HttpPost(&resp, url.c_str(), NULL, data.c_str(), ApiRespHandler);
	if (0 == code && resp.status_code_ == 200) {
		return resp.content_;
	}
	return "";
}


