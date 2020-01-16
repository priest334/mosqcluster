#include "global.h"

#include <sstream>

#include "helper/string_helper.h"
#include "helper/command_line.h"
#include "helper/lock_impl_posix.h"
#include "helper/ini_parser.h"
#include "db/connection_pool.h"


using std::ostringstream;

namespace config {
	hlp::Config* default_config = NULL;
	void Initialize(hlp::CommandLine* cmdline) {
		ostringstream oss;
		oss << cmdline->GetSwitchValueWithDefault("config", "./xauth.conf");
		default_config = new hlp::Config();
		default_config->LoadFile(oss.str());
	}

	void Cleanup() {
		delete default_config;
		default_config = NULL;
	}

	hlp::Config* Get() {
		return default_config;
	}

} // namespace config

namespace logger {
	hlp::Logger* default_logger = NULL;
	hlp::SingleFile* default_logfile = NULL;
	LockImplPosix* default_logfile_lock = NULL;

	void Initialize() {
		string file = config::Get()->Get("logdir", "/var/log/xauth.log");
		default_logger = new hlp::Logger();
		default_logfile_lock = new LockImplPosix();
		default_logfile = new hlp::SingleFile(file, default_logfile_lock);
		default_logger->SetStorage(default_logfile);
	}
	void Cleanup() {
		if (default_logger) {
			delete default_logger;
			default_logger = NULL;
		}
		if (default_logfile) {
			delete default_logfile;
			default_logfile = NULL;
		}
		if (default_logfile_lock) {
			delete default_logfile_lock;
			default_logfile_lock = NULL;
		}
	}

	hlp::Logger* Get() {
		return default_logger;
	}

	Message::Message(hlp::LogLevel level) : hlp::Message(level, Get()) {
	}

	Message::~Message() {
	}

	Debug::Debug() : Message(hlp::Debug) {}
	Info::Info() : Message(hlp::Info) {}
	Warning::Warning() : Message(hlp::Warning) {}
	Error::Error() : Message(hlp::Error) {}
	Fatal::Fatal() : Message(hlp::Fatal) {}

} // namespace logger

namespace db {
	hlp::ConnectionPool* workos_conn_pool = NULL;

	void Initialize() {
		string dsn = config::Get()->Get("WorkOS.dsn");
		if (!dsn.empty()) {
			int init_size = config::Get()->GetInt("WorkOS.init_size", 10);
			int inc_size = config::Get()->GetInt("WorkOS.inc_size", 2);
			int max_size = config::Get()->GetInt("WorkOS.max_size.", 100);
			workos_conn_pool = new hlp::ConnectionPool();
			workos_conn_pool->Init(dsn, init_size, inc_size, max_size);
		}		
	}

	void Cleanup() {
		delete workos_conn_pool;
	}

	SqlWrapper::SqlWrapper() : hlp::SqlWrapper(workos_conn_pool) {
	}

	SqlWrapper::SqlWrapper(const string& sql) : hlp::SqlWrapper(workos_conn_pool, sql) {
	}

} // namespace db

namespace app {

	namespace {
		class NamedCorpInfo : public CorpInfo {
		public:
			NamedCorpInfo() {
			}

			NamedCorpInfo(const string& corpid, const string& name) : CorpInfo(corpid), name_(name) {
			}

			bool HasName(const string& name) const {
				return name_ == name;
			}

			void SetName(const string& name) {
				name_ = name;
			}

			string Name() const {
				return name_;
			}

			void Attach(const string& corpid, const string& name) {
				SetCorpId(corpid);
				name_ = name;
			}
		private:
			string name_;
		};
	}

	NamedCorpInfo* corp_info = NULL;
	int corp_info_count = 0;

	void Initialize() {
		const int kMaxSize = 128;
		int i = 0;
		corp_info = new NamedCorpInfo[kMaxSize];
		hlp::String name, id;
		for (i = 0; i < kMaxSize; i++) {
			name.Format("CorpInfo.%d_name", i);
			id.Format("CorpInfo.%d_corpid", i);
			string corp_name = config::Get()->Get(name.str());
			string corp_id = config::Get()->Get(id.str());
			if (corp_name.empty() || corp_id.empty()) {
				break;
			}
			corp_info[corp_info_count++].Attach(corp_id, corp_name);
		}

		hlp::String corp, appid, secret, token, aeskey;
		for (i = 0; i < corp_info_count; i++) {
			for (int j = 0; j < kMaxSize; j++) {
				name.Format("%s.%d_name", corp_info[i].Name().c_str(), j);
				appid.Format("%s.%d_appid", corp_info[i].Name().c_str(), j);
				secret.Format("%s.%d_secret", corp_info[i].Name().c_str(), j);
				token.Format("%s.%d_callback_token", corp_info[i].Name().c_str(), j);
				aeskey.Format("%s.%d_callback_aeskey", corp_info[i].Name().c_str(), j);
				string app_name = config::Get()->Get(name.str());
				string app_id = config::Get()->Get(appid.str());
				string app_secret = config::Get()->Get(secret.str());
				string app_token = config::Get()->Get(token.str());
				string app_aeskey = config::Get()->Get(aeskey.str());
				corp_info[i].CreateApplication(app_name, app_id, app_secret);
				corp_info[i].SetAppParam(app_name, "callback_token", app_token);
				corp_info[i].SetAppParam(app_name, "callback_aeskey", app_aeskey);
			}
		}
	}

	void Cleanup() {
		delete[] corp_info;
	}

	CorpInfo* Corp() {
		return &corp_info[0];
	}

	CorpInfo* Corp(const string& name) {
		for (int i = 0; i < corp_info_count; i++) {
			if (corp_info[i].HasName(name))
				return &corp_info[i];
		}
		return NULL;
	}

	string GetAccessToken(const string& corp, const string& app) {
		CorpInfo* info = Corp(corp);
		if (info) {
			return info->GetAccessToken(app);
		}
		return string();
	}

	string GetAppParam(const string& corp, const string& app, const string& key) {
		CorpInfo* info = Corp(corp);
		if (info) {
			return info->GetAppParam(app, key);
		}
		return string();
	}
} // namespace app

