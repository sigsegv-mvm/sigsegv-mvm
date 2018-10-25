#include "mod.h"
#include "stub/tfweaponbase.h"


namespace Mod::Sniper::Head_Uncap
{
	DETOUR_DECL_MEMBER(float, CTFSniperRifleDecap_SniperRifleChargeRateMod)
	{
		auto rifle = reinterpret_cast<CTFSniperRifleDecap *>(this);
		
		/* ordinarily this would be Min'd with 50.0f */
		return (rifle->GetCount() - 2) * 12.5f;
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Sniper:Head_Uncap")
		{
			MOD_ADD_DETOUR_MEMBER(CTFSniperRifleDecap_SniperRifleChargeRateMod, "CTFSniperRifleDecap::SniperRifleChargeRateMod");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_sniper_head_uncap", "0", FCVAR_NOTIFY,
		"Mod: remove the 6-head cap on the Bazaar Bargain",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
