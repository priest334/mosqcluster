#pragma once

#include <string>
#include <stdint.h>
#include "tinyxml2/tinyxml2.h"

using std::string;
using namespace tinyxml2;

class XmlReader {
public:
	XmlReader();
	XmlReader(const string& data);

	bool Parse(const string& data);
	int Get(const string& field, string &value);
private:
	XMLDocument doc_;
};

