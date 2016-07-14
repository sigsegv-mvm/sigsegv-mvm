#include "stub/misc.h"


static StaticFuncThunk<CRConClient&> ft_RCONClient("RCONClient");
CRConClient& RCONClient() { return ft_RCONClient(); }


#if 0
StaticFuncThunk<const char *, const char *, int> TranslateWeaponEntForClass("TranslateWeaponEntForClass");
//const char *TranslateWeaponEntForClass(const char *name, int classnum)
//{
//	return ft_TranslateWeaponEntForClass(name, classnum);
//}
#endif
