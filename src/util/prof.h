#ifndef _INCLUDE_SIGSEGV_UTIL_PROF_H_
#define _INCLUDE_SIGSEGV_UTIL_PROF_H_


// basic profiling class
// only works on Windows currently
// not reentrant at all!


#if defined _MSC_VER
class Prof
{
public:
	static void Begin()
	{
		s_bFail = false;
		
		if (!QueryPerformanceFrequency(&s_Freq)) s_bFail = true;
		if (!QueryPerformanceCounter(&s_Count1)) s_bFail = true;
	}
	static void End(const char *msg)
	{
		if (!QueryPerformanceCounter(&s_Count2)) s_bFail = true;
		
		if (s_bFail) {
			DevMsg("PROF %s: failed\n", msg);
		} else {
			int64_t freq  = s_Freq.QuadPart;
			int64_t delta = s_Count2.QuadPart - s_Count1.QuadPart;
			
			double sec = (double)delta / (double)freq;
			double msec = sec * 1000.0;
			
			DevMsg("PROF %s: %.3f ms\n", msg, msec);
		}
	}
	
private:
	static bool s_bFail;
	static LARGE_INTEGER s_Freq;
	static LARGE_INTEGER s_Count1;
	static LARGE_INTEGER s_Count2;
};
#endif


#endif
