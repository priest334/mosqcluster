#include "crypto/base32.h"

namespace hlp {
	namespace crypt {
		namespace {
			const char kDefaultBase32Keys[] = "0123456789abcdfhijklmoqrstuvwxyz";

			int base32_encrypt(char keys[32], uchar_t* in, int inlen, uchar_t out[8]) {
				if (inlen <= 0)
					return 0;
				out[0] = keys[in[0] >> 3];
				if (inlen == 1) {
					out[1] = keys[((in[0] & 0x07) << 2)];
					return 2;
				}
				out[1] = keys[((in[0] & 0x07) << 2) | (in[1] >> 6)];
				out[2] = keys[(in[1] & 0x3f) >> 1];
				if (inlen == 2) {
					out[3] = keys[((in[1] & 0x01) << 4)];
					return 4;
				}
				out[3] = keys[((in[1] & 0x01) << 4) | (in[2] >> 4)];
				if (inlen == 3) {
					out[4] = keys[((in[2] & 0x0f) << 1)];
					return 5;
				}
				out[4] = keys[((in[2] & 0x0f) << 1) | (in[3] >> 7)];
				out[5] = keys[(in[3] & 0x7f) >> 2];
				if (inlen == 4) {
					out[6] = keys[((in[3] & 0x03) << 3)];
					return 7;
				}
				out[6] = keys[((in[3] & 0x03) << 3) | (in[4] >> 5)];
				out[7] = keys[in[4] & 0x1f];
				return 8;
			}

			int base32_decrypt(uchar_t index[256], uchar_t* in, int inlen, uchar_t out[5]) {
				if (inlen <= 0)
					return 0;
				out[0] = (index[in[0]] << 3);
				if (inlen == 1) {
					return 1;
				}
				out[0] = (index[in[0]] << 3) | (index[in[1]] >> 2);
				if (inlen == 2) {
					out[1] = ((index[in[1]] & 0x03) << 6);
					return 1 + (out[1] ? 1 : 0);
				}
				if (inlen == 3) {
					out[1] = ((index[in[1]] & 0x03) << 6) | (index[in[2]] << 1);
					return 2;
				}
				out[1] = ((index[in[1]] & 0x03) << 6) | (index[in[2]] << 1) | (index[in[3]] >> 4);
				if (inlen == 4) {
					out[2] = ((index[in[3]] & 0x0f) << 4);
					return 2 + (out[2] ? 1 : 0);
				}
				out[0] = (index[in[0]] << 3) | (index[in[1]] >> 2);
				out[1] = ((index[in[1]] & 0x03) << 6) | (index[in[2]] << 1) | (index[in[3]] >> 4);
				out[2] = ((index[in[3]] & 0x0f) << 4) | (index[in[4]] >> 1);
				if (inlen == 5) {
					out[3] = ((index[in[4]] & 0x01) << 7);
					return 3 + (out[3] ? 1 : 0);
				}
				if (inlen == 6) {
					out[3] = ((index[in[4]] & 0x01) << 7) | (index[in[5]] << 2);
					return 4;
				}
				out[3] = ((index[in[4]] & 0x01) << 7) | (index[in[5]] << 2) | (index[in[6]] >> 3);
				if (inlen == 7) {
					out[4] = ((index[in[6]] & 0x07) << 5);
					return 5;
				}
				out[4] = ((index[in[6]] & 0x07) << 5) | index[in[7]];
				return 5;
			}
		}

		BASE32::BASE32() {
			SetKeys(kDefaultBase32Keys);
		}

		int BASE32::SetKeys(const string& keys) {
			int retval = 0;
			if (keys.length() < BASE32_KEY_LENGTH) {
				memcpy(keys_.keys_, kDefaultBase32Keys, BASE32_KEY_LENGTH);
				retval = -1;
			} else {
				memcpy(keys_.keys_, keys.c_str(), BASE32_KEY_LENGTH);
			}

			char indexcount_[BASE32_INDEX_LENGTH] = { 0 };
			for (size_t i = 0; i < BASE32_KEY_LENGTH; i++) {
				if (indexcount_[keys_[i]] == 1)
					retval = -1;
				index_[keys_[i]] = i;
				indexcount_[keys_[i]] = 1;
			}
			return retval;
		}

		string BASE32::Encrypt(const string& str) {
			if (str.empty())
				return string();
			size_t len = str.length();
			StringBytes bytes(str.begin(), str.end());
			size_t block = len / 5 + ((len % 5) ? 1 : 0);
			StringBytes out;
			out.resize(block * 8, 0);
			size_t total = 0;
			for (size_t i = 0, oft = 0, oo = 0; i < block; i++, oft += 5, oo += 8) {
				total += base32_encrypt(keys_, &bytes[oft], len - oft, &out[oo]);
			}
			out.resize(total);
			return string(out.begin(), out.end());
		}

		string BASE32::Decrypt(const string& str) {
			size_t len = str.length();
			size_t block = len / 8 + ((len % 8) ? 1 : 0);
			StringBytes bytes(str.begin(), str.end()), out;
			out.resize(block * 5, 0);
			size_t total = 0;
			for (size_t i = 0, oft = 0, oo = 0; i < block; i++, oft += 8, oo += 5) {
				total += base32_decrypt(index_, &bytes[oft], len - oft, &out[oo]);
			}
			out.resize(total);
			return string(out.begin(), out.end());
		}

	} // namespace crypt
} // namespace hlp

