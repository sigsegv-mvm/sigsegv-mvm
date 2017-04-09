#ifndef _INCLUDE_SIGSEGV_UTIL_MISC_H_
#define _INCLUDE_SIGSEGV_UTIL_MISC_H_


#include <random>


#define SIZE_CHECK(_type, _size) static_assert(sizeof(_type) == _size, "sizeof(" #_type ") == " #_size)


constexpr long double operator"" _deg(long double deg)        { return deg * (M_PI / 180.0); }
constexpr long double operator"" _deg(unsigned long long deg) { return (long double)deg * (M_PI / 180.0); }


template<typename T>
constexpr size_t countof()
{
	using U = typename std::remove_reference<T>::type;
	
	static_assert(std::is_array<U>::value,   "countof() requires an array argument");
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
//#define MAT_SINGLE_THREAD_BLOCK for (MatSingleThreadBlock __mat_single_thread_block; __mat_single_thread_block.ShouldContinue(); )
#define MAT_SINGLE_THREAD_BLOCK


inline bool FStrEq(const char *sz1, const char *sz2)
{
	return (sz1 == sz2 || V_stricmp(sz1, sz2) == 0);
}


/* return an iterator to a random element in an STL container
 * based on: http://stackoverflow.com/a/16421677 */
template<typename Iterator>
Iterator select_random(Iterator begin, Iterator end)
{
	static std::random_device r_dev;
	static std::mt19937 r_engine(r_dev());
	
	std::uniform_int_distribution<> r_dist(0, std::distance(begin, end) - 1);
	
	std::advance(begin, r_dist(r_engine));
	return begin;
}

template<typename Container>
auto select_random(const Container& container)
{
	return select_random(begin(container), end(container));
}


#if 0
class CEntitySphereQuery
{
public:
	CEntitySphereQuery(const Vector& center, float radius, int flagMask = 0)
	{
		this->m_listCount = UTIL_EntitiesInSphere(this->m_pList, MAX_SPHERE_QUERY, center, radius, flagMask);
	}
	
	CBaseEntity *GetCurrentEntity()
	{
		if (this->m_listIndex >= this->m_listCount) return nullptr;
		
		return this->m_pList[this->m_listIndex];
	}
	
	void NextEntity() { ++this->m_listIndex; }
	
private:
	constexpr size_t MAX_SPHERE_QUERY = 512;
	
	int m_listIndex = 0;
	int m_listCount;
	CBaseEntity *m_pList[MAX_SPHERE_QUERY];
}
#endif


#if 0
/* allow using CHandle<T> as the key type in std::unordered_map */
namespace std
{
	template<typename T> template<> struct hash<CHandle<T>>
	{
		using argument_type = CHandle<T>;
		using result_type   = size_t;
		
		result_type operator()(const argument_type& arg) const
		{
			// TODO
		}
	};
}
#endif


#endif
