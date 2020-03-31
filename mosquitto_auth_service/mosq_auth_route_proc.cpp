#include "mosq_auth_route_proc.h"

#include "server/http_request.h"
#include "global.h"
#include "helper/json_wrapper.h"


int MosqAuthRouteProc::Process(HttpRequest* req, HttpResponse* resp) {
	resp->SetStatus(HTTP_STATUS_OK);
	resp->SetHeader("Content-Type", "application/json");
	resp->SetHeader("Server", "mosquitto_auth_service/1.0");
	return 0;
}


