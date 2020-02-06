#include "route_notify.h"

#include "server/http_request.h"
#include "helper/string_helper.h"
#include "helper/json_wrapper.h"
#include "global.h"
#include "wx_work_api.h"


using std::string;

RouteNotify::RouteNotify() : RouteApi("Main", "notify") {
}

RouteKey RouteNotify::route_key() const {
	return RouteKey(HTTP_POST, "/wxwork/notify");
}

int RouteNotify::ApiHandler(HttpRequest* req, HttpResponse* resp, CorpInfo* info) {
	resp->SetStatus(HTTP_STATUS_OK);

	string data = req->body().c_str();
	logger::Debug() << "notify body: " << data;

	string access_token = info->GetAccessToken(app_);
	int appid = info->AppId(app_);

	hlp::String url;
	url.Format("https://qyapi.weixin.qq.com/cgi-bin/message/send?access_token=%s", access_token.c_str());

	hlp::JsonDocument doc;
	doc.Parse(data);
	string touser, content;
	doc.Get("touser", touser);
	doc.Get("content", content);

	hlp::JsonDocument sdoc;
	sdoc.Set("touser", touser);
	sdoc.Set("msgtype", "text");
	sdoc.Set("agentid", appid);
	sdoc.Set("text.content", content);
	string message = sdoc.Write(false);

	logger::Info() << "message: " << message;
	string ret = WxApi::Post(url.str(), message);
	logger::Info() << "response: " << ret;
	if (!ret.empty()) {
		WxResp wxresp;
		wxresp.Parse(ret);
	}

	return 0;
}






