#include "mod.h"


namespace Mod_Debug_NoKnockback
{
	DETOUR_DECL_MEMBER(void, CTFPlayer_ApplyPushFromDamage, const CTakeDamageInfo& info, Vector v1)
	{
		/* do nothing */
	}
	
	
	ConVar cvar_airblast("sig_debug_noknockback_airblast", "0", FCVAR_NOTIFY,
		"Debug: also disable ApplyAirBlastImpulse");
	
	DETOUR_DECL_MEMBER(void, CTFPlayer_ApplyAirBlastImpulse, const Vector& impulse)
	{
		if (!cvar_airblast.GetBool()) {
			DETOUR_MEMBER_CALL(CTFPlayer_ApplyAirBlastImpulse)(impulse);
		}
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Debug:NoKnockback")
		{
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_ApplyPushFromDamage, "CTFPlayer::ApplyPushFromDamage");
			
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_ApplyAirBlastImpulse, "CTFPlayer::ApplyAirBlastImpulse");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_debug_noknockback", "0", FCVAR_NOTIFY,
		"Debug: disable all damage-induced knockback effects",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
