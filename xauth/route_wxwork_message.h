#pragma once

#include <string>
#include "server/routes.h"
#include "handler_dispatch.h"
#include "xml_reader.h"
#include "route_callback.h"


class RouteWxWorkMessageVerify : public RouteCallbackVerify {
public:
	RouteKey route_key() const;
};

class RouteWxWorkMessageHandler : public RouteCallbackHandler {
public:
	RouteKey route_key() const;
	int HandleMessage(HttpRequest* req, HttpResponse* resp, Tencent::WXBizMsgCrypt* wxbmc, XmlReader* xml);

public:
	typedef int (RouteWxWorkMessageHandler::*MsgHandler)(HttpRequest*, HttpResponse*, XmlReader*);
	DECLARE_HANDLER_CLUSTER(msgtype, string, MsgHandler)

	int OnRecvText(HttpRequest* req, HttpResponse* resp, XmlReader* xml);
	int OnRecvImage(HttpRequest* req, HttpResponse* resp, XmlReader* xml);
	int OnRecvVoice(HttpRequest* req, HttpResponse* resp, XmlReader* xml);
	int OnRecvVideo(HttpRequest* req, HttpResponse* resp, XmlReader* xml);
	int OnRecvLocation(HttpRequest* req, HttpResponse* resp, XmlReader* xml);
};


