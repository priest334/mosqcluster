#include "url_encode.h"


const char* encode_table[] = {
		"%00", "%01", "%02", "%03", "%04", "%05", "%06", "%07", "%08", "%09", "%0A", "%0B", "%0C", "%0D", "%0E", "%0F",
		"%10", "%11", "%12", "%13", "%14", "%15", "%16", "%17", "%18", "%19", "%1A", "%1B", "%1C", "%1D", "%1E", "%1F",
		"%20", "!", "%22", "%23", "%24", "%25", "%26", "%27", "(", ")", "*", "%2B", "%2C", "%2D", ".", "%2F",
		"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "%3A", "%3B", "%3C", "%3D", "%3E", "%3F",
		"%40", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
		"P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "%5B", "%5C", "%5D", "%5E", "_",
		"%60", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
		"p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "%7B", "%7C", "%7D", "~", "%7F",
		"%80", "%81", "%82", "%83", "%84", "%85", "%86", "%87", "%88", "%89", "%8A", "%8B", "%8C", "%8D", "%8E", "%8F",
		"%90", "%91", "%92", "%93", "%94", "%95", "%96", "%97", "%98", "%99", "%9A", "%9B", "%9C", "%9D", "%9E", "%9F",
		"%A0", "%A1", "%A2", "%A3", "%A4", "%A5", "%A6", "%A7", "%A8", "%A9", "%AA", "%AB", "%AC", "%AD", "%AE", "%AF",
		"%B0", "%B1", "%B2", "%B3", "%B4", "%B5", "%B6", "%B7", "%B8", "%B9", "%BA", "%BB", "%BC", "%BD", "%BE", "%BF",
		"%C0", "%C1", "%C2", "%C3", "%C4", "%C5", "%C6", "%C7", "%C8", "%C9", "%CA", "%CB", "%CC", "%CD", "%CE", "%CF",
		"%D0", "%D1", "%D2", "%D3", "%D4", "%D5", "%D6", "%D7", "%D8", "%D9", "%DA", "%DB", "%DC", "%DD", "%DE", "%DF",
		"%E0", "%E1", "%E2", "%E3", "%E4", "%E5", "%E6", "%E7", "%E8", "%E9", "%EA", "%EB", "%EC", "%ED", "%EE", "%EF",
		"%F0", "%F1", "%F2", "%F3", "%F4", "%F5", "%F6", "%F7", "%F8", "%F9", "%FA", "%FB", "%FC", "%FD", "%FE", "%FF"
};


string UrlEncode(const string& src) {
	string retval;
	size_t len = src.length();
	for (size_t i = 0; i < len; i++) {
		retval.append(encode_table[(unsigned char)src.at(i)]);
	}
	return retval;
}

namespace {
	bool is_hex_char(char ch) {
		return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
	}

	int char2hex(char ch) {
		if (ch >= '0' && ch <= '9')
			return (int)(ch - '0');
		else if (ch >= 'a' && ch <= 'f')
			return (int)(ch - 'a') + 0xA;
		else if (ch >= 'A' && ch <= 'F')
			return (int)(ch - 'A') + 0xA;
		else
			return 0;
	}
}

string UrlDecode(const string& src) {
	string retval;
	size_t i = 0, j = 0, len = src.length();
	for (; i < len; j++) {
		char c = src.at(i);
		if (c == '%' && (i + 2) < len
			&& is_hex_char(src.at(i + 1)) && is_hex_char(src.at(i + 2))) {
			char ch = ((char2hex(src.at(i + 1)) << 4) & 0xf0) | (char2hex(src.at(i + 2)) & 0x0f);
			retval.append(1, ch);
			i += 3;
		} else {
			retval.append(1, c);
			i++;
		}
	}
	return retval;
}


