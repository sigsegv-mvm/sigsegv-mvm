#ifndef _INCLUDE_SIGSEGV_UTIL_BREAKPOINT_H_
#define _INCLUDE_SIGSEGV_UTIL_BREAKPOINT_H_


#if defined __GNUC__ || defined __clang__

// GCC attributes: always_inline
//[[gnu::always_inline]] static inline void __int3() { asm volatile("int $0x03"); }
#define BREAKPOINT() asm volatile("int $0x03")

#elif defined _MSC_VER

#include <intrin.h>
#define BREAKPOINT() __debug_break

#endif


#define BREAKPOINT_IF(pred) \
	do { \
		if (pred) { \
			BREAKPOINT(); \
		} \
	} while (false)


#endif
