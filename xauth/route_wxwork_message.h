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

class GetRouteWxWorkMessage : public RouteProc {
public:
	int Process(HttpRequest* req, HttpResponse* resp);
	RouteKey route_key() const;
};

class PostRouteWxWorkMessage : public RouteProc {
public:
	int Process(HttpRequest* req, HttpResponse* resp);
	RouteKey route_key() const;

public:
	typedef int (PostRouteWxWorkMessage::*MsgHandler)(HttpRequest*, HttpResponse*, XmlReader*);
	DECLARE_HANDLER_CLUSTER(msgtype, string, MsgHandler)

	int OnRecvText(HttpRequest* req, HttpResponse* resp, XmlReader* xml);
	int OnRecvImage(HttpRequest* req, HttpResponse* resp, XmlReader* xml);
	int OnRecvVoice(HttpRequest* req, HttpResponse* resp, XmlReader* xml);
	int OnRecvVideo(HttpRequest* req, HttpResponse* resp, XmlReader* xml);
	int OnRecvLocation(HttpRequest* req, HttpResponse* resp, XmlReader* xml);
};


