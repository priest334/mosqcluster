#pragma once

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include <map>
#include <sstream>

namespace hlp {
		using std::string;
		using std::map;
		using std::ostringstream;

		typedef long long Int64;
		typedef int(*ArchiveCallback)(ostringstream& oss, const string&, const string&, const string&, void*);

		namespace conf {
			class Value {
			public:
				static char kSep;
				string Get(const string& name, bool& exist) const;
				string Get(const string& name) const;
				string Get(const string& name, const string& default_value) const;
				Int64 GetInt(const string& name) const;
				Int64 GetInt(const string& name, Int64 default_value);

				void Set(const string& name, const string& value);
				void SetInt(const string& name, Int64 value);

				void Archive(ostringstream& oss, const string& vpath, ArchiveCallback callback, void* argp) const;
			private:
				map<string, string> prop_;
				map<string, Value> child_;
			};
		}

		class ConfigParser;

		class Config {
		public:
			Config();
			Config(ConfigParser* parser, int delete_parser = 0);
			~Config();

			void LoadBuffer(const string& buffer);
			void LoadFile(const string& file);
			string ToBuffer() const;
			void ToFile(const string& file) const;

			bool HasMember(const string& name);
	
			string Get(const string& name) const;
			string Get(const string& name, const string& default_value) const;
			Int64 GetInt(const string& name) const;
			Int64 GetInt(const string& name, Int64 default_value);

			void Set(const string& name, const string& value);
			void SetInt(const string& name, Int64 value);

			void Archive(ostringstream& oss);
			void OnArchiveValue(ostringstream& oss, const string& vpath, const string& name, const string& value);
		private:
			conf::Value root_;
			ConfigParser* parser_;
			int delete_parser_;
		};

		class ConfigParser {
		public:
			virtual void OnArchiveValue(ostringstream& oss, const string& vpath, const string& name, const string& value) = 0;
			virtual void LoadBuffer(Config* config, const string& buffer) = 0;
			virtual void LoadFile(Config* config, const string& file) = 0;
			virtual string ToBuffer(Config* config) = 0;
			virtual void ToFile(Config* config, const string& file) = 0;
		};
} // namespace hlp

#endif // __CONFIG_H__


