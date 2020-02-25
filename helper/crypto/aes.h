
#ifndef __CRYPTO_AES_H__
#define __CRYPTO_AES_H__

#include <string>
#include "crypto/types.h"

namespace hlp {
	namespace crypt {
		class AES
		{
		public:
			AES(const string& key);
			AES(const StringBytes& key);
			string Encrypt(const string& str);
			string Decrypt(const string& str);
		protected:
			AES(const AES&);
			AES& operator=(const AES&);

		private:
			StringBytes key_;
		};

	} // namespace crypt
}// namespace hlp


#endif // __CRYPTO_AES_H__

