#ifndef _INCLUDE_SIGSEGV_UTIL_MISC_H_
#define _INCLUDE_SIGSEGV_UTIL_MISC_H_


#define SIZE_CHECK(_type, _size) static_assert(sizeof(_type) == _size, "sizeof(" #_type ") == " #_size)


constexpr long double operator"" _deg(long double deg)        { return deg * (M_PI / 180.0); }
constexpr long double operator"" _deg(unsigned long long deg) { return (long double)deg * (M_PI / 180.0); }


template<typename T>
constexpr size_t countof()
{
	using U = typename std::remove_reference<T>::type;
	
	static_assert(std::is_array<U>::value, "countof() requires an array argument");
	static_assert(std::extent<U>::value > 0, "zero- or unknown-size array");
	
	return std::extent<U>::value;
}
#define countof(x) countof<decltype(x)>()


#endif
