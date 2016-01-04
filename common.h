#ifndef _INCLUDE_SIGSEGV_COMMON_H_
#define _INCLUDE_SIGSEGV_COMMON_H_


/* C++ standard library */
#include <set>

/* Source SDK */
#include <tier0/dbg.h>
#include <tier1/convar.h>

/* SourceMod */
#include <smsdk_ext.h>
#include <CDetour/detours.h>

/* this extension */
#include "mod.h"


#define DETOUR_DECL_MEMBER_V(name, ret, ...) \
class name##Class \
{ \
public: \
	ret name(__VA_ARGS__); \
	static ret (name##Class::* name##_Actual)(__VA_ARGS__); \
}; \
ret (name##Class::* name##Class::name##_Actual)(__VA_ARGS__) = NULL; \
ret name##Class::name(__VA_ARGS__)


class CTFPlayer;


#endif
