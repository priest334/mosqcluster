#pragma once

#include <string>
#include "server/routes.h"
#include "handler_dispatch.h"
#include "xml_reader.h"

using std::string;

namespace Tencent {
	class WXBizMsgCrypt;
}

class HttpRequest;
class HttpResponse;


class RouteCallback : public RouteProc {
public:
	virtual ~RouteCallback();
	RouteCallback();
	RouteCallback(const string& corp, const string& app);
	int Process(HttpRequest* req, HttpResponse* resp);
	virtual int OnMessage(HttpRequest* req, HttpResponse* resp, Tencent::WXBizMsgCrypt* wxbmc) = 0;
protected:
	string corp_;
	string app_;
};


class RouteCallbackVerify : public RouteCallback {
public:
	virtual ~RouteCallbackVerify();
	RouteCallbackVerify();
	RouteCallbackVerify(const string& corp, const string& app);
	int OnMessage(HttpRequest* req, HttpResponse* resp, Tencent::WXBizMsgCrypt* wxbmc);
};

class RouteCallbackHandler : public RouteCallback {
public:
	virtual ~RouteCallbackHandler();
	RouteCallbackHandler();
	RouteCallbackHandler(const string& corp, const string& app);
	int OnMessage(HttpRequest* req, HttpResponse* resp, Tencent::WXBizMsgCrypt* wxbmc);
	virtual int HandleMessage(HttpRequest* req, HttpResponse* resp, Tencent::WXBizMsgCrypt* wxbmc, XmlReader* xml) = 0;
};


