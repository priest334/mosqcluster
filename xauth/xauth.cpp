#include "xauth.h"

#include "route_new_token.h"
#include "route_wxwork_message.h"
#include "route_notify.h"
#include "route_contact_notify.h"


void XAuthRoutes::InitTable() {
	Register(new RouteNewToken());
	Register(new RouteWxWorkMessageVerify());
	Register(new RouteWxWorkMessageHandler());
	Register(new RouteNotify());
	Register(new RouteContactVerify());
	Register(new RouteContactHandler());
}