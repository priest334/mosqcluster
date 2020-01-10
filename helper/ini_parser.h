#pragma once


#include "config.h"

namespace hlp {
	class IniParser : public ConfigParser {
		int ParseLine(Config* config, const string& line);
		int ParseLines(Config* config, const string& lines);
		int ParseFile(Config* config, const string& file);
	public:
		IniParser();
		~IniParser();

		void OnArchiveValue(ostringstream& oss, const string& vpath, const string& name, const string& value);
		void LoadBuffer(Config* config, const string& buffer);
		void LoadFile(Config* config, const string& file);
		string ToBuffer(Config* config);
		void ToFile(Config* config, const string& file);

	private:
		string current_section_;
		string crlf_;
	};
}


