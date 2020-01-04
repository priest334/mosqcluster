#include "xerror.h"


namespace {
	struct XAuthError {
		int code_;
		const char* msg_;
	};

	struct XAuthError xauth_error_list[] = {
#define XERROR_DEFINE(label, value, msg) {-(value), msg},
#include "xerror_list.h"
#undef XERROR_DEFINE
		{0x7fffffff, "Unknown Error"}
	};
}


std::string ErrorMessage(xerr_t error) {
	int code = -error;
	int total = sizeof(xauth_error_list) / sizeof(xauth_error_list[0]);
	for (int i = 0; i < total; i++) {
		if (code == xauth_error_list[i].code_) {
			return xauth_error_list[code].msg_;
		}
	}
	return xauth_error_list[total - 1].msg_;
}

