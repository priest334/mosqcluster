#include "route_new_token.h"

#include "server/http_request.h"
#include "global.h"
#include "corp_info.h"
#include "wx_work_api.h"
#include "wx_user_info.h"



int RouteNewToken::Process(HttpRequest* req, HttpResponse* resp) {
	resp->SetStatus(HTTP_STATUS_OK);

	string corp = req->query("corp").c_str();
	string app = req->query("app").c_str();
	string code = req->query("code").c_str();
	string access_token = app::GetAccessToken(corp, app);

	WxUserInfo user_info(access_token, code);
	resp->SetContent(user_info.raw());

	return 0;
}
RouteKey RouteNewToken::route_key() const {
	return RouteKey(HTTP_GET, "/xauth/token");
}