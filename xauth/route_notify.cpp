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

	hlp::JsonDocument doc;
	doc.Parse(data);
	string touser, content;
	doc.Get("touser", touser);
	doc.Get("content", content);

	WxApi::SendAppTextMessage(info, app_, touser, content);
	return 0;
}






