#include "json_wrapper.h"

#include "json/json.h"


namespace hlp {

	bool JsonDocument::Get(const Json::Value& parent, const string& key, Json::Value& value) const {
		string::size_type fpos = key.find('.');
		if (fpos == string::npos) {
			if (!parent.isMember(key))
				return false;
			value = parent[key];
			return true;
		} else {
			string pkey = key.substr(0, fpos);
			if (!parent.isMember(pkey))
				return false;
			string subkey = key.substr(fpos + 1);
			return Get(parent[pkey], subkey, value);
		}
	}

	void JsonDocument::Set(Json::Value& parent, const string& key, const Json::Value& value) {
		string::size_type fpos = key.find('.');
		if (fpos == string::npos) {
			parent[key] = value;
		} else {
			string pkey = key.substr(0, fpos);
			if (!parent.isMember(pkey) || !parent[pkey].isObject()) {
				parent[pkey] = Json::Value(Json::objectValue);
			}
			string subkey = key.substr(fpos + 1);
			return Set(parent[pkey], subkey, value);
		}
	}

	void JsonDocument::Append(Json::Value& parent, const string& key, const Json::Value& value) {
		string::size_type fpos = key.find('.');
		if (fpos == string::npos) {
			if (!parent.isMember(key) || !parent[key].isArray()) {
				parent[key] = Json::Value(Json::arrayValue);
			}
			parent[key].append(value);
		} else {
			string pkey = key.substr(0, fpos);
			if (!parent.isMember(pkey) || !parent[pkey].isObject()) {
				parent[pkey] = Json::Value(Json::objectValue);
			}
			string subkey = key.substr(fpos + 1);
			return Append(parent[pkey], subkey, value);
		}
	}

	JsonDocument::JsonDocument() : root_(Json::objectValue) {
	}

	bool JsonDocument::Parse(const string& doc) {
		Json::Reader reader;
		return reader.parse(doc.c_str(), root_);
	}

	string JsonDocument::Write(bool pretty/* = true*/) const {
		Json::StreamWriterBuilder wbuilder;
		wbuilder["commentStyle"] = "None";
		if (pretty) {
			wbuilder["indentation"] = "\t";
		} else {
			wbuilder["indentation"] = "";
		}
		return Json::writeString(wbuilder, root_);
	}

	bool JsonDocument::Get(const string& key, int& value) const {
		Json::Value v;
		if (Get(root_, key, v) && v.isInt()) {
			value = v.asInt();
			return true;
		}
		return false;
	}

	bool JsonDocument::Get(const string& key, string& value) const {
		Json::Value v;
		if (Get(root_, key, v) && v.isString()) {
			value = v.asString();
			return true;
		}
		return false;
	}

	bool JsonDocument::Get(const string& key, vector<int>& value) const {
		Json::Value v;
		if (Get(root_, key, v) && v.isArray()) {
			Json::ValueIterator iter = v.begin();
			for (; iter != v.end(); ++iter) {
				if (iter->isInt()) {
					value.push_back(iter->asInt());
				}
			}
			return true;
		}
		return false;
	}

	bool JsonDocument::Get(const string& key, vector<string>& value) const {
		Json::Value v;
		if (Get(root_, key, v) && v.isArray()) {
			Json::ValueIterator iter = v.begin();
			for (; iter != v.end(); ++iter) {
				if (iter->isString()) {
					value.push_back(iter->asString());
				}
			}
			return true;
		}
		return false;
	}

	void JsonDocument::Set(const string& key, int value) {
		Set(root_, key, Json::Value(value));
	}

	void JsonDocument::Set(const string& key, const string& value) {
		Set(root_, key, Json::Value(value));
	}

	void JsonDocument::Set(const string& key, const vector<int>& value) {
		Json::Value arr(Json::arrayValue);	
		vector<int>::const_iterator iter = value.begin();
		for (; iter != value.end(); ++iter) {
			arr.append(*iter);
		}
		Set(root_, key, arr);
	}

	void JsonDocument::Set(const string& key, const vector<string>& value) {
		Json::Value arr(Json::arrayValue);
		vector<string>::const_iterator iter = value.begin();
		for (; iter != value.end(); ++iter) {
			arr.append(*iter);
		}
		Set(root_, key, arr);
	}

	void JsonDocument::Append(const string& key, int value) {
		Append(root_, key, Json::Value(value));
	}

	void JsonDocument::Append(const string& key, const string& value) {
		Append(root_, key, Json::Value(value));
	}
}


