#pragma once

#include <string>
#include <set>
#include <vector>
#include "rapidjson/document.h"

namespace rpj = rapidjson;

class JsonWrapper {
public:
	JsonWrapper();
	~JsonWrapper();

	bool Parse(const char* str);

	bool Get(const char* key, bool& value);
	bool GetEx(const char* key, bool& value, char septor = '.');
	bool Get(const char* key, int& value);
	bool GetEx(const char* key, int& value, char septor = '.');
	bool Get(const char* key, double& value);
	bool GetEx(const char* key, double& value, char septor = '.');
	bool Get(const char* key, std::string& value);
	bool GetEx(const char* key, std::string& value, char septor = '.');
	bool Get(const char* key, std::set<int>& value);
	bool GetEx(const char* key, std::set<int>& value, char septor = '.');
	bool Get(const char* key, std::set<double>& value);
	bool GetEx(const char* key, std::set<double>& value, char septor = '.');
	bool Get(const char* key, std::set<std::string>& value);
	bool GetEx(const char* key, std::set<std::string>& value, char septor = '.');
	bool Get(const char* key, std::vector<int>& value);
	bool GetEx(const char* key, std::vector<int>& value, char septor = '.');
	bool Get(const char* key, std::vector<double>& value);
	bool GetEx(const char* key, std::vector<double>& value, char septor = '.');
	bool Get(const char* key, std::vector<std::string>& value);
	bool GetEx(const char* key, std::vector<std::string>& value, char septor = '.');

private:
	char* GetKey(char*& key, bool& cancel, char septor = '.');
	rpj::Value& GetChild(rpj::Value& parent, char* key, bool& find, char septor = '.');
	rpj::Value& GetValue(const char* key, bool& find);
	rpj::Value& GetValueEx(char* key, bool& find, char septor = '.');

	rpj::Document doc_;
	rpj::Value null_;

};

