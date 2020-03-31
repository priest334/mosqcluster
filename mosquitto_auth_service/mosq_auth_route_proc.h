#pragma once

#include "server/routes.h"


class HttpRequest;
class HttpResponse;

class MosqAuthRouteProc : public RouteProc {
public:
	virtual int Process(HttpRequest* req, HttpResponse* resp);
};

