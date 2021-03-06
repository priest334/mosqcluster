#pragma once

#include "mosq_auth_route_proc.h"


class RouteCheckAcl : public MosqAuthRouteProc {
public:
	int Process(HttpRequest* req, HttpResponse* resp);
	RouteKey route_key() const;
};

