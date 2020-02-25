#include "crypto/des.h"

#include <openssl/des.h>
#include "crypto/hexstr.h"

#define DES_BLOCK_SIZE sizeof(DES_cblock)
//const size_t DES_BLOCK_SIZE = sizeof(DES_cblock);

namespace hlp {
	namespace crypt {
		DES::DES(const string& key) : key_(key.begin(), key.end()) {
		}

		DES::DES(const StringBytes& key) : key_(key) {
		}

		void DES::SetKey(const string& key) {
		}

		string DES::Encrypt(const string& str) {
			size_t total = str.length();
			size_t block = total / DES_BLOCK_SIZE;
			bool padding = (total % DES_BLOCK_SIZE) ? true : false;
			block += padding ? 1 : 0;
			StringBytes bytes(str.begin(), str.end());
			if (padding) {
				size_t less = DES_BLOCK_SIZE - total % DES_BLOCK_SIZE;
				bytes.resize(block * DES_BLOCK_SIZE);
				bytes.insert(total, less, less);
			}

			DES_cblock des_key, ivec;
			DES_key_schedule ks;
			//DES_string_to_key((const char*)key_.c_str(), &des_key);
			memcpy(&des_key, key_.c_str(), DES_BLOCK_SIZE);
			DES_set_key(&des_key, &ks);

			StringBytes out;
			out.resize(block * DES_BLOCK_SIZE);
			for (size_t i = 0; i < block; i++) {
				memset(&ivec, 0, DES_BLOCK_SIZE);
				DES_ncbc_encrypt(&bytes[i*DES_BLOCK_SIZE], &out[i*DES_BLOCK_SIZE], DES_BLOCK_SIZE, &ks, &ivec, DES_ENCRYPT);
			}

			return Bytes2HexStr(out);
		}

		string DES::Decrypt(const string& str) {
			StringBytes bytes = HexStr2Bytes(str);

			size_t total = bytes.length();
			size_t block = total / DES_BLOCK_SIZE;
			bool padding = (total % DES_BLOCK_SIZE) ? true : false;
			if (padding)
				return string();

			DES_cblock des_key, ivec;
			DES_key_schedule ks;
			//DES_string_to_key((const char*)key_.c_str(), &des_key);
			memcpy(&des_key, key_.c_str(), DES_BLOCK_SIZE);
			DES_set_key(&des_key, &ks);

			StringBytes out;
			out.resize(block * DES_BLOCK_SIZE);
			for (size_t i = 0; i < block; i++) {
				memset(&ivec, 0, DES_BLOCK_SIZE);
				DES_ncbc_encrypt(&bytes[i*DES_BLOCK_SIZE], &out[i*DES_BLOCK_SIZE], DES_BLOCK_SIZE, &ks, &ivec, DES_DECRYPT);
			}

			size_t pv = out[out.length() - 1];
			if (pv < 0xf) {
				out.erase(out.length() - pv, pv);
			}

			return string(out.begin(), out.end());
		}

	} // namespace crypt
} // namespace hlp

