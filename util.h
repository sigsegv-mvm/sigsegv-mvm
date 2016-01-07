#ifndef _INCLUDE_SIGSEGV_UTIL_H_
#define _INCLUDE_SIGSEGV_UTIL_H_


#include "common.h"


#define SIZE_CHECK(_type, _size) static_assert(sizeof(_type) == _size, "sizeof(" #_type ") == " #_size)


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


template<typename T>
class AutoList
{
public:
	AutoList()
	{
		AllocateList();
		s_List->push_back(static_cast<T *>(this));
	}
	virtual ~AutoList()
	{
		assert(s_List != nullptr);
		s_List->remove(static_cast<T *>(this));
	}
	
	static const std::list<T *>& List()
	{
		AllocateList();
		return *s_List;
	}
	
private:
	static void AllocateList()
	{
		if (s_List == nullptr) {
			s_List = new std::list<T *>();
		}
	}
	
	static std::list<T *> *s_List;
};
template<typename T> std::list<T *> *AutoList<T>::s_List = nullptr;


#endif
