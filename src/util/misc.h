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


/* use this when you want to do e.g. multiple calls to console spew functions
 * and don't want mat_queue_mode 2 to mess up the ordering */
class MatSingleThreadBlock
{
public:
	MatSingleThreadBlock()
	{
		if (g_pMaterialSystem != nullptr) {
			g_pMaterialSystem->AllowThreading(false, GetMaterialSystemThreadNum());
		}
	}
	~MatSingleThreadBlock()
	{
		if (g_pMaterialSystem != nullptr) {
			g_pMaterialSystem->AllowThreading(true, GetMaterialSystemThreadNum());
		}
	}
	
	bool ShouldContinue() { return (this->m_iCounter++ == 0); }
	
private:
	/* this is normally 0; with -swapcores param, it is instead 1 */
	static constexpr int GetMaterialSystemThreadNum() { return 0; }
	
	int m_iCounter = 0;
};
#define MAT_SINGLE_THREAD_BLOCK for (MatSingleThreadBlock __mat_single_thread_block; __mat_single_thread_block.ShouldContinue(); )


#endif
