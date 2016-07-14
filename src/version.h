#ifndef _INCLUDE_SIGSEGV_VERSION_H_
#define _INCLUDE_SIGSEGV_VERSION_H_


#if defined __cplusplus
extern "C" const char *GetBuildDate();
extern "C" const char *GetBuildTime();
#else
const char *GetBuildDate(void);
const char *GetBuildTime(void);
#endif


#endif
