#include "wx_work_api.h"

#include "helper/http_client.h"
#include "helper/string_helper.h"
#include "helper/xstring.h"
#include "global.h"

typedef hlp::xstring mstr;

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

	class AsyncPostTask : public ThreadPool::Task {
	public:
		AsyncPostTask(const string& url, const string& content) : url_(url), content_(content) {
		}

		~AsyncPostTask() {
		}

		void Run() {
			string url = url_.c_str(), content = content_.c_str();
			logger::Debug() << url;
			logger::Debug() << "--: " << content;
			string ret = WxApi::Post(url, content);
			logger::Debug() << ret;
		}
		void Finish() {
			delete this;
		}
	private:
		mstr url_;
		mstr content_;
	};
}


WxResp::WxResp() {
}

bool WxResp::Parse(const string& content) {
	return document_.Parse(content);
}

string WxResp::Get(const string& key) {
	string value;
	document_.Get(key, value);
	return value;
}

int WxResp::GetInt(const string& key) {
	int value;
	document_.Get(key, value);
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
	const char* headers = "Content-Type: application/json";
	struct ApiResp resp;
	int code = HttpPost(&resp, url.c_str(), headers, data.c_str(), ApiRespHandler);
	if (0 == code && resp.status_code_ == 200) {
		return resp.content_;
	}
	return "";
}

void WxApi::AsyncPost(const string& url, const string& data) {
	async::Execute(new AsyncPostTask(url, data));
}

void WxApi::SendAppTextMessage(CorpInfo* corp, const string& app, const string& touser, const string& content) {
	string access_token = corp->GetAccessToken(app);
	int appid = corp->AppId(app);

	hlp::String url;
	url.Format("https://qyapi.weixin.qq.com/cgi-bin/message/send?access_token=%s", access_token.c_str());

	hlp::JsonDocument doc;
	doc.Set("touser", touser);
	doc.Set("msgtype", "text");
	doc.Set("agentid", appid);
	doc.Set("text.content", content);
	string message = doc.Write(false);

	AsyncPost(url.str(), message);
}

void WxApi::SendAppTextMessage(const string& corp, const string& app, const string& touser, const string& content) {
	CorpInfo* info = app::Corp(corp);
	SendAppTextMessage(info, "notify", touser, content);
}

string WxApi::GetExternalContact(const string& corp, const string& app, const string& external_userid) {
	CorpInfo* info = app::Corp(corp);
	string access_token = info->GetAccessToken("external_contact");
	if (access_token.empty()) {
		logger::Error() << "access_token is empty";
		return "";
	}
	hlp::String url;
	url.Format("https://qyapi.weixin.qq.com/cgi-bin/externalcontact/get?access_token=%s&external_userid=%s", access_token.c_str(), external_userid.c_str());
	string ret = WxApi::Get(url.str());
	return ret;
}


