#ifndef _INCLUDE_SIGSEGV_UTIL_H_
#define _INCLUDE_SIGSEGV_UTIL_H_


#include "common.h"


class RefCount
{
public:
	void Increment()
	{
		++m_iCount;
	}
	void Decrement()
	{
		assert(m_iCount > 0);
		--m_iCount;
	}
	
	bool NonZero() const
	{
		return (m_iCount != 0);
	}
	
private:
	int m_iCount = 0;
};

class ScopedIncrement
{
public:
	ScopedIncrement(RefCount& rc) : m_RefCount(rc)
	{
		m_RefCount.Increment();
	}
	~ScopedIncrement()
	{
		m_RefCount.Decrement();
	}
	
private:
	RefCount& m_RefCount;
};
#define SCOPED_INCREMENT(rc) ScopedIncrement rc##_incr(rc)


#define MOD_DETOUR_CREATE_MEMBER(obj, func) \
	detour_##obj##_##func = DETOUR_CREATE_MEMBER(obj##_##func, #obj "::" #func); \
	if (detour_##obj##_##func == nullptr) { \
		snprintf(error, maxlen, "DETOUR_CREATE_MEMBER failed for %s::%s", #obj, #func); \
		return false; \
	}
#define MOD_DETOUR_DESTROY_MEMBER(obj, func) \
	if (detour_##obj##_##func != nullptr) { \
		detour_##obj##_##func->Destroy(); \
		detour_##obj##_##func = nullptr; \
	}
#define MOD_DETOUR_TOGGLE_MEMBER(enable, obj, func) \
	if (enable) detour_##obj##_##func->EnableDetour(); \
	else        detour_##obj##_##func->DisableDetour();


#endif
