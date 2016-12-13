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


static MemberFuncThunk<CTakeDamageInfo *, void, CBaseEntity *, CBaseEntity *, CBaseEntity *, const Vector&, const Vector&, float, int, int, Vector *> ft_CTakeDamageInfo_ctor5("CTakeDamageInfo::CTakeDamageInfo [C1 | overload 5]");
CTakeDamageInfo::CTakeDamageInfo(CBaseEntity *pInflictor, CBaseEntity *pAttacker, CBaseEntity *pWeapon, const Vector& damageForce, const Vector& damagePosition, float flDamage, int bitsDamageType, int iKillType, Vector *reportedPosition)
{
	ft_CTakeDamageInfo_ctor5(this, pInflictor, pAttacker, pWeapon, damageForce, damagePosition, flDamage, bitsDamageType, iKillType, reportedPosition);
}


static MemberFuncThunk<CTraceFilterSimple *, void, const IHandleEntity *, int, ShouldHitFunc_t> ft_CTraceFilterSimple_ctor("CTraceFilterSimple::CTraceFilterSimple [C1]");
CTraceFilterSimple::CTraceFilterSimple(const IHandleEntity *passedict, int collisionGroup, ShouldHitFunc_t pExtraShouldHitFunc)
{
	ft_CTraceFilterSimple_ctor(this, passedict, collisionGroup, pExtraShouldHitFunc);
}
