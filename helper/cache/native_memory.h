#pragma once


#include <map>
#include "../cache.h"


namespace hlp {

	using std::map;

	class NativeMemory : public SimpleCacheEngine {
		class Value {
		public:
			Value(const string& value, time_t expires_in = -1);
			bool IsExpired(time_t now) const;
			void Set(const string& value, time_t expires_in);
			string Get() const;
			time_t expires() const;
		private:
			string value_;
			time_t expires_in_;
		};
	public:
		NativeMemory();
		~NativeMemory();

		void Set(const string& key, const string& value, time_t expires_in);
		string Get(const string& key, time_t& expires_in, bool cleanup = false);

	private:
		map<string, Value*> values_;
	};

} // namespace hlp

