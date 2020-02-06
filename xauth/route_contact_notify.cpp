#include "route_contact_notify.h"

#include "global.h"
#include "server/http_request.h"
#include "wxwork/WXBizMsgCrypt.h"

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
	return 0;
}


