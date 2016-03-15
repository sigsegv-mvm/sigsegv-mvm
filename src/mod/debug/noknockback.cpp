#include "mod.h"


namespace Mod_Debug_NoKnockback
{
	DETOUR_DECL_MEMBER(void, CTFPlayer_ApplyPushFromDamage, const CTakeDamageInfo& info, Vector v1)
	{
		/* do nothing */
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Debug:NoKnockback")
		{
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_ApplyPushFromDamage, "CTFPlayer::ApplyPushFromDamage");
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_debug_noknockback", "0", FCVAR_NOTIFY,
		"Debug: disable all damage-induced knockback effects",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
