#ifndef _INCLUDE_SIGSEGV_UTIL_TRACE_H_
#define _INCLUDE_SIGSEGV_UTIL_TRACE_H_


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


#endif
