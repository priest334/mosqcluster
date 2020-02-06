#pragma once

#include <string>
#include "server/routes.h"

using std::string;


class HttpRequest;
class HttpResponse;
class CorpInfo;


class RouteApi : public RouteProc {
public:
	RouteApi(const string& corp, const string& app);
	int Process(HttpRequest* req, HttpResponse* resp);
	virtual int ApiHandler(HttpRequest* req, HttpResponse* resp, CorpInfo* info);
	
protected:
	string corp_;
	string app_;
};


