#include "version.h"


ConVar cvar_build("sig_build", __DATE__ " " __TIME__, FCVAR_NONE);


const char *GetBuildDate()
{
	return __DATE__;
}

const char *GetBuildTime()
{
	return __TIME__;
}
