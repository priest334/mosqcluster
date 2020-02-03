#include "ini_parser.h"

#include <fstream>

#include "string_helper.h"
#include "config.h"

namespace hlp {
	using std::ifstream;
	using std::ofstream;

	int IniParser::ParseLine(Config* config, const string& line) {
		if (line.empty())
			return -1;
		string str = line;
		StripLeft(str, " \t\r=", false);
		StripRight(str, " \t\r", false);
		if (str.empty() || str[0] == ';')
			return -1;
		string::size_type size = str.length();
		if (size >= 2 && str[0] == '[' && str[size - 1] == ']') {
			if (size == 2) {
				current_section_ = "";
			} else {
				current_section_ = str.substr(1, size - 2);
			}
			return 1;
		}
		
		string key, value;
		string::size_type fpos = str.find('=');
		if (string::npos == fpos) {
			key = str;
		} else {
			key = str.substr(0, fpos);
			value = str.substr(fpos + 1);
			StripRight(key, " \t\r", false);
			StripLeft(value, " \t\r", false);
		}

		String name;
		if (current_section_.empty()) {
			name = key;
		} else {
			name.Format("%s.%s", current_section_.c_str(), key.c_str());
		}
		config->Set(name.str(), value);
		return 0;
	}

	int IniParser::ParseLines(Config* config, const string& lines) {
		string::size_type fpos, offset = 0, total = lines.length();
		do {
			fpos = lines.find('\n', offset);
			if (string::npos != fpos) {
				ParseLine(config, lines.substr(offset, fpos));
				offset = fpos + 1;
			} else {
				ParseLine(config, lines.substr(offset));
			}
		} while (offset < total);
		return 0;
	}

	int IniParser::ParseFile(Config* config, const string& file) {
		if (file.empty())
			return -1;
		ifstream ifs(file.c_str());
		if (ifs.is_open()) {
			string line;
			while (!ifs.eof()) {
				getline(ifs, line);
				if (line.empty())
					continue;
				ParseLine(config, line);
			}
			ifs.close();
		}
		return 0;
	}


	IniParser::IniParser() : crlf_("\n"){
	}

	IniParser::~IniParser() {
	}

	void IniParser::OnArchiveValue(ostringstream& oss, const string& vpath, const string& name, const string& value) {
		if (!vpath.empty() && current_section_.empty()) {
			current_section_ = vpath;
			oss << "[" << current_section_ << "]" << crlf_;
		}
		oss << name << "=" << value << crlf_;
	}

	void IniParser::LoadBuffer(Config* config, const string& buffer) {
		ParseLines(config, buffer);
	}

	void IniParser::LoadFile(Config* config, const string& file) {
		ParseFile(config, file);
	}
	
	string IniParser::ToBuffer(Config* config) {
		ostringstream oss;
		config->Archive(oss);
		return oss.str();
	}

	void IniParser::ToFile(Config* config, const string& file) {
		if (file.empty())
			return;
		ofstream ofs(file.c_str());
		if (ofs.is_open()) {
			ofs << ToBuffer(config) << crlf_;
			ofs.flush();
		}
	}
}