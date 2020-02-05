#include "native_memory.h"

#include <time.h>

namespace hlp {

	NativeMemory::Value::Value(const string& value, time_t expires_in/* = -1*/) : value_(value), expires_in_(expires_in) {
	}

	bool NativeMemory::Value::IsExpired(time_t now) const {
		return (expires_in_ >= 0 && now > expires_in_);
	}

	void NativeMemory::Value::Set(const string& value, time_t expires_in) {
		value_ = value;
		expires_in_ = (expires_in >= 0) ? (time(NULL) + expires_in) : -1;
	}

	string NativeMemory::Value::Get() const {
		return value_;
	}

	time_t NativeMemory::Value::expires() const {
		return expires_in_;
	}

	NativeMemory::NativeMemory() {
	}

	NativeMemory::~NativeMemory() {
	}

	void NativeMemory::Set(const string& key, const string& value, time_t expires_in) {
		map<string, Value*>::const_iterator iter = values_.find(key);
		if (iter != values_.end()) {
			Value* p = iter->second;
			p->Set(value, expires_in);
		}
		else {
			Value* p = new Value(value, expires_in);
			values_[key] = p;
		}
	}

	string NativeMemory::Get(const string& key, time_t& expires_in, bool cleanup) {
		map<string, Value*>::iterator iter = values_.find(key);
		if (iter != values_.end()) {
			Value* p = iter->second;
			if (!p->IsExpired(time(NULL))) {
				expires_in = p->expires();
				return p->Get();
			}
			if (cleanup) {
				values_.erase(iter);
				delete p;
			}
		}
		return string();
	}
} // namespace hlp


