#include "config.h"

#include <stdlib.h>

#include "ini_parser.h"

namespace hlp {
	namespace conf {
		char Value::kSep = '.';

		string Value::Get(const string& name, bool& exist) const {
			exist = false;
			string::size_type fpos = name.find(kSep);
			if (fpos != string::npos && (fpos + 2) < name.length()) {
				string prop = name.substr(0, fpos);
				map<string, Value>::const_iterator iter = child_.find(prop);
				if (iter != child_.end()) {
					return iter->second.Get(name.substr(fpos + 1), exist);
				}
				return string();
			} else {
				map<string, string>::const_iterator iter = prop_.find(name);
				if (iter != prop_.end()) {
					exist = true;
					return iter->second;
				}
				return string();
			}
		}

		string Value::Get(const string& name) const {
			bool exist = false;
			return Get(name, exist);
		}

		string Value::Get(const string& name, const string& default_value) const {
			string value = Get(name);
			if (value.empty())
				return default_value;
			return value;
		}

		Int64 Value::GetInt(const string& name) const {
			string value = Get(name);
			if (value.empty())
				return 0;
			return atoll(value.c_str());
		}

		Int64 Value::GetInt(const string& name, Int64 default_value) {
			string value = Get(name);
			if (value.empty())
				return default_value;
			return atoll(value.c_str());
		}

		void Value::Set(const string& name, const string& value) {
			string::size_type fpos = name.find('.');
			if (fpos != string::npos && (fpos + 2) < name.length()) {
				string prefix = name.substr(0, fpos);
				string prop = name.substr(fpos + 1);
				map<string, Value>::iterator iter = child_.find(prefix);
				if (iter != child_.end()) {
					iter->second.Set(prop, value);
				} else {
					Value child;
					child.Set(prop, value);
					child_[prefix] = child;
				}
			} else {
				prop_[name] = value;
			}
		}

		void Value::SetInt(const string& name, Int64 value) {
			ostringstream oss;
			oss << value;
			Set(name, oss.str());
		}

		void Value::Archive(ostringstream& oss, const string& vpath, ArchiveCallback callback, void* argp) const {
			map<string, string>::const_iterator iter = prop_.begin();
			for (; iter != prop_.end(); ++iter) {
				callback(oss, vpath, iter->first, iter->second, argp);
			}
			map<string, Value>::const_iterator child_iter = child_.begin();
			for (; child_iter != child_.end(); ++child_iter) {
				ostringstream osspath;
				if (vpath.empty())
					osspath << child_iter->first;
				else
					osspath << vpath << "." << child_iter->first;
				child_iter->second.Archive(oss, osspath.str(), callback, argp);
			}
		}
	}

	Config::Config() : parser_(new IniParser()), delete_parser_(1){
	}

	Config::Config(ConfigParser* parser, int delete_parser/* = 0*/) : parser_(parser), delete_parser_(delete_parser){
	}

	Config::~Config() {
		if (delete_parser_) {
			delete parser_;
		}
	}

	void Config::LoadBuffer(const string& buffer) {
		parser_->LoadBuffer(this, buffer);
	}

	void Config::LoadFile(const string& file) {
		parser_->LoadFile(this, file);
	}

	string Config::ToBuffer() {
		return parser_->ToBuffer(this);
	}

	void Config::ToFile(const string& file) {
		parser_->ToFile(this, file);
	}

	string Config::Get(const string& name) const {
		return root_.Get(name);	
	}

	string Config::Get(const string& name, const string& default_value) const {
		return root_.Get(name, default_value);
	}

	Int64 Config::GetInt(const string& name) const {
		return root_.GetInt(name);
	}

	Int64 Config::GetInt(const string& name, Int64 default_value) {
		return root_.GetInt(name, default_value);
	}

	void Config::Set(const string& name, const string& value) {
		root_.Set(name, value);
	}

	void Config::SetInt(const string& name, Int64 value) {
		root_.SetInt(name, value);
	}

	int OnArchiveValueInteral(ostringstream& oss, const string& vpath, const string& name, const string& value, void* argp) {
		Config* config = reinterpret_cast<Config*>(argp);
		config->OnArchiveValue(oss, vpath, name, value);
		return 0;
	}

	void Config::Archive(ostringstream& oss) {
		root_.Archive(oss, "", OnArchiveValueInteral, this);
	}

	void Config::OnArchiveValue(ostringstream& oss, const string& vpath, const string& name, const string& value) {
		parser_->OnArchiveValue(oss, vpath, name, value);
	}


	ConfigParser::~ConfigParser() {
	}

} // namespace hlp



