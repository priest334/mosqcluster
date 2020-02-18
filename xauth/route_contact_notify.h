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

public:
	typedef int (RouteContactHandler::*MsgHandler)(HttpRequest*, HttpResponse*, XmlReader*);

	DECLARE_HANDLER_CLUSTER(msgtype, string, MsgHandler)
	int OnEvent(HttpRequest* req, HttpResponse* resp, XmlReader* xml);

	DECLARE_HANDLER_CLUSTER(event, string, MsgHandler)
	int OnChangeContact(HttpRequest* req, HttpResponse* resp, XmlReader* xml);
	int OnChangeExternalContact(HttpRequest* req, HttpResponse* resp, XmlReader* xml);
	int OnChangeExternalChat(HttpRequest* req, HttpResponse* resp, XmlReader* xml);

	DECLARE_HANDLER_CLUSTER(change_contact, string, MsgHandler)
	int OnAddUser(HttpRequest* req, HttpResponse* resp, XmlReader* xml);
	int OnUpdateUser(HttpRequest* req, HttpResponse* resp, XmlReader* xml);
	int OnDeleteUser(HttpRequest* req, HttpResponse* resp, XmlReader* xml);

	DECLARE_HANDLER_CLUSTER(change_external_contact, string, MsgHandler)
	int OnAddExternalContact(HttpRequest* req, HttpResponse* resp, XmlReader* xml);
	int OnDelExternalContact(HttpRequest* req, HttpResponse* resp, XmlReader* xml);
	int OnDelFollowUser(HttpRequest* req, HttpResponse* resp, XmlReader* xml);

};


