#pragma once

#include <string>
using std::string;

typedef struct _UUID {
	unsigned int Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char Data4[8];
} UUID;

class uuidx {
public:
	uuidx();
	string str(bool upper = true) const;
	unsigned char* data();
	unsigned char* data() const;
private:
	UUID uuid_;
};

