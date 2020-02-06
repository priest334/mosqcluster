#pragma once

#include <string>
#include "server/routes.h"
#include "handler_dispatch.h"
#include "xml_reader.h"
#include "route_callback.h"


class RouteContactVerify : public RouteCallbackVerify {
public:
	RouteKey route_key() const;
	RouteContactVerify();
};

class RouteContactHandler : public RouteCallbackHandler {
public:
	RouteKey route_key() const;
	RouteContactHandler();
	int HandleMessage(HttpRequest* req, HttpResponse* resp, Tencent::WXBizMsgCrypt* wxbmc, XmlReader* xml);

};


