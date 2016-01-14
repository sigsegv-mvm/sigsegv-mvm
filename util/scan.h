#ifndef _INCLUDE_SIGSEGV_UTIL_SCAN_H_
#define _INCLUDE_SIGSEGV_UTIL_SCAN_H_


#include "library.h"
#include "util/buf.h"

namespace Scan
{
	bool CheckMasked(const void *ptr, const ByteBuf& pattern, const ByteBuf& mask);
	void FindMasked(Library lib, const ByteBuf& pattern, const ByteBuf& mask, int align, std::vector<void *>& matches);
	
	void FindFast(Library lib, const void *pattern, int len, int align, std::vector<void *>& matches);
	void FindString(Library lib, const char *str, std::vector<void *>& matches);
}


#endif
