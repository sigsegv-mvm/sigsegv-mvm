#include "mod.h"
#include "stub/tfweaponbase.h"
#include "util/scope.h"


namespace Mod::Etc::Melee_Ignore_Teammates
{
	CTFWeaponBaseMelee *melee = nullptr;
	CTFPlayer *owner = nullptr;
	bool is_whip = false;
	
	RefCount rc_CTFWeaponBaseMelee_DoSwingTraceInternal;
	DETOUR_DECL_MEMBER(bool, CTFWeaponBaseMelee_DoSwingTraceInternal, CGameTrace& tr, bool cleave_attack, CUtlVector<CGameTrace> *traces)
	{
		auto weapon = reinterpret_cast<CTFWeaponBaseMelee *>(this);
		
		bool result;
		if (!cleave_attack) {
			melee   = weapon;
			owner   = (weapon != nullptr ? weapon->GetTFPlayerOwner() : nullptr);
			is_whip = (CAttributeManager::AttribHookValue<int>(0, "speed_buff_ally", weapon) > 0);
			
			SCOPED_INCREMENT(rc_CTFWeaponBaseMelee_DoSwingTraceInternal);
			result = DETOUR_MEMBER_CALL(CTFWeaponBaseMelee_DoSwingTraceInternal)(tr, cleave_attack, traces);
			
			melee   = nullptr;
			owner   = nullptr;
			is_whip = false;
		} else {
			result = DETOUR_MEMBER_CALL(CTFWeaponBaseMelee_DoSwingTraceInternal)(tr, cleave_attack, traces);
		}
		
		return result;
	}
	
	
	RefCount rc_FindHullIntersection;
	DETOUR_DECL_STATIC(void, FindHullIntersection, const Vector& vecSrc, trace_t& tr, const Vector& mins, const Vector& maxs, CBaseEntity *pEntity)
	{
		SCOPED_INCREMENT(rc_FindHullIntersection);
		DETOUR_STATIC_CALL(FindHullIntersection)(vecSrc, tr, mins, maxs, pEntity);
	}
	
	
	DETOUR_DECL_MEMBER(void, IEngineTrace_TraceRay, const Ray_t& ray, unsigned int fMask, ITraceFilter *pTraceFilter, trace_t *pTrace)
	{
		if (rc_CTFWeaponBaseMelee_DoSwingTraceInternal > 0) {
			// doing fallback stuff
			if (rc_FindHullIntersection > 0) {
				// if it's a CTraceFilterSimple and we're called from within FindHullIntersection, then we're doing fallback stuff
				// ...
			}
			
			// if it's a CTraceFilterIgnorePlayers, then we're doing the Homewrecker thing
			// and we should leave it alone
			
			// if it's a CTraceFilterIgnoreTeammates, then we're doing the regular trace for MvM robots
			// ...
			
			// if it's a CTraceFilterIgnoreFriendlyCombatItems, then we're doing the regular trace for non-MvM-robots
			// ...
			
			#warning TODO
			#warning TODO
			#warning TODO
		}
		
		DETOUR_MEMBER_CALL(IEngineTrace_TraceRay)(ray, fMask, pTraceFilter, pTrace);
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Etc:Melee_Ignore_Teammates")
		{
			MOD_ADD_DETOUR_MEMBER(CTFWeaponBaseMelee_DoSwingTraceInternal, "CTFWeaponBaseMelee::DoSwingTraceInternal");
			
			MOD_ADD_DETOUR_STATIC(FindHullIntersection, "FindHullIntersection");
			
			MOD_ADD_DETOUR_MEMBER(IEngineTrace_TraceRay, "IEngineTrace::TraceRay");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_etc_melee_ignore_teammates", "0", FCVAR_NOTIFY,
		"Mod: allow melee traces to pass through teammates (for anyone, not just MvM blu team players)",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue){
			s_Mod.Toggle(static_cast<ConVar *>(pConVar)->GetBool());
		});
}
