#include "route_check_acl.h"

#include "server/http_request.h"
#include "global.h"
#include "helper/json_wrapper.h"
#include "helper/local_stream.h"


int RouteCheckAcl::Process(HttpRequest* req, HttpResponse* resp) {
	resp->SetStatus(HTTP_STATUS_OK);

	bool granted = false;
	hlp::JsonDocument data, doc;
	data.Parse(req->body().c_str());

	int access = -1;
	string name, clientid, topic;
	if (data.Get("name", name)
		&& data.Get("clientid", clientid)
		&& data.Get("access", access)
		&& data.Get("topic", topic)) {
		granted = service::MosqMgr()->CheckAcl(name, clientid, access, topic);
	}

	doc.Set("code", granted ? 0 : -1);
	doc.Set("message", granted ? "success" : "failed");
	resp->SetHeader("Content-Type", "application/json");
	resp->SetContent(doc.Write(false));

	return 0;
}
RouteKey RouteCheckAcl::route_key() const {
	return RouteKey(HTTP_GET, "/mosquitto/checkacl");
}

