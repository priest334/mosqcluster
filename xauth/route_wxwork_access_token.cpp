#include "route_wxwork_access_token.h"

#include "server/http_request.h"
#include "helper/string_helper.h"
#include "helper/json_wrapper.h"
#include "global.h"
#include "corp_info.h"
#include "wx_work_api.h"
#include "wx_user_info.h"



int RouteWxWorkAccessToken::Process(HttpRequest* req, HttpResponse* resp) {
	resp->SetStatus(HTTP_STATUS_OK);

	string corp = req->query("corp").c_str();
	string app = req->query("app").c_str();
	if (corp.empty() || app.empty()) {
		logger::Error() << "corp: " << corp << ", app: " << app << " must both not empty";
		return 0;
	}
	
	string access_token = app::GetAccessToken(corp, app);

	hlp::JsonDocument doc;
	doc.Set("access_token", access_token);

	resp->SetHeader("Content-Type", "application/json");
	resp->SetContent(doc.Write(false));

	return 0;
}
RouteKey RouteWxWorkAccessToken::route_key() const {
	return RouteKey(HTTP_GET, "/wxwork/access_token");
}
