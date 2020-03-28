#include "auth_service_routes.h"

#include "route_initialize.h"
#include "route_check_user.h"
#include "route_check_acl.h"


void AuthServiceRoutes::InitTable() {
	Register(new RouteInitialize());
	Register(new RouteCheckUser());
	Register(new RouteCheckAcl());
}

