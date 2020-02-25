#include "route_wxwork_jsapi_ticket.h"

#include <stdlib.h>
#include <stdio.h>
#include "server/http_request.h"
#include "helper/string_helper.h"
#include "helper/json_wrapper.h"
#include "helper/crypto/hash.h"
#include "helper/xtime.h"
#include "helper/uuidx.h"
#include "global.h"
#include "corp_info.h"
#include "wx_work_api.h"



int RouteWxWorkJsapiTicket::Process(HttpRequest* req, HttpResponse* resp) {
	resp->SetStatus(HTTP_STATUS_OK);

	string corp = req->query("corp").c_str();
	string app = req->query("app").c_str();
	string url = req->query("url").c_str();
	if (corp.empty() || app.empty()) {
		logger::Error() << "corp: " << corp << ", app: " << app << " must both not empty";
		return 0;
	}

	CorpInfo* info = app::Corp(corp);

	string corpid = info->CorpId();
	int appid = info->AppId(app);
	string ticket = app::GetJsapiTicket(corp, app);
	if (!ticket.empty()) {
		hlp::JsonDocument doc;
		doc.Parse(ticket);
		string corp_jsapi_ticket, app_jsapi_ticket;
		doc.Get("corp_jsapi_ticket", corp_jsapi_ticket);
		doc.Get("app_jsapi_ticket", app_jsapi_ticket);

		time_t timestamp = hlp::Time::Now().Timestamp();
		string nonce = uuidx().str(), config_signature, agent_config_signature;
		hlp::String temp;	
		temp.Format("jsapi_ticket=%s&noncestr=%s&timestamp=%d&url=%s", corp_jsapi_ticket.c_str(), nonce.c_str(), timestamp, url.c_str());
		config_signature = hlp::crypt::sha1(temp.str());
		temp.Format("jsapi_ticket=%s&noncestr=%s&timestamp=%d&url=%s", app_jsapi_ticket.c_str(), nonce.c_str(), timestamp, url.c_str());
		agent_config_signature = hlp::crypt::sha1(temp.str());

		hlp::JsonDocument odoc;
		odoc.Set("corpid", corpid);
		odoc.Set("appid", appid);
		odoc.Set("nonce", nonce);
		odoc.Set("timestamp", timestamp);
		odoc.Set("config_signature", config_signature);
		odoc.Set("agent_config_signature", config_signature);
		string content = odoc.Write(false);

		resp->SetHeader("Content-Type", "application/json");
		resp->SetContent(content);
	}

	return 0;
}
RouteKey RouteWxWorkJsapiTicket::route_key() const {
	return RouteKey(HTTP_GET, "/wxwork/jsapi_ticket");
}

