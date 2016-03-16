#include "mod.h"
#include "stub/tfplayer.h"
#include "stub/tf_shareddefs.h"


namespace Mod_Cond_Reprogrammed
{
	void OnAddReprogrammed(CTFPlayer *player)
	{
		DevMsg("OnAddReprogrammed(#%d \"%s\")\n", ENTINDEX(player), player->GetPlayerName());
		
		player->m_Shared->StunPlayer(5.0f, 0.65f, TF_STUNFLAG_NOSOUNDOREFFECT | TF_STUNFLAG_SLOWDOWN, nullptr);
		player->ForceChangeTeam(TF_TEAM_RED, false);
		
		/* this used to be in CTFPlayerShared::OnAddReprogrammed on the client
		 * side, but we now have to do it from the server side */
		DispatchParticleEffect("sapper_sentry1_fx", PATTACH_POINT_FOLLOW, player, "head");
	}
	
	void OnRemoveReprogrammed(CTFPlayer *player)
	{
		DevMsg("OnRemoveReprogrammed(#%d \"%s\")\n", ENTINDEX(player), player->GetPlayerName());
		
		player->ForceChangeTeam(TF_TEAM_BLUE, false);
		
		/* this is far from ideal; we can only remove ALL particle effects from
		 * the server side */
		StopParticleEffects(player);
	}
	
	
	DETOUR_DECL_MEMBER(void, CTFPlayerShared_OnConditionAdded, ETFCond cond)
	{
		if (cond == TF_COND_REPROGRAMMED) {
			auto shared = reinterpret_cast<CTFPlayerShared *>(this);
			OnAddReprogrammed(shared->GetOuter());
		} else {
			DETOUR_MEMBER_CALL(CTFPlayerShared_OnConditionAdded)(cond);
		}
	}
	
	DETOUR_DECL_MEMBER(void, CTFPlayerShared_OnConditionRemoved, ETFCond cond)
	{
		if (cond == TF_COND_REPROGRAMMED) {
			auto shared = reinterpret_cast<CTFPlayerShared *>(this);
			OnRemoveReprogrammed(shared->GetOuter());
		} else {
			DETOUR_MEMBER_CALL(CTFPlayerShared_OnConditionRemoved)(cond);
		}
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Cond:Reprogrammed")
		{
			MOD_ADD_DETOUR_MEMBER(CTFPlayerShared_OnConditionAdded,   "CTFPlayerShared::OnConditionAdded");
			MOD_ADD_DETOUR_MEMBER(CTFPlayerShared_OnConditionRemoved, "CTFPlayerShared::OnConditionRemoved");
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_cond_reprogrammed", "0", FCVAR_NOTIFY,
		"Mod: reimplement TF_COND_REPROGRAMMED",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
