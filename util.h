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


#if 0
class ScopedPush
{
	
};
#define SCOPED_PUSH(stack, data) stack##_push(stack, (void *)data)
#endif


class TraceLevel
{
public:
	static int Get() { return s_Level; }
	
	static void Increment() { ++s_Level; }
	static void Decrement() { --s_Level; }
	
private:
	static int s_Level;
};


class ScopedTrace
{
public:
	ScopedTrace(const char *func_name, const char *format = "", ...) __attribute__((format(printf, 3, 4))) :
		m_pszFuncName(func_name)
	{
		this->m_szExitMsg[0] = '\0';
		
		va_list va;
		va_start(va, format);
		vsnprintf(this->m_szEnterMsg, sizeof(this->m_szEnterMsg), format, va);
		va_end(va);
		
		DevMsg("%*sENTER %s %s\n", 2 * TraceLevel::Get(), "", this->m_pszFuncName, this->m_szEnterMsg);
		TraceLevel::Increment();
	}
	virtual ~ScopedTrace()
	{
		TraceLevel::Decrement();
		DevMsg("%*sEXIT  %s %s\n", 2 * TraceLevel::Get(), "", this->m_pszFuncName, this->m_szExitMsg);
	}
	
	void SetExitMsg(const char *format, ...) __attribute__((format(printf, 2, 3)))
	{
		va_list va;
		va_start(va, format);
		vsnprintf(this->m_szExitMsg, sizeof(this->m_szExitMsg), format, va);
		va_end(va);
	}
	
private:
	const char *m_pszFuncName;
	
	char m_szEnterMsg[1024];
	char m_szExitMsg[1024];
};
#define TRACE(...) ScopedTrace _trace(__func__, __VA_ARGS__)
#define TRACE_EXIT(fmt, ...) _trace.SetExitMsg(fmt, __VA_ARGS__)


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
