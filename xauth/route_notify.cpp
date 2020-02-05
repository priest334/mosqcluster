#include "route_notify.h"

#include "server/http_request.h"
#include "helper/string_helper.h"
#include "helper/json_wrapper.h"
#include "global.h"
#include "wx_work_api.h"


using std::string;


int RouteNotify::Process(HttpRequest* req, HttpResponse* resp) {
	resp->SetStatus(HTTP_STATUS_OK);

	string corp = req->query("corp").c_str();
	string app = req->query("notify").c_str();
	string data = req->body().c_str();
	CorpInfo* info = app::Corp(corp.empty() ? "Main" : corp);
	string access_token = info->GetAccessToken(app.empty() ? "notify" : app);
	int appid = info->AppId(app.empty() ? "notify" : app);

	logger::Info() << "notify body: " << data;

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

	WxResp wxresp;
	wxresp.Parse(ret);
	return 0;
}

RouteKey RouteNotify::route_key() const {
	return RouteKey(HTTP_POST, "/wxwork/notify");
}




