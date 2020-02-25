#ifndef __CRYPTO_BASE32_H__
#define __CRYPTO_BASE32_H__

#include <string>
#include "crypto/types.h"

#define BASE32_KEY_LENGTH 32
#define BASE32_INDEX_LENGTH 256

namespace hlp {
	namespace crypt {

		using std::string;

		class BASE32 {
			typedef struct base32_keys_s {
				char keys_[BASE32_KEY_LENGTH];
				operator char*() {
					return &keys_[0];
				}
			} Base32Keys;

			typedef struct base32_index_s {
				uchar_t index_[BASE32_INDEX_LENGTH];
				operator uchar_t*() {
					return &index_[0];
				}
			} Base32Index;
		public:
			BASE32();
			int SetKeys(const string& keys);
			string Encrypt(const string& str);
			string Decrypt(const string& str);
		protected:
			void InitIndex();
		private:
			Base32Keys keys_;
			Base32Index index_;
		};
	} // namespace crypt
} // namespace hlp

#endif // __CRYPTO_BASE32_H__
