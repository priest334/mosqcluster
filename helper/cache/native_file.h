#pragma once

#include <stdio.h>
#include "../cache.h"

namespace hlp {

	class NativeFile : public SimpleCacheEngine {
		struct Item {
			size_t capcity;
			size_t next;
			time_t expires_in;
			string key;
			string value;
		};

		void Write(const NativeFile::Item& item);
		bool Read(NativeFile::Item& item);
		void WriteItem(size_t addr, const NativeFile::Item& item);
		void ReadItem(NativeFile::Item& item);
	public:
		NativeFile();
		NativeFile(const string& file);
		~NativeFile();

		void Open(const string& file);

		void Set(const string& key, const string& value, time_t expires_in);
		string Get(const string& key, time_t& expires_in, bool cleanup = false);

	private:
		FILE* fpr_;
		FILE* fpw_;
		string file_;
	};

} // namespace hlp

