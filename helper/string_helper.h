#pragma once

#ifndef __BASE_STRINGS_STRING_HELPER_H__
#define __BASE_STRINGS_STRING_HELPER_H__

#include <cstdarg>
#include <string>
#include <vector>
#include <map>

namespace hlp {
		using std::string;
		using std::vector;
		using std::map;
		using std::multimap;

		string TripLeft(const string& str, char ch);
		string TripRight(const string& str, char ch);
		string TripAll(const string& str, char ch);
		int RemoveAll(string& str, const std::string& value, bool entire = true);
		int StripLeft(string& str, const std::string& value, bool entire = true);
		int StripRight(string& str, const string& value, bool entire = true);
		int ReplaceAll(string& str, const string& old, const string& value);

		class String {
		public:
			typedef enum {
				FIRST_MATCHED,
				MAX_MATCHED
			}MatchType;

			static const string empty_;
			static size_t Hash(const string& str);

			String();
			String(const char* s);
			String(const string& s);
			~String();

			String(const String& s);
			String& operator=(const char* s);
			String& operator=(const string& s);
			String& operator=(const String& s);

			String operator+(const char* s);
			String operator+(const string& s);
			String operator+(const String& s);

			String& operator+=(const char* s);
			String& operator+=(const string& s);
			String& operator+=(const String& s);

			bool operator==(const char* s);
			bool operator==(const string& s);
			bool operator==(const String& s);
			friend bool operator==(const char* s1, const String& s2);
			friend bool operator==(const string& s1, const String& s2);

			bool operator!=(const char* s);
			bool operator!=(const string& s);
			bool operator!=(const String& s);
			friend bool operator!=(const char* s1, const String& s2);
			friend bool operator!=(const string& s1, const String& s2);

		public:
			String& FormatV(const char* fmt, va_list args);
			String& Format(const char* fmt, ...);
			String& AppendFormatV(const char* fmt, va_list args);
			String& AppendFormat(const char* fmt, ...);
			String& Append(const char* s);
			String& Append(const string& s);
			String& Append(const String& s);
			
			String Left(const string& start, MatchType type = MAX_MATCHED);
			String Right(const string& start, MatchType type = MAX_MATCHED);
			String SubStr(const string& start, const string& end, MatchType type = MAX_MATCHED);

			String& Replace(const char* old, const char* repl, bool all = true);
			String& Replace(const string& old, const string& repl, bool all = true);

			String& TrimLeft(const char* s);
			String& TrimLeft(const string& s);
			String& TrimLeft(const String& s);
			String& TrimRight(const char* s);
			String& TrimRight(const string& s);
			String& TrimRight(const String& s);
			String& Trim(const char* s);
			String& Trim(const string& s);
			String& Trim(const String& s);

			bool IsDigit() const;
			bool IsHex() const;
			bool IsEmpty() const;
			bool Exist(const string& sub, bool ignore_case = false) const;

			size_t Hash() const;

			String& ToLower();
			String& ToUpper();

			const string& str() const;
			const char* c_str() const;
			size_t length() const;
			
		private:
			string value_;
		};

		class StringVector {
		public:
			StringVector();
			explicit StringVector(const vector<string>& vt);
			StringVector(const char* s, const char* sp);
			StringVector(const string& s, const string& sp);
			virtual ~StringVector();

			size_t Split(const char* s, const char* sp);
			size_t Split(const string& s, const string& sp);

			size_t Append(const char* s);
			size_t Append(const string& s);
			size_t Append(const vector<string>& vt);
			size_t Append(const StringVector& s);
			size_t Append(const char* s, const char* sp);
			size_t Append(const string& s, const string& sp);

			size_t Size() const;
			void SetAt(size_t index, const string& s);
			string At(size_t index) const;
			string operator[](int index) const;

			string ToString(const char* sp, bool append_sp = false);
			string ToString(const string& sp, bool append_sp = false);

			vector<string>& reference() { return value_; }
			vector<string> const& const_reference() const { return value_; }

		private:
			typedef vector<string>::const_iterator vtciter;
			vector<string> value_;
		};

		class StringMap {
		public:
			StringMap();
			explicit StringMap(const map<string, string>& m);
			StringMap(const char* values, const char* sp, const char* assign_key);
			StringMap(const string& values, const string& sp, const string& assign_key);
			virtual ~StringMap();

			size_t Split(const char* values, const char* sp, const char* assign_key);
			size_t Split(const string& values, const string& sp, const string& assign_key);

			size_t Append(const char* key, const char* value);
			size_t Append(const string& key, const string& value);
			size_t Append(const map<string, string>& m);
			size_t Append(const StringMap& m);
			size_t Append(const char* values, const char* sp, const char* assign_key);
			size_t Append(const string& values, const string& sp, const string& assign_key);

			void Set(const char* key, const char* value);
			void Set(const string& key, const string& value);
			string Find(const char* key) const;
			string Find(const string& key) const;
			void Remove(const char* key);
			void Remove(const string& key);

			string ToString(const char* sp, const char* assign_key, bool append_sp = false);
			string ToString(const string& sp, const string& assign_key, bool append_sp = false);

			map<string, string>& reference() { return value_; }
			map<string, string> const& const_reference() const { return value_; }

			string operator[](const string& key) const;

		private:
			typedef map<string, string>::const_iterator mciter;
			typedef map<string, string>::iterator miter;
			map<string, string> value_;
		};

		class StringMultiMap {
		public:
			StringMultiMap();
			explicit StringMultiMap(const map<string, string>& m);
			explicit StringMultiMap(const multimap<string, string>& m);
			StringMultiMap(const char* values, const char* sp, const char* assign_key);
			StringMultiMap(const string& values, const string& sp, const string& assign_key);
			virtual ~StringMultiMap();

			size_t Split(const char* values, const char* sp, const char* assign_key);
			size_t Split(const string& values, const string& sp, const string& assign_key);

			size_t Append(const char* key, const char* value);
			size_t Append(const string& key, const string& value);
			size_t Append(const map<string, string>& m);
			size_t Append(const multimap<string, string>& m);
			size_t Append(const StringMultiMap& m);
			size_t Append(const char* values, const char* sp, const char* assign_key);
			size_t Append(const string& values, const string& sp, const string& assign_key);

			void Set(const char* key, const char* value);
			void Set(const string& key, const string& value);
			string Find(const char* key) const;
			string Find(const string& key) const;
			vector<string> FindAll(const char* key) const;
			vector<string> FindAll(const string& key) const;
			void Remove(const char* key);
			void Remove(const string& key);

			string ToString(const char* sp, const char* assign_key, bool append_sp = false);
			string ToString(const string& sp, const string& assign_key, bool append_sp = false);

			multimap<string, string>& reference() { return value_; }
			multimap<string, string> const& const_reference() const { return value_; }

			string operator[](const string& key) const;

		private:
			typedef multimap<string, string>::const_iterator mciter;
			typedef multimap<string, string>::iterator miter;
			typedef std::pair<miter, miter> miter_range;
			typedef std::pair<mciter, mciter> mciter_range;
			multimap<string, string> value_;
		};
} // namespace hlp

#endif // __BASE_STRINGS_STRING_HELPER_H__

