#include "crypto/base64.h"

#include <string.h>

namespace hlp {
	namespace crypt {
		namespace {
			const char kDefaultBase64Keys[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

			int base64_encrypt(char keys[64], uchar_t* in, int inlen, uchar_t out[4]) {
				if (inlen <= 0)
					return 0;
				out[0] = keys[in[0] >> 2];
				if (inlen == 1) {
					out[1] = keys[(in[0] & 0x3) << 4];
					out[2] = out[3] = '=';
					return 2;
				}
				out[1] = keys[((in[0] & 0x3) << 4) | (in[1] >> 4)];
				if (inlen == 2) {
					out[2] = keys[(in[1] & 0xf) << 2];
					out[3] = '=';
					return 1;
				}
				out[2] = keys[((in[1] & 0xf) << 2) | (in[2] >> 6)];
				out[3] = keys[in[2] & 0x3f];
				return 0;
			}

			int base64_decrypt(uchar_t index[256], uchar_t in[4], uchar_t out[3]) {
				out[0] = (index[in[0]] << 2) | (index[in[1]] >> 4);
				if (in[2] == '=') {
					return 1;
				}
				out[1] = ((index[in[1]] & 0xf) << 4) | (index[in[2]] >> 2);
				if (in[3] == '=') {
					return 2;
				}
				out[2] = ((index[in[2]] & 0x3) << 6) | (index[in[3]] & 0x3f);
				return 3;
			}
		}

		Base64::Base64() {
			SetKeys(kDefaultBase64Keys);
		}

		int Base64::SetKeys(const string& keys) {
			int retval = 0;
			if (keys.length() < BASE64_KEY_LENGTH) {
				memcpy(keys_.keys_, kDefaultBase64Keys, BASE64_KEY_LENGTH);
				retval = -1;
			} else {
				memcpy(keys_.keys_, keys.c_str(), BASE64_KEY_LENGTH);
			}

			char index_count_[BASE64_INDEX_LENGTH] = { 0 };
			for (size_t i = 0; i < BASE64_KEY_LENGTH; i++) {
				if (index_count_[keys_[i]] == 1)
					retval = -1;
				index_.index_[keys_[i]] = i;
				index_count_[keys_[i]] = 1;
			}
			return retval;
		}

		string Base64::Encrypt(const string& str) {
			if (str.empty())
				return string();
			size_t len = str.length();
			StringBytes bytes(str.begin(), str.end());
			size_t block = len / 3 + ((len % 3) ? 1 : 0);
			StringBytes out;
			out.resize(block * 4);
			for (size_t i = 0, oft = 0, oo = 0; i < block; i++, oft += 3, oo += 4) {
				base64_encrypt(keys_, &bytes[oft], len - oft, &out[oo]);
			}
			return string(out.begin(), out.end());
		}

		string Base64::Decrypt(const string& str) {
			size_t len = str.length();
			if (len % 4 != 0)
				return string();
			size_t block = len / 4;
			StringBytes bytes(str.begin(), str.end()), out;
			out.resize(block * 3, 0);
			size_t total = 0;
			for (size_t i = 0, oft = 0, oo = 0; i < block; i++, oft += 4, oo += 3) {
				total += base64_decrypt(index_.index_, &bytes[oft], &out[oo]);
			}
			out.resize(total);
			return string(out.begin(), out.end());
		}

	} // namespace crypt
} // namespace hlp

