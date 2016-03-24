#ifndef _INCLUDE_SIGSEGV_UTIL_MISC_H_
#define _INCLUDE_SIGSEGV_UTIL_MISC_H_


#define SIZE_CHECK(_type, _size) static_assert(sizeof(_type) == _size, "sizeof(" #_type ") == " #_size)


#define EXTENT(_array, _dimension) std::extent<decltype(_array), (_dimension)>::value


constexpr long double operator"" _deg(long double deg)        { return deg * (M_PI / 180.0); }
constexpr long double operator"" _deg(unsigned long long deg) { return (long double)deg * (M_PI / 180.0); }


#endif
