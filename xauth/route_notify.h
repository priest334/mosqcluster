#pragma once

#include "route_api.h"



class RouteNotify : public RouteApi {
public:
	RouteNotify();
	RouteKey route_key() const;
	int ApiHandler(HttpRequest* req, HttpResponse* resp, CorpInfo* info);
};


