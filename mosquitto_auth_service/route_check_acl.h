#pragma once

#include "server/routes.h"

class HttpRequest;
class HttpResponse;

class RouteCheckAcl : public RouteProc {
public:
	int Process(HttpRequest* req, HttpResponse* resp);
	RouteKey route_key() const;
};

