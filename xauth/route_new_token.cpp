#include "route_new_token.h"
#include "server/http_request.h"


int RouteNewToken::Process(HttpRequest* req, HttpResponse* resp) {
	resp->SetStatus(HTTP_STATUS_OK);
	return 0;
}
RouteKey RouteNewToken::route_key() const {
	return RouteKey(HTTP_GET, "/xauth/token");
}