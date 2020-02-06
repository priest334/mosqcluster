#include "route_api.h"

#include "server/http_request.h"
#include "helper/string_helper.h"
#include "helper/json_wrapper.h"
#include "global.h"
#include "wx_work_api.h"


using std::string;

RouteApi::RouteApi(const string& corp, const string& app) 
	: corp_(corp), app_(app) {
}

int RouteApi::Process(HttpRequest* req, HttpResponse* resp) {
	resp->SetStatus(HTTP_STATUS_OK);
	CorpInfo* info = app::Corp(corp_);
	return ApiHandler(req, resp, info);
}

int RouteApi::ApiHandler(HttpRequest* req, HttpResponse* resp, CorpInfo* info) {
	return 0;
}




