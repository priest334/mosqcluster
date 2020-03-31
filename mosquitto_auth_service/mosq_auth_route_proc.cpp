#include "mosq_auth_route_proc.h"

#include "server/http_request.h"
#include "global.h"
#include "helper/json_wrapper.h"


int MosqAuthRouteProc::Process(HttpRequest* req, HttpResponse* resp) {
	logger::Debug() << req->url().c_str();
	logger::Debug() << req->body().c_str();

	resp->SetStatus(HTTP_STATUS_OK);
	resp->SetHeader("Content-Type", "application/json");
	resp->SetHeader("Server", "mosquitto_auth_service/1.0");
	return 0;
}


