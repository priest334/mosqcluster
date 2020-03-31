#include "command_line.h"

#include <stdlib.h>
#include <string.h>

namespace hlp {
	CommandLine::CommandLine() {
	}

	CommandLine::CommandLine(int argc, char* argv[]) {
		FromArgs(argc, argv);
	}

	CommandLine::~CommandLine() {
	}

	void CommandLine::FromArgs(int argc, char* argv[]) {
		application_ = argv[0];
		for (int i = 1; i < argc; i++) {
			char* argp = argv[i];
			for (char ch = *argp; ch == '-'; ch = *++argp) {
			}
			char* sep = strchr(argp, '=');
			if (sep) {
				string name(argp, sep - argp);
				string value(sep + 1);
				switches_[name] = value;
			} else {
				switches_[argp] = "";
			}
		}
	}

	bool CommandLine::HasSwitch(const string& name) {
		return (switches_.end() != switches_.find(name));
	}

	string CommandLine::GetSwitchValue(const string& name) {
		string value;
		map<string, string>::const_iterator iter = switches_.find(name);
		if (iter != switches_.end()) {
			value = iter->second;
		}
		return value;
	}

	string CommandLine::GetSwitchValueWithDefault(const string& name, const string& default_value) {
		string value = GetSwitchValue(name);
		if (value.empty()) {
			return default_value;
		}
		return value;
	}

	int CommandLine::GetSwitchIntValue(const string& name, int default_value/* = 0*/) {
		string value = GetSwitchValue(name);
		if (value.empty()) {
			return default_value;
		}
		return atoi(value.c_str());
	}

} // namespace hlp


