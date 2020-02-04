#include "route_notify.h"

#include "global.h"
#include "server/http_request.h"
#include "wxwork/WXBizMsgCrypt.h"

using std::string;
using namespace Tencent;


int RouteNotify::Process(HttpRequest* req, HttpResponse* resp) {
	resp->SetStatus(HTTP_STATUS_OK);

	return 0;
}
RouteKey RouteNotify::route_key() const {
	return RouteKey(HTTP_POST, "/wxwork/notify");
}




