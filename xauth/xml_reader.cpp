#include "xml_reader.h"


XmlReader::XmlReader() {
}

XmlReader::XmlReader(const string& data) {
	Parse(data);
}

bool XmlReader::Parse(const string& data) {
	return (XML_SUCCESS == doc_.Parse(data.c_str(), data.size()));
}

int XmlReader::Get(const string& field, string &value) {
	XMLElement* xmlElement = doc_.FirstChildElement("xml");
	if (NULL == xmlElement) {
		return -1;
	}

	XMLElement * msgElement = xmlElement->FirstChildElement(field.c_str());
	if (NULL == msgElement) {
		return -1;
	}

	const char* pMsgBuf = msgElement->GetText();
	if (NULL == pMsgBuf) {
		return -1;
	}

	value = pMsgBuf;
	return 0;
}

