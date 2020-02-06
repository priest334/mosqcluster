#include "route_callback.h"

#include "global.h"
#include "server/http_request.h"
#include "wxwork/WXBizMsgCrypt.h"

using std::string;
using namespace Tencent;

RouteCallback::~RouteCallback() {
}

RouteCallback::RouteCallback() : corp_("Main"), app_("notify") {
}

RouteCallback::RouteCallback(const string& corp, const string& app)
	: corp_(corp), app_(app) {
}

int RouteCallback::Process(HttpRequest* req, HttpResponse* resp) {
	resp->SetStatus(HTTP_STATUS_OK);

	CorpInfo* info = app::Corp(corp_);
	string sToken, sEncodingAESKey, sCorpID;
	if (info) {
		sToken = info->GetAppParam(app_, "callback_token");
		sEncodingAESKey = info->GetAppParam(app_, "callback_aeskey");
		sCorpID = info->CorpId();
	}

	WXBizMsgCrypt wxcpt(sToken, sEncodingAESKey, sCorpID);
	return OnMessage(req, resp, &wxcpt);
}



RouteCallbackVerify::~RouteCallbackVerify() {
}

RouteCallbackVerify::RouteCallbackVerify() {
}

RouteCallbackVerify::RouteCallbackVerify(const string& corp, const string& app)
	: RouteCallback(corp, app) {
}

int RouteCallbackVerify::OnMessage(HttpRequest* req, HttpResponse* resp, Tencent::WXBizMsgCrypt* wxbmc) {
	resp->SetStatus(HTTP_STATUS_OK);

	string msgsig = req->query("msg_signature").c_str();
	string timestamp = req->query("timestamp").c_str();
	string nonce = req->query("nonce").c_str();
	string echostr = req->query("echostr").c_str();

	string echo;
	int ret = wxbmc->VerifyURL(msgsig, timestamp, nonce, echostr, echo);
	if (ret != 0) {
		logger::Error() << "VerifyURL Failed: (" << corp_ << "," << app_ << ") " << ret;
		return 0;
	}
	resp->SetContent(echo);
	return 0;
}


RouteCallbackHandler::~RouteCallbackHandler() {
}

RouteCallbackHandler::RouteCallbackHandler() {
}

RouteCallbackHandler::RouteCallbackHandler(const string& corp, const string& app)
	: RouteCallback(corp, app) {
}

int RouteCallbackHandler::OnMessage(HttpRequest* req, HttpResponse* resp, Tencent::WXBizMsgCrypt* wxbmc) {
	resp->SetStatus(HTTP_STATUS_OK);

	string msgsig = req->query("msg_signature").c_str();
	string timestamp = req->query("timestamp").c_str();
	string nonce = req->query("nonce").c_str();
	string content = req->body().c_str();

	string message;
	int ret = wxbmc->DecryptMsg(msgsig, timestamp, nonce, content, message);
	if (ret != 0) {
		logger::Error() << "DecryptMsg Failed: (" << corp_ << "," << app_ << ") " << ret;
		return 0;
	}

	logger::Debug() << "message: " << message;

	XmlReader xml(message);
	return HandleMessage(req, resp, wxbmc, &xml);
}


