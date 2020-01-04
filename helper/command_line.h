#pragma once

#ifndef __BASE_COMMAND_LINE_H__
#define __BASE_COMMAND_LINE_H__

#include <string>
#include <map>

namespace hlp {
	using std::string;
	using std::map;

	class CommandLine
	{
	public:
		CommandLine();
		CommandLine(int argc, char* argv[]);
		~CommandLine();

		void FromArgs(int argc, char* argv[]);

		bool HasSwitch(const string& name);
		string GetSwitchValue(const string& name);
		string GetSwitchValueWithDefault(const string& name, const string& default_value);
		int GetSwitchIntValue(const string& name, int default_value = 0);

	private:
		string application_;
		map<string, string> switches_;
	};
} // namespace hlp


#endif // __BASE_COMMAND_LINE_H__


