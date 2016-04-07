#ifndef _INCLUDE_SIGSEGV_UTIL_SCOPE_H_
#define _INCLUDE_SIGSEGV_UTIL_SCOPE_H_


#include "util/backtrace.h"


class RefCount
{
public:
	operator const int&() const { return this->m_iCount; }
	
	void operator++() { this->Increment(); }
	
	void operator--() { this->Decrement(); }
	
	void Increment()
	{
		++this->m_iCount;
	}
	void Decrement()
	{
		if (this->m_iCount > 0) {
			--this->m_iCount;
		} else {
			DevWarning("RefCount::Decrement: m_iCount was %d!\n", this->m_iCount);
			BACKTRACE();
			this->m_iCount = 0;
		}
	}
	
private:
	int m_iCount = 0;
};


class ScopedIncrement
{
public:
	ScopedIncrement(RefCount& rc) : m_RefCount(rc) {}
	~ScopedIncrement()
	{
		if (this->m_bIncremented) {
			--this->m_RefCount;
			this->m_bIncremented = false;
		}
	}
	
	void Increment()
	{
		assert(!this->m_bIncremented);
		++this->m_RefCount;
		this->m_bIncremented = true;
	}
	
private:
	RefCount& m_RefCount;
	bool m_bIncremented = false;
};
#define SCOPED_INCREMENT(rc) \
	ScopedIncrement _incr_##rc(rc); \
	_incr_##rc.Increment()
#define SCOPED_INCREMENT_IF(rc, predicate) \
	ScopedIncrement _incr_##rc(rc); \
	if (predicate) { \
		_incr_##rc.Increment(); \
	}


#if 0
class ScopedPush
{
	
};
#define SCOPED_PUSH(stack, data) stack##_push(stack, (void *)data)
#endif


#endif
