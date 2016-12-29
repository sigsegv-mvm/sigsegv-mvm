#include "mod.h"
#include "stub/baseplayer.h"
#include "util/clientmsg.h"


namespace Mod_Debug_Sticky_Detonation_Order
{
	int indent = 0;
	
	template<typename... ARGS>
	void ClientMsgIndent(const char *fmt, ARGS... args)
	{
		size_t fmt2_len = strlen(fmt) + (2 * indent) + 1;
		char *fmt2 = new char[fmt2_len];
		
		memset(fmt2, ' ', (2 * indent));
		V_strncpy(fmt2 + (2 * indent), fmt, fmt2_len);
		
		ClientMsg(fmt2, std::forward<ARGS>(args)...);
		
		delete[] fmt2;
	}
	
	
	DETOUR_DECL_MEMBER(int, CBaseEntity_TakeDamage, const CTakeDamageInfo& inputInfo)
	{
		ClientMsgIndent("CBaseEntity::TakeDamage\n");
		
		++indent;
		auto result = DETOUR_MEMBER_CALL(CBaseEntity_TakeDamage)(inputInfo);
		--indent;
		
		return result;
	}
	
	
	DETOUR_DECL_MEMBER(void, CTFWeaponBaseGrenadeProj_Explode, trace_t *pTrace, int bitsDamageType)
	{
		ClientMsgIndent("BEGIN CTFWeaponBaseGrenadeProj::Explode\n");
		
		++indent;
		DETOUR_MEMBER_CALL(CTFWeaponBaseGrenadeProj_Explode)(pTrace, bitsDamageType);
		--indent;
		
		ClientMsgIndent("END   CTFWeaponBaseGrenadeProj::Explode\n");
	}
	
	
	DETOUR_DECL_MEMBER(bool, CTFPipebombLauncher_DetonateRemotePipebombs, bool bFizzle)
	{
		ClientMsgIndent("BEGIN CTFPipebombLauncher::DetonateRemotePipebombs [bFizzle: %s]\n",
			(bFizzle ? "true" : "false"));
		
		++indent;
		auto result = DETOUR_MEMBER_CALL(CTFPipebombLauncher_DetonateRemotePipebombs)(bFizzle);
		--indent;
		
		ClientMsgIndent("END   CTFPipebombLauncher::DetonateRemotePipebombs [%s]\n",
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
