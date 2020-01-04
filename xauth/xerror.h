#pragma once

#ifndef __XERROR_H__
#define __XERROR_H__

#include <string>

typedef int xerr_t;

namespace xerr {
	enum Error {
#define XERROR_DEFINE(lable, value, msg) k ## lable = value,
#include "xerror_list.h"
#undef XERROR_DEFINE
	};
}

std::string ErrorMessage(xerr_t error);



#endif // __XERROR_H__



