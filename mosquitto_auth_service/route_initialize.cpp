#include "route_initialize.h"

#include <vector>
#include "server/http_request.h"
#include "global.h"
#include "helper/json_wrapper.h"

using std::vector;

int RouteInitialize::Process(HttpRequest* req, HttpResponse* resp) {
	MosqAuthRouteProc::Process(req, resp);

	hlp::JsonDocument data;
	data.Parse(req->body().c_str());
	string action, node;
	if (data.Get("action", action) && data.Get("node", node)) {
		vector<string> nodes;
		service::MosqMgr()->Initialize(node);
		service::MosqMgr()->NodeNames(nodes);

		hlp::JsonDocument doc;
		doc.Set("code", 0);
		doc.Set("message", "success");
		doc.Set("nodes", nodes);
		resp->SetContent(doc.Write(false));
	}

	return 0;
}

RouteKey RouteInitialize::route_key() const {
	return RouteKey(HTTP_POST, "/mosquitto/initialize");
}

