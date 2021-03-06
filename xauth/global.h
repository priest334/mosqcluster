#pragma once

#include <string>
#include "helper/config.h"
#include "helper/logger.h"
#include "db/xdb.h"
#include "db/wrapper.h"
#include "corp_info.h"
#include "helper/thread_pool.h"

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
		template <class _Type>
		Message& operator<<(const _Type& str) {
			hlp::Message* os = dynamic_cast<hlp::Message*>(this);
			(*os) << str;
			return *this;
		}
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

namespace async {
	void Initialize();
	void Cleanup();

	void Execute(ThreadPool::Task* task);
} // namespace async

namespace db {
	void Initialize();
	void Cleanup();

	class SqlWrapper : public hlp::SqlWrapper {
	public:
		SqlWrapper(const string& db);
		SqlWrapper(const string& db, const string& sql);
	};

} // namespace db


namespace app {
	void Initialize();
	void Cleanup();

	CorpInfo* Corp(const string& name);
	string GetAccessToken(const string& corp, const string& app);
	string GetAppParam(const string& corp, const string& app, const string& key);
	string GetJsapiTicket(const string& corp, const string& app);
} // namespace app


