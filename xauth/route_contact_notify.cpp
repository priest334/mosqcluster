#include "route_contact_notify.h"

#include "global.h"
#include "server/http_request.h"
#include "wxwork/WXBizMsgCrypt.h"
#include "wx_work_api.h"

using std::string;
using namespace Tencent;

RouteKey RouteContactVerify::route_key() const {
	return RouteKey(HTTP_GET, "/wxwork/contact");
}

RouteContactVerify::RouteContactVerify() 
	: RouteCallbackVerify("Main", "contact") {
}

RouteKey RouteContactHandler::route_key() const {
	return RouteKey(HTTP_POST, "/wxwork/contact");
}

RouteContactHandler::RouteContactHandler()
	: RouteCallbackHandler("Main", "contact") {
}

int RouteContactHandler::HandleMessage(HttpRequest* req, HttpResponse* resp, WXBizMsgCrypt* wxbmc, XmlReader* xml) {
	logger::Debug() << "RouteContactHandler";
	string msgtype;
	if (0 != xml->Get("MsgType", msgtype)) {
		logger::Error() << "Invalid Message Type: " << msgtype;
		return 0;
	}

	logger::Debug() << "MsgType: " << msgtype;
	MsgHandler handler = GET_HANDLER(msgtype, msgtype);
	if (handler != NULL) {
		return (this->*handler)(req, resp, xml);
	}
	return 0;
}


BEGIN_HANDLER_CLUSTER(msgtype, RouteContactHandler)
	ADD_HANDLER_TO_CLUSTER("event", &RouteContactHandler::OnEvent)
END_HANDLER_CLUSTER("")

int RouteContactHandler::OnEvent(HttpRequest* req, HttpResponse* resp, XmlReader* xml) {
	string event;
	if (0 != xml->Get("Event", event)) {
		logger::Error() << "Get Event Failed: " << event;
		return 0;
	}

	logger::Debug() << "Event: " << event;
	MsgHandler handler = GET_HANDLER(event, event);
	if (handler != NULL) {
		return (this->*handler)(req, resp, xml);
	}
	return 0;
}

BEGIN_HANDLER_CLUSTER(event, RouteContactHandler)
	ADD_HANDLER_TO_CLUSTER("change_contact", &RouteContactHandler::OnChangeContact)
	ADD_HANDLER_TO_CLUSTER("change_external_contact", &RouteContactHandler::OnChangeExternalContact)
	ADD_HANDLER_TO_CLUSTER("change_external_chat", &RouteContactHandler::OnChangeExternalChat)
END_HANDLER_CLUSTER("")

int RouteContactHandler::OnChangeContact(HttpRequest* req, HttpResponse* resp, XmlReader* xml) {
	string change_type;
	if (0 != xml->Get("ChangeType", change_type)) {
		logger::Error() << "Get ChangeType Failed: " << change_type;
		return 0;
	}
	logger::Debug() << "ChangeType: " << change_type;
	MsgHandler handler = GET_HANDLER(change_contact, change_type);
	if (handler != NULL) {
		return (this->*handler)(req, resp, xml);
	}
	return 0;
}

int RouteContactHandler::OnChangeExternalContact(HttpRequest* req, HttpResponse* resp, XmlReader* xml) {
	string change_type;
	if (0 != xml->Get("ChangeType", change_type)) {
		logger::Error() << "Get ChangeType Failed: " << change_type;
		return 0;
	}
	logger::Debug() << "ChangeType: " << change_type;
	MsgHandler handler = GET_HANDLER(change_external_contact, change_type);
	if (handler != NULL) {
		return (this->*handler)(req, resp, xml);
	}
	return 0;
}

int RouteContactHandler::OnChangeExternalChat(HttpRequest* req, HttpResponse* resp, XmlReader* xml) {
	return 0;
}

BEGIN_HANDLER_CLUSTER(change_contact, RouteContactHandler)
	ADD_HANDLER_TO_CLUSTER("add_user", &RouteContactHandler::OnAddUser)
	ADD_HANDLER_TO_CLUSTER("update_user", &RouteContactHandler::OnUpdateUser)
	ADD_HANDLER_TO_CLUSTER("delete_user", &RouteContactHandler::OnDeleteUser)
END_HANDLER_CLUSTER("")


int RouteContactHandler::OnAddUser(HttpRequest* req, HttpResponse* resp, XmlReader* xml) {
	return 0;
}

int RouteContactHandler::OnUpdateUser(HttpRequest* req, HttpResponse* resp, XmlReader* xml) {
	return 0;
}

int RouteContactHandler::OnDeleteUser(HttpRequest* req, HttpResponse* resp, XmlReader* xml) {
	return 0;
}

BEGIN_HANDLER_CLUSTER(change_external_contact, RouteContactHandler)
	ADD_HANDLER_TO_CLUSTER("add_external_contact", &RouteContactHandler::OnAddExternalContact)
	ADD_HANDLER_TO_CLUSTER("del_external_contact", &RouteContactHandler::OnDelExternalContact)
	ADD_HANDLER_TO_CLUSTER("del_follow_user", &RouteContactHandler::OnDelFollowUser)
END_HANDLER_CLUSTER("")

int RouteContactHandler::OnAddExternalContact(HttpRequest* req, HttpResponse* resp, XmlReader* xml) {
	string userid, external_userid;
	xml->Get("UserID", userid);
	xml->Get("ExternalUserID", external_userid);

	WxResp info;
	info.Parse(WxApi::GetExternalContact(corp_, app_, external_userid));
	string wechat = info.Get("external_contact.name");

	string content = "New External Contact: " + wechat;
	logger::Debug() << content;
	WxApi::SendAppTextMessage(corp_, app_, userid, content);
	return 0;
}

int RouteContactHandler::OnDelExternalContact(HttpRequest* req, HttpResponse* resp, XmlReader* xml) {
	string userid, external_userid;
	xml->Get("UserID", userid);
	xml->Get("ExternalUserID", external_userid);

	WxResp info;
	info.Parse(WxApi::GetExternalContact(corp_, app_, external_userid));
	string wechat = info.Get("external_contact.name");

	string content = "Del External Contact: " + wechat;
	logger::Debug() << content;
	WxApi::SendAppTextMessage(corp_, app_, userid, content);
	return 0;
}

int RouteContactHandler::OnDelFollowUser(HttpRequest* req, HttpResponse* resp, XmlReader* xml) {
	return 0;
}


