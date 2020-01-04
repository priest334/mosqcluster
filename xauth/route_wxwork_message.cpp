#include "route_wxwork_message.h"

#include "helper/logger.h"
#include "server/http_request.h"
#include "wxwork/WXBizMsgCrypt.h"

using namespace std;
using namespace Tencent;



int GetRouteWxWorkMessage::Process(HttpRequest* req, HttpResponse* resp) {
	resp->SetStatus(HTTP_STATUS_OK);

	string sToken = "CU9fXNiO52V9X3lk4frcLyVJ";
	string sEncodingAESKey = "tDvKQrqzrb2enKBr5yTXrmdi01dXpiJFsJYnAGLEXna";
	string sCorpID = "ww1fb1fce12f2f8ce0";

	string sVerifyMsgSig = req->query("msg_signature").c_str();
	string sVerifyTimeStamp = req->query("timestamp").c_str();
	string sVerifyNonce = req->query("nonce").c_str();
	string sVerifyEchoStr = req->query("echostr").c_str();

	WXBizMsgCrypt wxcpt(sToken, sEncodingAESKey, sCorpID);

	string sEchoStr;
	int ret = wxcpt.VerifyURL(sVerifyMsgSig, sVerifyTimeStamp, sVerifyNonce, sVerifyEchoStr, sEchoStr);
	if (ret != 0) {
		Message(logger::Error)<< "VerifyURL Failed: " << ret;
		return 0;
	}
	Message(logger::Info) << "echo: " << sEchoStr;
	resp->SetContent(sEchoStr);
	return 0;
}
RouteKey GetRouteWxWorkMessage::route_key() const {
	return RouteKey(HTTP_GET, "/wxwork/message");
}


int PostRouteWxWorkMessage::Process(HttpRequest* req, HttpResponse* resp) {
	resp->SetStatus(HTTP_STATUS_OK);

	string sToken = "CU9fXNiO52V9X3lk4frcLyVJ";
	string sEncodingAESKey = "tDvKQrqzrb2enKBr5yTXrmdi01dXpiJFsJYnAGLEXna";
	string sCorpID = "ww1fb1fce12f2f8ce0";

	string sReqMsgSig = req->query("msg_signature").c_str();
	string sReqTimeStamp = req->query("timestamp").c_str();
	string sReqNonce = req->query("nonce").c_str();
	string sReqData = req->body().c_str();

	WXBizMsgCrypt wxcpt(sToken, sEncodingAESKey, sCorpID);

	string sMsg;
	int ret = wxcpt.DecryptMsg(sReqMsgSig, sReqTimeStamp, sReqNonce, sReqData, sMsg);
	if (ret != 0) {
		Message(logger::Error) << "DecryptMsg Failed: " << ret;
		return 0;
	}

	string content;
	if (0 != wxcpt.GetXmlField(sMsg, "Content", content)) {
		Message(logger::Error) << "Data Format Error";
		return 0;
	}
	Message(logger::Info) << "message: " << content;
	return 0;
}
RouteKey PostRouteWxWorkMessage::route_key() const {
	return RouteKey(HTTP_POST, "/wxwork/message");
}



