#include "uuidx.h"

#include <stdio.h>
#ifdef _WIN32
#include <objbase.h>
#else
#include <uuid/uuid.h>
#endif


uuidx::uuidx() {
#ifdef _WIN32
	CoCreateGuid((GUID*)&uuid_);
#else
	uuid_generate((unsigned char*)&uuid_);
#endif // _WIN32
}

string uuidx::str(bool upper/* = true*/) const {
	char str[36 + 1] = { 0 };
	if (upper) {
		sprintf(str, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			uuid_.Data1, uuid_.Data2, uuid_.Data3, uuid_.Data4[0], uuid_.Data4[1],
			uuid_.Data4[2], uuid_.Data4[3], uuid_.Data4[4], uuid_.Data4[5], uuid_.Data4[6], uuid_.Data4[7]);
	} else {
		sprintf(str, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
			uuid_.Data1, uuid_.Data2, uuid_.Data3, uuid_.Data4[0], uuid_.Data4[1],
			uuid_.Data4[2], uuid_.Data4[3], uuid_.Data4[4], uuid_.Data4[5], uuid_.Data4[6], uuid_.Data4[7]);
	}
	return string(str);
}

unsigned char* uuidx::data() {
	return (unsigned char*)&(uuid_);
}

unsigned char* uuidx::data() const {
	return (unsigned char*)&(uuid_);
}

