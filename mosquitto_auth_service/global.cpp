#include "global.h"

#include <sstream>
#include <map>

#include "helper/string_helper.h"
#include "helper/command_line.h"
#include "helper/lock_impl_posix.h"
#include "helper/ini_parser.h"


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

namespace async {
	ThreadPool* worker_thread_pool = NULL;
	void Initialize() {
		worker_thread_pool = new ThreadPool();
		worker_thread_pool->Create(20);
	}

	void Cleanup() {
		worker_thread_pool->Stop();
		worker_thread_pool->Wait();
		delete worker_thread_pool;
	}

	void Execute(ThreadPool::Task* task) {
		worker_thread_pool->Push(task);
	}
} // namespace async

namespace service {
	MosquittoContextManager* manager = NULL;
	void Initialize() {
		string username, password, prefix;
		username = config::Get()->Get("mosq_trust_username");
		password = config::Get()->Get("mosq_trust_password");
		prefix = config::Get()->Get("mosq_trust_prefix");
		manager = new MosquittoContextManager(username, password, prefix);
	}
	void Cleanup() {
		delete manager;
	}

	MosquittoContextManager* MosqMgr() {
		return manager;
	}
} // namespace service

