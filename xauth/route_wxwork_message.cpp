#include "route_wxwork_message.h"

#include "global.h"
#include "server/http_request.h"
#include "wxwork/WXBizMsgCrypt.h"

using std::string;
using namespace Tencent;

RouteKey RouteWxWorkMessageVerify::route_key() const {
	return RouteKey(HTTP_GET, "/wxwork/message");
}

RouteKey RouteWxWorkMessageHandler::route_key() const {
	return RouteKey(HTTP_POST, "/wxwork/message");
}

int RouteWxWorkMessageHandler::HandleMessage(HttpRequest* req, HttpResponse* resp, WXBizMsgCrypt* wxbmc, XmlReader* xml) {
	string msgtype;
	if (0 != xml->Get("MsgType", msgtype)) {
		logger::Error() << "Invalid Message Type: " << msgtype;
		return 0;
	}

	MsgHandler handler = GET_HANDLER(msgtype, msgtype);
	if (handler != NULL) {
		return (this->*handler)(req, resp, xml);
	}
	return 0;
}

BEGIN_HANDLER_CLUSTER(msgtype, RouteWxWorkMessageHandler)
	ADD_HANDLER_TO_CLUSTER("text", &RouteWxWorkMessageHandler::OnRecvText)
	ADD_HANDLER_TO_CLUSTER("image", &RouteWxWorkMessageHandler::OnRecvImage)
	ADD_HANDLER_TO_CLUSTER("voice", &RouteWxWorkMessageHandler::OnRecvVoice)
	ADD_HANDLER_TO_CLUSTER("video", &RouteWxWorkMessageHandler::OnRecvVideo)
	ADD_HANDLER_TO_CLUSTER("location", &RouteWxWorkMessageHandler::OnRecvLocation)
END_HANDLER_CLUSTER("")

int RouteWxWorkMessageHandler::OnRecvText(HttpRequest* req, HttpResponse* resp, XmlReader* xml) {
	string content;
	if (0 != xml->Get("Content", content)) {
		logger::Error() << "Invalid Text Message";
		return 0;
	}
	logger::Info() << "Content: " << content;
	return 0;
}

int RouteWxWorkMessageHandler::OnRecvImage(HttpRequest* req, HttpResponse* resp, XmlReader* xml) {
	string picurl;
	if (0 != xml->Get("PicUrl", picurl)) {
		logger::Error() << "Invalid Image Message";
		return 0;
	}
	logger::Info() << "PicUrl: " << picurl;
	return 0;
}

int RouteWxWorkMessageHandler::OnRecvVoice(HttpRequest* req, HttpResponse* resp, XmlReader* xml) {
	string format;
	if (0 != xml->Get("Format", format)) {
		logger::Error() << "Invalid Voice Message";
		return 0;
	}
	logger::Info() << "Format: " << format;
	return 0;
}

int RouteWxWorkMessageHandler::OnRecvVideo(HttpRequest* req, HttpResponse* resp, XmlReader* xml) {
	string thumb_media_id;
	if (0 != xml->Get("ThumbMediaId", thumb_media_id)) {
		logger::Error() << "Invalid Voice Message";
		return 0;
	}
	logger::Info() << "ThumbMediaId: " << thumb_media_id;
	return 0;
}

int RouteWxWorkMessageHandler::OnRecvLocation(HttpRequest* req, HttpResponse* resp, XmlReader* xml) {
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


