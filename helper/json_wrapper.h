#pragma once

#include <string>
#include <vector>
#include "json/json.h"

namespace hlp {
	using std::string;
	using std::vector;

	class JsonDocument {
		bool Get(const Json::Value& parent, const string& key, Json::Value& value) const;
		void Set(Json::Value& parent, const string& key, const Json::Value& value);
		void Append(Json::Value& parent, const string& key, const Json::Value& value);
	public:
		JsonDocument();
		bool Parse(const string& doc);
		string Write(bool pretty = true) const;

		bool Get(const string& key, int& value) const;
		bool Get(const string& key, string& value) const;
		bool Get(const string& key, vector<int>& value) const;
		bool Get(const string& key, vector<string>& value) const;
		
		void Set(const string& key, int value);
		void Set(const string& key, const string& value);
		void Set(const string& key, const vector<int>& value);
		void Set(const string& key, const vector<string>& value);

		void Append(const string& key, int value);
		void Append(const string& key, const string& value);

	private:
		Json::Value root_;
	};


}


