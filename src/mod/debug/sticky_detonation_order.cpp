#include "mod.h"
#include "stub/baseplayer.h"


namespace Mod_Debug_Sticky_Detonation_Order
{
	int indent = 0;
	
	
	void ClientMsg(const char *format, ...)
	{
		static char buf[4096];
		
		va_list va;
		va_start(va, format);
		vsnprintf(buf, sizeof(buf), format, va);
		va_end(va);
		
		for (int i = 1; i <= 32; ++i) {
			CBasePlayer *player = UTIL_PlayerByIndex(i);
			if (player != nullptr) {
				g_SMAPI->ClientConPrintf(player->GetNetworkable()->GetEdict(), "%*s%s", indent * 2, " ", buf);
			}
		}
	}
	
	
	DETOUR_DECL_MEMBER(int, CBaseEntity_TakeDamage, const CTakeDamageInfo& inputInfo)
	{
		ClientMsg("CBaseEntity::TakeDamage\n");
		
		++indent;
		auto result = DETOUR_MEMBER_CALL(CBaseEntity_TakeDamage)(inputInfo);
		--indent;
		
		return result;
	}
	
	
	DETOUR_DECL_MEMBER(void, CTFWeaponBaseGrenadeProj_Explode, trace_t *pTrace, int bitsDamageType)
	{
		ClientMsg("BEGIN CTFWeaponBaseGrenadeProj::Explode\n");
		
		++indent;
		DETOUR_MEMBER_CALL(CTFWeaponBaseGrenadeProj_Explode)(pTrace, bitsDamageType);
		--indent;
		
		ClientMsg("END   CTFWeaponBaseGrenadeProj::Explode\n");
	}
	
	
	DETOUR_DECL_MEMBER(bool, CTFPipebombLauncher_DetonateRemotePipebombs, bool bFizzle)
	{
		ClientMsg("BEGIN CTFPipebombLauncher::DetonateRemotePipebombs [bFizzle: %s]\n",
			(bFizzle ? "true" : "false"));
		
		++indent;
		auto result = DETOUR_MEMBER_CALL(CTFPipebombLauncher_DetonateRemotePipebombs)(bFizzle);
		--indent;
		
		ClientMsg("END   CTFPipebombLauncher::DetonateRemotePipebombs [%s]\n",
			(result ? "true" : "false"));
		
		return result;
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Debug:Sticky_Detonation_Order")
		{
			MOD_ADD_DETOUR_MEMBER(CBaseEntity_TakeDamage,                      "CBaseEntity::TakeDamage");
			MOD_ADD_DETOUR_MEMBER(CTFWeaponBaseGrenadeProj_Explode,            "CTFWeaponBaseGrenadeProj::Explode");
			MOD_ADD_DETOUR_MEMBER(CTFPipebombLauncher_DetonateRemotePipebombs, "CTFPipebombLauncher::DetonateRemotePipebombs");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_debug_sticky_detonation_order", "0", FCVAR_NOTIFY,
		"Debug: diagnose weird effects due to stickybomb detonation order",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
