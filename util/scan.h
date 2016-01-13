#ifndef _INCLUDE_SIGSEGV_UTIL_SCAN_H_
#define _INCLUDE_SIGSEGV_UTIL_SCAN_H_


#include "common.h"


bool MemFindPattern(const void *libPtr, const char *pattern, size_t len, std::vector<void *>& matches);


#endif
