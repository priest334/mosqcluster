#include "xauth.h"

#include "route_new_token.h"
#include "route_wxwork_message.h"
#include "route_notify.h"


void XAuthRoutes::InitTable() {
	Register(new RouteNewToken());
	Register(new GetRouteWxWorkMessage());
	Register(new PostRouteWxWorkMessage());
	Register(new RouteNotify());
}