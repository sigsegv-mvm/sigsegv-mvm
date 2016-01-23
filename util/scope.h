#ifndef _INCLUDE_SIGSEGV_UTIL_SCOPE_H_
#define _INCLUDE_SIGSEGV_UTIL_SCOPE_H_


class RefCount
{
public:
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
			this->m_iCount = 0;
		}
	}
	
	bool NonZero() const
	{
		return (this->m_iCount != 0);
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


#if 0
class ScopedPush
{
	
};
#define SCOPED_PUSH(stack, data) stack##_push(stack, (void *)data)
#endif


#endif
