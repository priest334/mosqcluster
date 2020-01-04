#pragma once

#include <string>
#include <map>

#include "helper/xstring.h"

//using std::mstr;
typedef hlp::xstring mstr;
using std::map;

class HttpRequest;
class HttpResponse;

class RouteKey {
public:
	RouteKey();
	RouteKey(int method, const mstr& url);
	RouteKey(const RouteKey& src);
	RouteKey& operator=(const RouteKey& src);

	bool operator==(const RouteKey& src) const;
	bool operator<(const RouteKey& src) const;

	mstr const& url() const;

private:
	int method_;
	mstr url_;
};

class RouteProc {
public:
	virtual ~RouteProc();
	virtual int Process(HttpRequest* req, HttpResponse* resp) = 0;
	virtual RouteKey route_key() const = 0;
};

class Routes {
	typedef map<RouteKey, RouteProc*> RouteTable;
	typedef RouteTable::iterator RouteTableIter;
	typedef RouteTable::const_iterator RouteTableConstIter;
public:
	Routes();
	virtual ~Routes();
	
	void Register(const RouteKey& rkey, RouteProc* rproc);
	void Register(RouteProc* rproc);
	int Process(HttpRequest* req, HttpResponse* resp);

	virtual void InitTable();

private:
	RouteTable route_table_;
};


