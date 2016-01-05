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


#endif
