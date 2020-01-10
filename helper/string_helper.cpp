
#include "string_helper.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>
#include <string>
#include <vector>
#include <map>

#if defined(_WIN2)
#define I64d "i64d"
#define StringPrintf _vsnprintf
#define StrCmpI stricmp
#define StrCmpNI strnicmp
#else // !_WIN2
#define I64d "llu"
#define StringPrintf vsnprintf
#define StrCmpI strcasecmp
#define StrCmpNI strncasecmp
#endif

namespace hlp {
	using std::string;

	namespace {
		template <typename T>
		T Round(const T& v, const T& m) {
			return (v > m) ? m : v;
		}
	}

	string TripLeft(const string& str, char ch) {
		string::size_type size = str.length(), i = 0;
		for (; i < size; i++) {
			if (ch != str[i])
				break;
		}
		return str.substr(i);
	}

	string TripRight(const string& str, char ch) {
		string::size_type size = str.length(), i = size - 1;
		for (; i >= 0; i--) {
			if (ch != str[i])
				break;
		}
		return str.substr(0, i + 1);
	}

	string TripAll(const string& str, char ch) {
		return TripLeft(TripRight(str, ch), ch);
	}

	int RemoveAll(string& str, const std::string& value, bool entire/* = true*/) {
		int retval = 0;
		string::size_type len = value.length();
		if (entire) {
			string::size_type fpos, offset = 0;
			do {
				fpos = str.find(value, offset);
				if (string::npos == fpos)
					break;
				str.replace(fpos, len, "");
				offset = fpos + 1;
				++retval;
			} while (true);
		} else {
			for (string::size_type i = 0; i < len; ++i) {
				string::iterator iter = std::remove(str.begin(), str.end(), value[i]);
				retval += int(str.end() - iter);
				str.erase(iter, str.end());
			}
		}
		return retval;
	}

	int StripLeft(string& str, const std::string& value, bool entire/* = true*/) {
		int retval = 0;
		string::size_type len = value.length(), total = str.length();
		if (entire) {
			string::size_type fpos;
			do {
				fpos = str.find(value);
				if (string::npos == fpos || 0 != fpos)
					break;
				str.replace(fpos, len, "");
				++retval;
			} while (true);
		} else {
			string::size_type off = string::npos;
			for (off = 0; off < total; ++off) {
				if (string::npos == value.find(str[off]))
					break;
				++retval;
			}
			if (off > 0)
				str = str.substr(off);
		}
		return retval;
	}

	int StripRight(string& str, const string& value, bool entire/* = true*/) {
		int retval = 0;
		string::size_type len = value.length(), total = str.length();
		if (entire) {
			string::size_type fpos, offset = string::npos;
			do {
				fpos = str.rfind(value, offset);
				if (string::npos == fpos || len != (total - fpos))
					break;
				str.replace(fpos, len, "");
				total -= len;
				++retval;
			} while (true);
		} else {
			string::size_type total = str.length(), off = string::npos;
			for (off = total; off > 0; --off) {
				if (string::npos == value.find(str[off - 1]))
					break;
				++retval;
			}
			if (off > 0)
				str = str.substr(0, off);
		}
		return retval;
	}

	int ReplaceAll(string& str, const string& old, const string& value) {
		int retval = 0;
		string::size_type len = value.length(), oldlen = old.length();
		string::size_type fpos, offset = 0;
		do {
			fpos = str.find(old, offset);
			if (string::npos == fpos)
				break;
			str.replace(fpos, oldlen, value);
			offset = fpos + len;
			++retval;
		} while (true);
		return retval;
	}

	size_t String::Hash(const string& str) {
		size_t seed = 131;
		size_t hash = 0;

		string::size_type index = 0, len = str.length();
		for (; index < len; index++) {
			hash = hash * seed + str[index];
		}
		return hash;
	}

	String::String()
		: value_("") {
	}

	String::String(const char* s)
		: value_(!s ? "" : s) {
	}

	String::String(const string& s)
		: value_(s) {
	}


	String::~String() {
	}

	String::String(const String& s) {
		value_.assign(s.value_);
	}

	String& String::operator=(const char* s) {
		value_.assign(s ? s : "");
		return *this;
	}

	String& String::operator=(const string& s) {
		value_.assign(s);
		return *this;
	}

	String& String::operator=(const String& s) {
		value_.assign(s.value_);
		return *this;
	}

	String String::operator+(const char* s) {
		return String(value_ + (s ? s : ""));
	}

	String String::operator+(const string& s) {
		return String(value_ + s);
	}

	String String::operator+(const String& s) {
		return String(value_ + s.value_);
	}

	String& String::operator+=(const char* s) {
		value_ += s ? s : "";
		return *this;
	}

	String& String::operator+=(const string& s) {
		value_ += s;
		return *this;
	}

	String& String::operator+=(const String& s) {
		value_ += s.value_;
		return *this;
	}


	bool String::operator==(const char* s) {
		return (value_ == s);
	}

	bool String::operator==(const string& s) {
		return (value_ == s);
	}

	bool String::operator==(const String& s) {
		return (value_ == s.value_);
	}

	bool operator==(const char* s1, const String& s2) {
		return (s2.value_ == s1);
	}

	bool operator==(const string& s1, const String& s2) {
		return (s2.value_ == s1);
	}

	bool String::operator!=(const char* s) {
		return !(value_ == s);
	}

	bool String::operator!=(const string& s) {
		return !(value_ == s);
	}

	bool String::operator!=(const String& s) {
		return !(value_ == s.value_);
	}

	bool operator!=(const char* s1, const String& s2) {
		return !(s2.value_ == s1);
	}

	bool operator!=(const string& s1, const String& s2) {
		return !(s2.value_ == s1);
	}

	String& String::FormatV(const char* fmt, va_list args) {
		const int max_buf_len = 102400;
		value_.resize(max_buf_len);
		int wr = StringPrintf(&value_[0], value_.length(), fmt, args);
		if (wr > 0)
			value_.resize(Round(wr, max_buf_len));
		else
			value_.resize(0);
		return *this;
	}

	String& String::Format(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		(*this).FormatV(fmt, args);
		va_end(args);
		return *this;
	}

	String& String::AppendFormatV(const char* fmt, va_list args) {
		String append;
		append.FormatV(fmt, args);
		value_ += append.c_str();
		return *this;
	}

	String& String::AppendFormat(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		(*this).AppendFormatV(fmt, args);
		va_end(args);
		return *this;
	}

	String& String::Append(const char* s) {
		return Append(string(s ? s : ""));
	}

	String& String::Append(const string& s) {
		value_ += s;
		return *this;
	}

	String& String::Append(const String& s) {
		return Append(s.value_);
	}

	String String::Left(const string& start, MatchType type/* = FIRST_MATCHED*/) {
		if (start.empty())
			return *this;

		string::size_type fpos = string::npos;
		switch (type)
		{
		case MAX_MATCHED:
			fpos = value_.rfind(start, value_.length());
			break;
		default:
			fpos = value_.find(start, 0);
			break;
		}

		if (fpos == string::npos)
			return String();

		return String(value_.substr(0, fpos));
	}

	String String::Right(const string& start, MatchType type/* = FIRST_MATCHED*/) {
		if (start.empty())
			return *this;

		string::size_type fpos = string::npos;
		switch (type)
		{
		case MAX_MATCHED:
			fpos = value_.find(start);
			break;
		default:
			fpos = value_.rfind(start, value_.length());
			break;
		}

		if (fpos == string::npos)
			return String();

		return String(value_.substr(fpos + start.length()));
	}

	String String::SubStr(const string& start, const string& end, MatchType type/* = FIRST_MATCHED*/) {
		String empty;
		string::size_type spos, epos;
		spos = epos = string::npos;
		if (start.empty())
			return Left(end, type);

		if (end.empty())
			return Right(start, type);

		spos = value_.find(start);
		epos = value_.rfind(end, value_.length());
		if (spos == string::npos ||
			epos == string::npos ||
			epos <= (spos + start.length()))
			return empty;

		switch (type){
		case MAX_MATCHED: {
			return String(value_.substr(spos + start.length(), epos - spos - start.length()));
		}
		default: {
			epos = value_.find(end, spos + start.length());
			if (epos == string::npos)
				return String();
			return String(value_.substr(spos + start.length(), epos - spos - start.length()));
		}
		}
	}

	String& String::Replace(const char* old, const char* repl, bool all/* = true*/) {
		return Replace(string(old ? old : ""), string(repl ? repl : ""), all);
	}

	String& String::Replace(const string& old, const string& repl, bool all/* = true*/) {
		size_t olen = 0;
		if (old.empty())
			return *this;

		string::size_type fpos = string::npos, offset = 0;
		string rep = repl;

		fpos = value_.find(old, offset);

		while (fpos != string::npos) {
			offset = fpos + rep.length();
			value_.replace(fpos, olen, rep);
			if (!all)
				break;
			fpos = value_.find(old, offset);
		}

		return *this;
	}

	String& String::TrimLeft(const char* s) {
		return TrimLeft(string(s ? s : ""));
	}

	String& String::TrimLeft(const string& s) {
		StripLeft(value_, s);
		return *this;
	}

	String& String::TrimLeft(const String& s) {
		return TrimLeft(s.value_);
	}

	String& String::TrimRight(const char* s) {
		return TrimRight(string(s ? s : ""));
	}

	String& String::TrimRight(const string& s) {
		StripRight(value_, s);
		return *this;
	}

	String& String::TrimRight(const String& s) {
		return TrimRight(s.value_);
	}

	String& String::Trim(const char* s) {
		return Trim(string(s ? s : ""));
	}

	String& String::Trim(const string& s) {
		TrimLeft(s);
		TrimRight(s);
		return *this;
	}

	String& String::Trim(const String& s) {
		return Trim(s.value_);
	}

	bool String::IsDigit() const {
		if (value_.empty())
			return false;

		string::size_type len = value_.length();

		for (string::size_type index = 0; index < len; index++) {
			if (value_[index] < '0' || value_[index] > '9')
				return false;
		}
		return true;
	}

	bool String::IsHex() const {
		if (value_.empty())
			return false;

		string::size_type len = value_.length();

		for (string::size_type index = 0; index < len; index++) {
			if ((value_[index] >= '0' && value_[index] <= '9')
				|| (value_[index] >= 'a' && value_[index] <= 'f')
				|| (value_[index] >= 'A' && value_[index] <= 'F'))
				continue;
			return false;
		}
		return true;
	}

	bool String::IsEmpty() const {
		return value_.empty();
	}

	bool String::Exist(const string& sub, bool ignore_case/* = false*/) const {
		if (sub.empty())
			return false;

		string::size_type i = 0, j = 0, len = value_.length(), sublen = sub.length();
		if (value_.length() < sub.length())
			return false;

		bool eq = false;
		for (; i < len; i++) {
			if (ignore_case) {
				eq = (tolower(value_[i]) == tolower(sub[j]));
			}
			else {
				eq = (value_[i] == sub[j]);
			}
			if (eq && (++j == sublen)) {
				return true;
			}
		}
		return false;
	}

	size_t String::Hash() const {
		return Hash(value_);
	}

	String& String::ToLower() {
		string::size_type index = 0, len = value_.length();
		for (; index < len; index++)
			value_[index] = tolower(value_[index]);
		return *this;
	}

	String& String::ToUpper() {
		string::size_type index = 0, len = value_.length();
		for (; index < len; index++)
			value_[index] = toupper(value_[index]);
		return *this;
	}

	const string& String::str() const {
		return value_;
	}

	const char* String::c_str() const {
		return value_.c_str();
	}

	size_t String::length() const {
		return value_.length();
	}

	// StringVector
	StringVector::StringVector() {
	}

	StringVector::StringVector(const vector<string>& vt) {
		value_ = vt;
	}

	StringVector::StringVector(const char* s, const char* sp) {
		Split(string(s ? s : ""), string(sp ? sp : ""));
	}

	StringVector::StringVector(const string& s, const string& sp) {
		Split(s, sp);
	}

	StringVector::~StringVector() {
		value_.clear();
	}

	size_t StringVector::Split(const char* s, const char* sp) {
		return Split(string(s ? s : ""), string(sp ? sp : ""));
	}

	size_t StringVector::Split(const string& s, const string& sp) {
		value_.clear();
		if (s.empty())
			return 0;

		if (sp.empty()) {
			value_.push_back(s);
			return value_.size();
		}

		string::size_type fpos, offset = 0, splen = sp.length();

		for (;(offset + splen) <= s.length();) {
			fpos = s.find(sp, offset);
			if (fpos == string::npos) {
				value_.push_back(s.substr(offset));
				break;
			} else {
				value_.push_back(s.substr(offset, fpos - offset));
				offset = fpos + splen;
			}
		}

		return value_.size();
	}

	size_t StringVector::Append(const char* s) {
		return Append(string(s ? s : ""));
	}

	size_t StringVector::Append(const string& s) {
		value_.push_back(s);
		return value_.size();
	}

	size_t StringVector::Append(const vector<string>& vt) {
		value_.insert(value_.end(), vt.begin(), vt.end());
		return value_.size();
	}

	size_t StringVector::Append(const StringVector& s) {
		return Append(s.value_);
	}

	size_t StringVector::Append(const char* s, const char* sp) {
		StringVector vt(s, sp);
		return Append(vt);
	}

	size_t StringVector::Append(const string& s, const string& sp) {
		StringVector vt(s, sp);
		return Append(vt);
	}

	size_t StringVector::Size() const {
		return value_.size();
	}

	void StringVector::SetAt(size_t index, const string& s) {
		if (index >= 0 && index <= value_.size())
			value_[index] = s;
		else
			value_.push_back(s);
	}

	string StringVector::At(size_t index) const {
		if (index < 0 || index >= value_.size())
			return "";
		return value_[index];
	}

	string StringVector::operator[](int index) const {
		return At(index);
	}

	string StringVector::ToString(const char* sp, bool append_sp/* = false*/) {
		return ToString(string(sp ? sp : ""), append_sp);
	}

	string StringVector::ToString(const string& sp, bool append_sp/* = false*/) {
		string s = "";
		vtciter iter = value_.begin();

		for (;;){
			s += *iter;
			if ((++iter) == value_.end()) {
				if (append_sp)
					s += sp;
				break;
			}
			s += sp;
		}
		return s;
	}

	// StringMap
	StringMap::StringMap() {
	}

	StringMap::StringMap(const map<string, string>& m) {
		value_.insert(m.begin(), m.end());
	}

	StringMap::StringMap(const char* values, const char* sp, const char* assign_key) {
		Split(string(values ? values : ""), string(sp ? sp : ""), string(assign_key ? assign_key : ""));
	}

	StringMap::StringMap(const string& values, const string& sp, const string& assign_key) {
		Split(values, sp, assign_key);
	}

	StringMap::~StringMap() {
		value_.clear();
	}

	size_t StringMap::Split(const char* values, const char* sp, const char* assign_key) {
		return Split(string(values ? values : ""), string(sp ? sp : ""), string(assign_key ? assign_key : ""));
	}

	size_t StringMap::Split(const string& values, const string& sp, const string& assign_key) {
		value_.clear();
		if (values.empty())
			return 0;

		StringVector vt(values, sp);

		int size = vt.Size();
		for (int i = 0; i < size; i++) {
			string::size_type fpos = vt[i].find(assign_key);
			if (fpos == string::npos)
				continue;
			string key = vt[i].substr(0, fpos);
			string value = vt[i].substr(fpos + assign_key.length());
			if (key.empty())
				continue;
			value_[key] = value;
		}
		return value_.size();
	}

	size_t StringMap::Append(const char* key, const char* value) {
		return Append(string(key ? key : ""), string(value ? value : ""));
	}

	size_t StringMap::Append(const string& key, const string& value) {
		if (!key.empty())
			value_[key] = value;
		return value_.size();
	}

	size_t StringMap::Append(const map<string, string>& m) {
		value_.insert(m.begin(), m.end());
		return value_.size();
	}

	size_t StringMap::Append(const StringMap& m) {
		return Append(m.value_);
	}

	size_t StringMap::Append(const char* values, const char* sp, const char* assign_key) {
		StringMap m(values, sp, assign_key);
		return Append(m);
	}

	size_t StringMap::Append(const string& values, const string& sp, const string& assign_key) {
		StringMap m(values, sp, assign_key);
		return Append(m);
	}

	void StringMap::Set(const char* key, const char* value) {
		Set(string(key ? key : ""), string(value ? value : ""));
	}

	void StringMap::Set(const string& key, const string& value) {
		if (key.empty())
			return;
		value_[key] = value;
	}

	string StringMap::Find(const char* key) {
		string k(key ? key : "");
		return Find(k);
	}

	string StringMap::Find(const string& key) {
		if (key.empty())
			return key;
		mciter iter = value_.find(key);
		if (iter != value_.end())
			return iter->second;
		return string();
	}

	void StringMap::Remove(const char* key) {
		string k(key ? key : "");
		return Remove(k);
	}

	void StringMap::Remove(const string& key) {
		if (key.empty())
			return;
		miter iter = value_.find(key);
		if (iter != value_.end())
			value_.erase(iter);
	}

	string StringMap::ToString(const char* sp, const char* assign_key, bool append_sp/* = false*/) {
		return ToString(string(sp ? sp : ""), string(assign_key ? assign_key : ""), append_sp);
	}

	string StringMap::ToString(const string& sp, const string& assign_key, bool append_sp/* = false*/) {
		string s("");

		mciter iter = value_.begin();
		for (;;) {
			s += (*iter).first;
			s += assign_key;
			s += (*iter).second;
			if ((++iter) == value_.end()) {
				if (append_sp)
					s += sp;
				break;
			}
			s += sp;
		}
		return s;
	}

	string StringMap::operator[](const string& key) const {
		mciter iter = value_.find(key);
		if (iter != value_.end()) {
			return iter->second;
		}
		return "";
	}
} // namespace hlp


