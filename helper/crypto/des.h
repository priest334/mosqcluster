
#ifndef __CRYPTO_DES_H__
#define __CRYPTO_DES_H__

#include "crypto/types.h"

namespace hlp {
	namespace crypt {
		class DES
		{
		public:
			DES(const string& key);
			DES(const StringBytes& key);
			void SetKey(const string& key);
			string Encrypt(const string& str);
			string Decrypt(const string& str);
		protected:
			DES(const DES&);
			DES& operator=(const DES&);

		private:
			StringBytes key_;
		};

	} // namespace crypt
}// namespace hlp


#endif // __CRYPTO_DES_H__

