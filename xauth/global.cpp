#include "global.h"

#include <sstream>
#include <map>

#include "helper/string_helper.h"
#include "helper/command_line.h"
#include "helper/lock_impl_posix.h"
#include "helper/ini_parser.h"
#include "db/connection_pool.h"


using std::ostringstream;
using std::map;

namespace global{
	const int kMaxSize = 128;
	template <class _Type>
	class Manager {	
		typedef map<string, _Type> Instances;
	public:
		typedef typename Instances::const_iterator InstanceIter;

		Manager(bool free_instances = true) : free_instances_(free_instances){
		}

		~Manager() {
			typename Instances::const_iterator iter = instances_.begin();
			for (; iter != instances_.end(); ++iter) {
				if (free_instances_) {
					delete iter->second;
				}
			}
			instances_.clear();
		}

		void Add(const string& name, const _Type& inst) {
			instances_[name] = inst;
		}

		_Type Get(const string& name) {
			typename Instances::const_iterator iter = instances_.find(name);
			if (iter != instances_.end()) {
				return iter->second;
			}
			return NULL;
		}

		InstanceIter begin() const {
			return instances_.begin();
		}

		InstanceIter end() const {
			return instances_.end();
		}
		
	private:
		bool free_instances_;
		Instances instances_;
	};
} // namespace global

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
	namespace {
		class NamedFileLogger : public hlp::Logger {
		public:
			NamedFileLogger(const string& file) 
				: lock_(new LockImplPosix()),
				logfile_(new hlp::SingleFile(file, lock_)) {
				SetStorage(logfile_);
			}
			~NamedFileLogger() {
				delete logfile_;
				delete lock_;
			}

		private:
			LockImplPosix* lock_;
			hlp::SingleFile* logfile_;
		};
	}

	NamedFileLogger* default_logger = NULL;
	void Initialize() {
		string file = config::Get()->Get("logdir", "/var/log/xauth.log");
		default_logger = new NamedFileLogger(file);
	}
	void Cleanup() {
		if (default_logger)
			delete default_logger;
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
	global::Manager<hlp::ConnectionPool*>* manager = NULL;

	void Initialize() {
		manager = new global::Manager<hlp::ConnectionPool*>();
		hlp::String name;
		for (int i = 0; i < global::kMaxSize; i++) {
			int index = i + 1;
			name.Format("DSN.%d_name", index);
			string db = config::Get()->Get(name.str());
			if (db.empty()) {
				break;
			}

			string dsn = config::Get()->Get(name.Format("%s.dsn", db.c_str()).str());
			if (dsn.empty())
				continue;
			int init_size = config::Get()->GetInt(name.Format("%s.init_size", db.c_str()).str(), 10);
			int inc_size = config::Get()->GetInt(name.Format("%s.inc_size", db.c_str()).str(), 2);
			int max_size = config::Get()->GetInt(name.Format("%s.max_size", db.c_str()).str(), 100);
			hlp::ConnectionPool* pool = new hlp::ConnectionPool();
			pool->Init(dsn, init_size, inc_size, max_size);
			manager->Add(db, pool);
		}	
	}

	void Cleanup() {
		if (manager)
			delete manager;
	}

	SqlWrapper::SqlWrapper(const string& db) : hlp::SqlWrapper(manager->Get(db)) {
	}

	SqlWrapper::SqlWrapper(const string& db, const string& sql) : hlp::SqlWrapper(manager->Get(db), sql) {
	}

} // namespace db

namespace app {

	global::Manager<CorpInfo*>* manager = NULL;

	void Initialize() {
		manager = new global::Manager<CorpInfo*>();
		int i = 0;
		hlp::String name, id;
		for (i = 0; i < global::kMaxSize; i++) {
			int index = i + 1;
			name.Format("CorpInfo.%d_name", index);
			id.Format("CorpInfo.%d_corpid", index);
			string corp_name = config::Get()->Get(name.str());
			string corp_id = config::Get()->Get(id.str());
			if (corp_name.empty() || corp_id.empty()) {
				break;
			}
			CorpInfo* corp = new CorpInfo(corp_id);
			manager->Add(corp_name, corp);
		}

		hlp::String corp, appid, secret, token, aeskey;

		global::Manager<CorpInfo*>::InstanceIter iter = manager->begin();
		for (; iter != manager->end(); ++iter) {
			CorpInfo* corp = iter->second;
			string corp_name = iter->first;
			for (int j = 0; j < global::kMaxSize; j++) {
				int index = j + 1;
				name.Format("%s.%d_name", corp_name.c_str(), index);
				appid.Format("%s.%d_appid", corp_name.c_str(), index);
				secret.Format("%s.%d_secret", corp_name.c_str(), index);
				token.Format("%s.%d_callback_token", corp_name.c_str(), index);
				aeskey.Format("%s.%d_callback_aeskey", corp_name.c_str(), index);
				string app_name = config::Get()->Get(name.str());
				string app_id = config::Get()->Get(appid.str());
				string app_secret = config::Get()->Get(secret.str());
				string app_token = config::Get()->Get(token.str());
				string app_aeskey = config::Get()->Get(aeskey.str());

				corp->CreateApplication(app_name, app_id, app_secret);
				corp->SetAppParam(app_name, "callback_token", app_token);
				corp->SetAppParam(app_name, "callback_aeskey", app_aeskey);
			}
		}
	}

	void Cleanup() {
		if (manager)
			delete manager;
	}

	CorpInfo* Corp(const string& name) {
		return manager->Get(name);
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

