#include "route_check_user.h"

#include "server/http_request.h"
#include "global.h"
#include "helper/json_wrapper.h"
#include "helper/local_stream.h"


int RouteCheckUser::Process(HttpRequest* req, HttpResponse* resp) {
	resp->SetStatus(HTTP_STATUS_OK);

	bool granted = false;
	hlp::JsonDocument data, doc;
	data.Parse(req->body().c_str());
	string name, clientid, username, password;
	if (data.Get("name", name) 
		&& data.Get("clientid", clientid) 
		&& data.Get("username", username) 
		&& data.Get("password", password)) {
		granted = service::MosqMgr()->CheckUser(name, clientid, username, password);
	}

	doc.Set("code", granted ? 0 : -1);
	doc.Set("message", granted ? "success" : "failed");
	resp->SetHeader("Content-Type", "application/json");
	resp->SetContent(doc.Write(false));

	return 0;
}
RouteKey RouteCheckUser::route_key() const {
	return RouteKey(HTTP_GET, "/mosquitto/checkuser");
}

