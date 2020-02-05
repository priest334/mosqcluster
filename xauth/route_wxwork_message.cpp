#include "route_wxwork_message.h"

#include "global.h"
#include "server/http_request.h"
#include "wxwork/WXBizMsgCrypt.h"

using std::string;
using namespace Tencent;



int GetRouteWxWorkMessage::Process(HttpRequest* req, HttpResponse* resp) {
	resp->SetStatus(HTTP_STATUS_OK);
	
	string corp = req->query("corp").c_str();
	string app = req->query("app").c_str();
	CorpInfo* info = app::Corp(corp.empty() ? "Main" : corp);
	string sToken = "";
	string sEncodingAESKey = "";
	string sCorpID = "";
	if (info) {
		sToken = info->GetAppParam(app.empty() ? "notify" : app, "callback_token");
		sEncodingAESKey = info->GetAppParam(app.empty() ? "notify" : app, "callback_aeskey");
		sCorpID = info->CorpId();
	}

	string sVerifyMsgSig = req->query("msg_signature").c_str();
	string sVerifyTimeStamp = req->query("timestamp").c_str();
	string sVerifyNonce = req->query("nonce").c_str();
	string sVerifyEchoStr = req->query("echostr").c_str();

	WXBizMsgCrypt wxcpt(sToken, sEncodingAESKey, sCorpID);

	string sEchoStr;
	int ret = wxcpt.VerifyURL(sVerifyMsgSig, sVerifyTimeStamp, sVerifyNonce, sVerifyEchoStr, sEchoStr);
	if (ret != 0) {
		logger::Error() << "VerifyURL Failed: " << ret;
		return 0;
	}
	logger::Info() << "echo: " << sEchoStr;
	resp->SetContent(sEchoStr);
	return 0;
}
RouteKey GetRouteWxWorkMessage::route_key() const {
	return RouteKey(HTTP_GET, "/wxwork/message");
}


int PostRouteWxWorkMessage::Process(HttpRequest* req, HttpResponse* resp) {
	resp->SetStatus(HTTP_STATUS_OK);

	string corp = req->query("corp").c_str();
	string app = req->query("notify").c_str();
	CorpInfo* info = app::Corp(corp.empty() ? "Main" : corp);
	string sToken = "";
	string sEncodingAESKey = "";
	string sCorpID = "";
	if (info) {
		sToken = info->GetAppParam(app.empty() ? "notify" : app, "callback_token");
		sEncodingAESKey = info->GetAppParam(app.empty() ? "notify" : app, "callback_aeskey");
		sCorpID = info->CorpId();
	}

	string sReqMsgSig = req->query("msg_signature").c_str();
	string sReqTimeStamp = req->query("timestamp").c_str();
	string sReqNonce = req->query("nonce").c_str();
	string sReqData = req->body().c_str();

	WXBizMsgCrypt wxcpt(sToken, sEncodingAESKey, sCorpID);

	string sMsg;
	int ret = wxcpt.DecryptMsg(sReqMsgSig, sReqTimeStamp, sReqNonce, sReqData, sMsg);
	if (ret != 0) {
		logger::Error() << "DecryptMsg Failed: " << ret;
		return 0;
	}

	string msgtype;
	if (0 != wxcpt.GetXmlField(sMsg, "MsgType", msgtype)) {
		logger::Error() << "Invalid Message Type: " << msgtype;
		return 0;
	}

	logger::Debug() << "message:\n" << sMsg;

	MsgHandler handler = GET_HANDLER(msgtype, msgtype);
	if (handler != NULL) {
		XmlReader xml(sMsg);
		return (this->*handler)(req, resp, &xml);
	}
	return 0;
}
RouteKey PostRouteWxWorkMessage::route_key() const {
	return RouteKey(HTTP_POST, "/wxwork/message");
}

BEGIN_HANDLER_CLUSTER(msgtype, PostRouteWxWorkMessage)
	ADD_HANDLER_TO_CLUSTER("text", &PostRouteWxWorkMessage::OnRecvText)
	ADD_HANDLER_TO_CLUSTER("image", &PostRouteWxWorkMessage::OnRecvImage)
	ADD_HANDLER_TO_CLUSTER("voice", &PostRouteWxWorkMessage::OnRecvVoice)
	ADD_HANDLER_TO_CLUSTER("video", &PostRouteWxWorkMessage::OnRecvVideo)
	ADD_HANDLER_TO_CLUSTER("location", &PostRouteWxWorkMessage::OnRecvLocation)
END_HANDLER_CLUSTER("")

int PostRouteWxWorkMessage::OnRecvText(HttpRequest* req, HttpResponse* resp, XmlReader* xml) {
	string content;
	if (0 != xml->Get("Content", content)) {
		logger::Error() << "Invalid Text Message";
		return 0;
	}
	logger::Info() << "Content: " << content;
	return 0;
}

int PostRouteWxWorkMessage::OnRecvImage(HttpRequest* req, HttpResponse* resp, XmlReader* xml) {
	string picurl;
	if (0 != xml->Get("PicUrl", picurl)) {
		logger::Error() << "Invalid Image Message";
		return 0;
	}
	logger::Info() << "PicUrl: " << picurl;
	return 0;
}

int PostRouteWxWorkMessage::OnRecvVoice(HttpRequest* req, HttpResponse* resp, XmlReader* xml) {
	string format;
	if (0 != xml->Get("Format", format)) {
		logger::Error() << "Invalid Voice Message";
		return 0;
	}
	logger::Info() << "Format: " << format;
	return 0;
}

int PostRouteWxWorkMessage::OnRecvVideo(HttpRequest* req, HttpResponse* resp, XmlReader* xml) {
	string thumb_media_id;
	if (0 != xml->Get("ThumbMediaId", thumb_media_id)) {
		logger::Error() << "Invalid Voice Message";
		return 0;
	}
	logger::Info() << "ThumbMediaId: " << thumb_media_id;
	return 0;
}

int PostRouteWxWorkMessage::OnRecvLocation(HttpRequest* req, HttpResponse* resp, XmlReader* xml) {
	string location_x, location_y, label;
	if (0 != xml->Get("Location_X", location_x)
		|| 0 != xml->Get("Location_Y", location_x)
		|| 0 != xml->Get("Label", label)) {
		logger::Error() << "Invalid Location Message";
		return 0;
	}
	logger::Info() << "Location: " << label;
	return 0;
}


