#pragma once

#include <string>
#include "helper/config.h"
#include "helper/logger.h"
#include "db/xdb.h"
#include "db/wrapper.h"
#include "corp_info.h"

using std::string;

namespace hlp {
	class CommandLine;
}

namespace config {
	void Initialize(hlp::CommandLine* cmdline);
	void Cleanup();
	hlp::Config* Get();
} // namespace config

namespace logger {
	void Initialize();
	void Cleanup();
	hlp::Logger* Get();

	class Message : public hlp::Message {
	public:
		Message(hlp::LogLevel level);
		virtual ~Message();
	};

	class Debug : public Message {
	public:
		Debug();
	};

	class Info : public Message {
	public:
		Info();
	};

	class Warning : public Message {
	public:
		Warning();
	};

	class Error : public Message {
	public:
		Error();
	};

	class Fatal : public Message {
	public:
		Fatal();
	};
} // namespace logger

namespace db {
	void Initialize();
	void Cleanup();

	hlp::Connection* Get();

	class SqlWrapper : public hlp::SqlWrapper {
	public:
		SqlWrapper();
		SqlWrapper(const string& sql);
	};

} // namespace db


namespace app {
	void Initialize();
	void Cleanup();

	CorpInfo* Corp();
	CorpInfo* Corp(const string& name);
	string GetAccessToken(const string& corp, const string& app);
} // namespace app


