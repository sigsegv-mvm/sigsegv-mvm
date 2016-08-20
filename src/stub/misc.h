#ifndef _INCLUDE_SIGSEGV_STUB_MISC_H_
#define _INCLUDE_SIGSEGV_STUB_MISC_H_


#include "link/link.h"


class CTFBotMvMEngineerTeleportSpawn;
class CTFBotMvMEngineerBuildTeleportExit;
class CTFBotMvMEngineerBuildSentryGun;

class CFlaggedEntitiesEnum;

class CRConClient;

class CBaseEntity;


CRConClient& RCONClient();


#define DECL_FT_WRAPPER(ret, name) \
	template<typename... ARGS> ret name(ARGS&&... args) { return ft_ ## name(std::forward<ARGS>(args)...); }


extern StaticFuncThunk<void, const Vector&, trace_t&, const Vector&, const Vector&, CBaseEntity *> ft_FindHullIntersection;
DECL_FT_WRAPPER(void, FindHullIntersection);


#if 0
extern StaticFuncThunk<const char *, const char *, int> TranslateWeaponEntForClass;
//const char *TranslateWeaponEntForClass(const char *name, int classnum);
#endif


#endif
