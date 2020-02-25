#ifndef __CRYPTO_HEXSTR_H__
#define __CRYPTO_HEXSTR_H__

#include "crypto/types.h"

namespace hlp {

	StringBytes HexStr2Bytes(const string& str);
	string Bytes2HexStr(const StringBytes& bytes);

} // namespace hlp


#endif // __CRYPTO_HEXSTR_H__

