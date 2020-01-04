#include "routes.h"
#include "http_parser.h"
#include "http_request.h"
#include "helper/logger.h"


RouteProc::~RouteProc() {
}


RouteKey::RouteKey() : method_(HTTP_GET) {
}

RouteKey::RouteKey(int method, const mstr& url) : method_(method), url_(url) {
}

RouteKey::RouteKey(const RouteKey& src) {
	method_ = src.method_;
	url_ = src.url_;
}

RouteKey& RouteKey::operator=(const RouteKey& src) {
	method_ = src.method_;
	url_ = src.url_;
	return *this;
}

bool RouteKey::operator==(const RouteKey& src) const {
	return (method_ == src.method_ && url_ == src.url_);
}

bool RouteKey::operator<(const RouteKey& src) const {
	if (method_ < src.method_)
		return true;
	if (method_ > src.method_)
		return false;
	return url_ < src.url_;
}

mstr const& RouteKey::url() const {
	return url_;
}


Routes::Routes() {
}


Routes::~Routes() {
}


void Routes::Register(const RouteKey& rkey, RouteProc* rproc) {
	route_table_[rkey] = rproc;
}

void Routes::Register(RouteProc* rproc) {
	Register(rproc->route_key(), rproc);
}


namespace {
	class RouteUnknown : public RouteProc {
	public:
		int Process(HttpRequest* req, HttpResponse* resp) {
			resp->SetStatus(HTTP_STATUS_NOT_FOUND);
			return 0;
		}

		RouteKey route_key() const {
			return RouteKey(-1, "/");
		}
	};

	class RouteHeadOk : public RouteProc {
	public:
		int Process(HttpRequest* req, HttpResponse* resp) {
			resp->SetStatus(HTTP_STATUS_OK);
			return 0;
		}

		RouteKey route_key() const {
			return RouteKey(-1, "/");
		}
	};
}


int Routes::Process(HttpRequest* req, HttpResponse* resp) {
	if (req->method() == HTTP_HEAD) {
		RouteTableConstIter iter = route_table_.begin();
		for (; iter != route_table_.end(); ++iter) {
			if (iter->first.url() == req->url()) {
				RouteHeadOk headok;
				return headok.Process(req, resp);
			}
		}
	} else {
		RouteKey rkey(req->method(), req->url());
		RouteTableConstIter iter = route_table_.find(rkey);
		if (iter != route_table_.end()) {
			RouteProc* rp = iter->second;
			if (rp) {
				return rp->Process(req, resp);
			}
		}
	}

	RouteUnknown unknown;
	return unknown.Process(req, resp);
}

void Routes::InitTable() {
}

