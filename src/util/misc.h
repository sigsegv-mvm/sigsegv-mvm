#ifndef _INCLUDE_SIGSEGV_UTIL_MISC_H_
#define _INCLUDE_SIGSEGV_UTIL_MISC_H_


#define SIZE_CHECK(_type, _size) static_assert(sizeof(_type) == _size, "sizeof(" #_type ") == " #_size)


#endif
