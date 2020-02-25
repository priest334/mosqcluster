#include "crypto/aes.h"

#include <string.h>
#include <openssl/aes.h>
#include "crypto/hexstr.h"

namespace hlp {
	namespace crypt {
		AES::AES(const string& key) : key_(key.begin(), key.end()) {
		}

		AES::AES(const StringBytes& key) : key_(key) {
		}

		string AES::Encrypt(const string& str) {
			AES_KEY enckey;
			switch (key_.length()) {
			case 16:
				AES_set_encrypt_key(key_.c_str(), 128, &enckey);
				break;
			default:
				break;
			}

			//memcpy(&enckey, key_.c_str(), sizeof(enckey));
			
			size_t total = str.length();
			size_t block = total / AES_BLOCK_SIZE;
			bool padding = (total % AES_BLOCK_SIZE) ? true : false;
			block += padding ? 1 : 0;
			StringBytes bytes(str.begin(), str.end());
			if (padding) {
				size_t less = AES_BLOCK_SIZE - total % AES_BLOCK_SIZE;
				bytes.resize(block * AES_BLOCK_SIZE);
				bytes.insert(total, less, less);
			}

			unsigned char ivec[AES_BLOCK_SIZE] = { 0 };
			memcpy(ivec, key_.c_str(), AES_BLOCK_SIZE);

			StringBytes out;
			out.resize(block * AES_BLOCK_SIZE);
			for (size_t i = 0; i < block; i++) {
				AES_encrypt(&bytes[i*AES_BLOCK_SIZE], &out[i*AES_BLOCK_SIZE], &enckey);
			}
			return Bytes2HexStr(out);
		}

		string AES::Decrypt(const string& str) {
			StringBytes bytes = HexStr2Bytes(str);

			size_t total = bytes.length();
			size_t block = total / AES_BLOCK_SIZE;
			bool padding = (total % AES_BLOCK_SIZE) ? true : false;
			if (padding)
				return string();

			AES_KEY enckey;
			switch (key_.length()) {
			case 16:
				AES_set_decrypt_key(key_.c_str(), 128, &enckey);
				break;
			default:
				break;
			}

			StringBytes out;
			out.resize(block * AES_BLOCK_SIZE);
			for (size_t i = 0; i < block; i++) {
				AES_decrypt(&bytes[i*AES_BLOCK_SIZE], &out[i*AES_BLOCK_SIZE], &enckey);
			}

			size_t pv = out[out.length() - 1];
			if (pv < 0xf) {
				out.erase(out.length() - pv, pv);
			}

			return string(out.begin(), out.end());
		}

	} // namespace crypt
} // namespace hlp



