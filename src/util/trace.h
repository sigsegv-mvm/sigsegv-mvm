#ifndef _INCLUDE_SIGSEGV_UTIL_TRACE_H_
#define _INCLUDE_SIGSEGV_UTIL_TRACE_H_


//#define TRACE_ENABLE 0
//#define TRACE_TERSE  0


class TraceLevel
{
public:
	static int Get() { return s_Level; }
	
	static void Increment() { ++s_Level; }
	static void Decrement() { --s_Level; }
	
private:
	static int s_Level;
};


[[gnu::format(printf, 1, 2)]]
inline void IndentMsg(const char *format, ...)
{
	char buf[1024];
	
	va_list va;
	va_start(va, format);
	vsnprintf(buf, sizeof(buf), format, va);
	va_end(va);
	
	DevMsg("%*s%s", 2 * TraceLevel::Get(), "", buf);
}


class ScopedTrace
{
public:
	ScopedTrace(const char *func_name, bool disable) :
		m_strFuncName(func_name), m_bDisabled(disable)
	{
		this->m_szEnterMsg[0] = '\0';
		this->m_szExitMsg[0]  = '\0';
	}
	~ScopedTrace()
	{
		this->Exit();
	}
	
	[[gnu::format(printf, 2, 3)]]
	void PrintEnterMsg(const char *format = nullptr, ...)
	{
		if (format != nullptr) {
			va_list va;
			va_start(va, format);
			vsnprintf(this->m_szEnterMsg, sizeof(this->m_szEnterMsg), format, va);
			va_end(va);
		}
		
		this->Enter();
	}
	
	[[gnu::format(printf, 2, 3)]]
	void SetExitMsg(const char *format = nullptr, ...)
	{
		if (format != nullptr) {
			va_list va;
			va_start(va, format);
			vsnprintf(this->m_szExitMsg, sizeof(this->m_szExitMsg), format, va);
			va_end(va);
		}
	}
	
private:
	void Enter()
	{
		if (this->m_bDisabled) return;
		
		assert(!this->m_bEntered);
		this->m_bEntered = true;
		
#if TRACE_TERSE
		constexpr auto strEnter = "";
#else
		constexpr auto strEnter = "ENTER ";
#endif
		
		ConColorMsg(Color(0xff, 0x00, 0xff, 0xff), "%*s%s",
			2 * TraceLevel::Get(), "", strEnter);
		ConColorMsg(Color(0x00, 0xff, 0x00, 0xff), "%s", this->m_strFuncName.c_str());
		DevMsg(" %s\n", this->m_szEnterMsg);
		
		TraceLevel::Increment();
	}
	
	void Exit()
	{
		if (this->m_bDisabled) return;
		
		assert(this->m_bEntered);
		this->m_bEntered = false;
		
		TraceLevel::Decrement();
		
#if !TRACE_TERSE
		ConColorMsg(Color(0xff, 0x00, 0xff, 0xff), "%*s%s",
			2 * TraceLevel::Get(), "", "EXIT  ");
		ConColorMsg(Color(0x00, 0xff, 0x00, 0xff), "%s", this->m_strFuncName.c_str());
		DevMsg(" %s\n", this->m_szExitMsg);
#endif
	}
	
	std::string m_strFuncName;
	
	char m_szEnterMsg[1024];
	char m_szExitMsg[1024];
	
	bool m_bEntered  = false;
	bool m_bDisabled = false;
};


#if TRACE_ENABLE

#if defined __GNUC__

inline std::string GetTheActualFunctionName(const std::string& func, const std::string& pretty)
{
	/* thanks GCC for making me use this pile of shit
	 * http://stackoverflow.com/a/29856690 */
	
//	DevMsg("%s(\"%s\", \"%s\")\n", __FUNCTION__, func.c_str(), pretty.c_str());
	
	size_t l_func  = pretty.find(func);
	size_t l_begin = pretty.rfind(" ", l_func) + 1;
	size_t l_end   = pretty.find("(", l_func + func.length());
	
	return pretty.substr(l_begin, l_end - l_begin);
}

#define TRACE_FUNC_NAME GetTheActualFunctionName(__FUNCTION__, __PRETTY_FUNCTION__).c_str()

#else

/* my god, MSVC actually did something right here */
#define TRACE_FUNC_NAME __FUNCTION__

#endif

// TRACE_ENABLE = 1
#define TRACE(...)          ScopedTrace _trace(TRACE_FUNC_NAME, false);   _trace.PrintEnterMsg(__VA_ARGS__)
#define TRACE_IF(pred, ...) ScopedTrace _trace(TRACE_FUNC_NAME, !(pred)); _trace.PrintEnterMsg(__VA_ARGS__)
#define TRACE_EXIT(...)     _trace.SetExitMsg(__VA_ARGS__)
#define TRACE_MSG(...)      IndentMsg(__VA_ARGS__)

#else

// TRACE_ENABLE = 0
#define TRACE(...)
#define TRACE_IF(...)
#define TRACE_EXIT(...)
#define TRACE_MSG(...)

#endif


#endif
