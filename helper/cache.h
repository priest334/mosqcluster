#pragma once

#include <time.h>
#include <string>

class ILock;

namespace hlp {

	using std::string;

	class SimpleCacheEngine {
	public:
		virtual ~SimpleCacheEngine();
		virtual void Set(const string& key, const string& value, time_t expires_in) = 0;
		virtual string Get(const string& key, time_t& expires_in, bool cleanup) = 0;
	};

	class SimpleCacheEngineFactory {
	public:
		virtual ~SimpleCacheEngineFactory();
		virtual SimpleCacheEngine* Create(const string& url);
		virtual void Destroy(SimpleCacheEngine* engine);
	};

	class SimpleCache {
	public:
		SimpleCache();
		explicit SimpleCache(ILock* lock);
		explicit SimpleCache(SimpleCacheEngine* engine);
		SimpleCache(SimpleCacheEngine* engine, ILock* lock);
		void Set(const string& key, const string& value, time_t expires_in);
		string Get(const string& key, time_t& expires_in);
	private:
		SimpleCacheEngine* engine_;
		ILock* lock_;
	};

} // namespace hlp;

