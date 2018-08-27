#ifndef _INCLUDE_LIBSTRCOMPAT_
#define _INCLUDE_LIBSTRCOMPAT_


#include <cstddef>


extern "C"
{
	void *strcompat_alloc(void);
	void  strcompat_free (void *ptr);
	
	size_t strcompat_get(const void *ptr, char *dst, size_t dst_len);
	void   strcompat_set(void *ptr, const char *src);
}


#endif
