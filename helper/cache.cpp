#include "cache.h"

#include "cache/native_memory.h"
#include "cache/native_file.h"
#include "lock.h"

namespace hlp {

	SimpleCacheEngineFactory::~SimpleCacheEngineFactory() {
	}

	SimpleCacheEngine* SimpleCacheEngineFactory::Create(const string& url) {
		string::size_type fpos = url.find("://");
		if (fpos == string::npos)
			return 0;
		string prefix = url.substr(0, fpos);
		if (prefix == "memory") {
			return new NativeMemory();
		}
		else if (prefix == "file") {
			string file = url.substr(fpos + 3);
			return new NativeFile(url.substr(fpos + 3));
		}
		else {
			return 0;
		}
	}

	void SimpleCacheEngineFactory::Destroy(SimpleCacheEngine* engine) {
		delete engine;
	}


	SimpleCacheEngine::~SimpleCacheEngine() {
	}

	SimpleCache::SimpleCache() : engine_(0) {
	}

	SimpleCache::SimpleCache(ILock* lock) : engine_(0), lock_(lock) {
	}

	SimpleCache::SimpleCache(SimpleCacheEngine* engine) : engine_(engine), lock_(0) {
	}

	SimpleCache::SimpleCache(SimpleCacheEngine* engine, ILock* lock) : engine_(engine), lock_(lock) {
	}

	void SimpleCache::Set(const string& key, const string& value, time_t expires_in) {
		AutoLock lock(lock_);
		if (engine_)
			engine_->Set(key, value, expires_in);
	}

	string SimpleCache::Get(const string& key) {
		AutoLock lock(lock_);
		if (engine_)
			return engine_->Get(key, false);
		return string();
	}

} // namespace hlp

