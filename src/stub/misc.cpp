#include "stub/misc.h"


/* duplicate definition is fine; fixes linker errors */
ConVar r_visualizetraces( "r_visualizetraces", "0", FCVAR_CHEAT );


static StaticFuncThunk<CRConClient&> ft_RCONClient("RCONClient");
CRConClient& RCONClient() { return ft_RCONClient(); }


StaticFuncThunk<void, const Vector&, trace_t&, const Vector&, const Vector&, CBaseEntity *> ft_FindHullIntersection("FindHullIntersection");


#if 0
StaticFuncThunk<const char *, const char *, int> TranslateWeaponEntForClass("TranslateWeaponEntForClass");
//const char *TranslateWeaponEntForClass(const char *name, int classnum) { return ft_TranslateWeaponEntForClass(name, classnum); }
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


static MemberFuncThunk<const CStudioHdr *, int> ft_CStudioHdr_GetNumPoseParameters("CStudioHdr::GetNumPoseParameters");
int CStudioHdr::GetNumPoseParameters() const { return ft_CStudioHdr_GetNumPoseParameters(this); }

static MemberFuncThunk<CStudioHdr *, const mstudioposeparamdesc_t&, int> ft_CStudioHdr_pPoseParameter("CStudioHdr::pPoseParameter");
const mstudioposeparamdesc_t& CStudioHdr::pPoseParameter(int i) { return ft_CStudioHdr_pPoseParameter(this, i); }


static StaticFuncThunk<string_t, const char *> ft_AllocPooledString("AllocPooledString");
string_t AllocPooledString(const char *pszValue) { return ft_AllocPooledString(pszValue); }

static StaticFuncThunk<string_t, const char *> ft_AllocPooledString_StaticConstantStringPointer("AllocPooledString_StaticConstantStringPointer");
string_t AllocPooledString_StaticConstantStringPointer(const char *pszGlobalConstValue) { return ft_AllocPooledString_StaticConstantStringPointer(pszGlobalConstValue); }

static StaticFuncThunk<string_t, const char *> ft_FindPooledString("FindPooledString");
string_t FindPooledString(const char *pszValue) { return ft_FindPooledString(pszValue); }

static StaticFuncThunk<IGameSystem *> ft_GameStringSystem("GameStringSystem");
IGameSystem *GameStringSystem() { return ft_GameStringSystem(); }


static StaticFuncThunk<const char *, uint64> ft_CSteamID_Render_static("CSteamID::Render [static]");
const char *CSteamID::Render(uint64 ulSteamID) { return ft_CSteamID_Render_static(ulSteamID); }

static MemberFuncThunk<const CSteamID *, const char *> ft_CSteamID_Render_member("CSteamID::Render [member]");
const char *CSteamID::Render() const { return ft_CSteamID_Render_member(this); }
