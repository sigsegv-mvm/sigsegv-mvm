#include "stub/misc.h"


/* duplicate definition is fine; fixes linker errors */
ConVar r_visualizetraces( "r_visualizetraces", "0", FCVAR_CHEAT );


static StaticFuncThunk<CRConClient&> ft_RCONClient("RCONClient");
CRConClient& RCONClient() { return ft_RCONClient(); }


StaticFuncThunk<void, const Vector&, trace_t&, const Vector&, const Vector&, CBaseEntity *> ft_FindHullIntersection("FindHullIntersection");


#if 0
StaticFuncThunk<const char *, const char *, int> TranslateWeaponEntForClass("TranslateWeaponEntForClass");
//const char *TranslateWeaponEntForClass(const char *name, int classnum)
//{
//	return ft_TranslateWeaponEntForClass(name, classnum);
//}
#endif
