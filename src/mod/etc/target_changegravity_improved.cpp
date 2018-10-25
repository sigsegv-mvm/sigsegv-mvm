#include "mod.h"
#include "stub/baseentity.h"


namespace Mod::Etc::Target_ChangeGravity_Improved
{
	DETOUR_DECL_MEMBER(void, CTargetChangeGravity_InputChangeGrav, inputdata_t& inputdata)
	{
		auto me = reinterpret_cast<CBaseEntity *>(this);
		
		CBaseEntity *them = inputdata.pActivator;
		if (them == nullptr) {
			DevMsg("[%8.3f] target_changegravity(#%d): InputChangeGrav: activator is null!\n", gpGlobals->curtime, ENTINDEX(me));
			return;
		}
		
		DevMsg("[%8.3f] target_changegravity(#%d): InputChangeGrav: activator #%d '%s' '%s' [%.3f --> %.3f]\n", gpGlobals->curtime, ENTINDEX(me),
			ENTINDEX(them), them->GetClassname(), STRING(them->GetEntityName()),
			them->GetGravity(), me->GetGravity());
		
		them->SetGravity(me->GetGravity());
	}
	
	DETOUR_DECL_MEMBER(void, CTargetChangeGravity_InputResetGrav, inputdata_t& inputdata)
	{
		auto me = reinterpret_cast<CBaseEntity *>(this);
		
		CBaseEntity *them = inputdata.pActivator;
		if (them == nullptr) {
			DevMsg("[%8.3f] target_changegravity(#%d): InputResetGrav:  activator is null!\n", gpGlobals->curtime, ENTINDEX(me));
			return;
		}
		
		DevMsg("[%8.3f] target_changegravity(#%d): InputResetGrav:  activator #%d '%s' '%s' [%.3f --> %.3f]\n", gpGlobals->curtime, ENTINDEX(me),
			ENTINDEX(them), them->GetClassname(), STRING(them->GetEntityName()),
			them->GetGravity(), 1.000f);
		
		them->SetGravity(1.000f);
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Etc:Target_ChangeGravity_Improved")
		{
			MOD_ADD_DETOUR_MEMBER(CTargetChangeGravity_InputChangeGrav, "CTargetChangeGravity::InputChangeGrav");
			MOD_ADD_DETOUR_MEMBER(CTargetChangeGravity_InputResetGrav,  "CTargetChangeGravity::InputResetGrav");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_etc_target_changegravity_improved", "0", FCVAR_NOTIFY,
		"Mod: improve the target_changegravity entity so that it's actually usable",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue){
			s_Mod.Toggle(static_cast<ConVar *>(pConVar)->GetBool());
		});
}
