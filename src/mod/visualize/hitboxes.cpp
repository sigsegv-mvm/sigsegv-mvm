#include "mod.h"
#include "stub/baseanimating.h"


namespace Mod_Visualize_Hitboxes
{
	ConVar cvar_interval("sig_visualize_hitboxes_interval", "2", FCVAR_NOTIFY,
		"Visualization: frame interval");
	
	
	void DrawHitboxes(CBaseAnimating *anim)
	{
		if (!anim->IsPlayer() && !anim->IsBaseObject()) return;
		if (anim->GetTeamNumber() == TEAM_SPECTATOR)    return;
		if (!anim->IsAlive())                           return;
		
		anim->DrawServerHitboxes(cvar_interval.GetInt() * gpGlobals->interval_per_tick, false);
		
		// CBaseAnimating::DrawServerHitboxes
	#if 0
		CStudioHdr *pStudioHdr = anim->GetModelPtr();
		if (pStudioHdr == nullptr) return;
		
		mstudiohitboxset_t *set = pStudioHdr->pHitboxSet(anim->m_nHitboxSet);
		if (set == nullptr) return;
		
		for (int i = 0; i < set->numhitboxes; ++i) {
			
		}
	#endif
	}
	
	
	class CMod : public IMod, public IFrameUpdateListener
	{
	public:
		CMod() : IMod("Visualize:Hitboxes") {}
		
		virtual bool ShouldReceiveFrameEvents() const override { return this->IsEnabled(); }
		
		virtual void FrameUpdatePostEntityThink() override
		{
			static long frame = 0;
			if (++frame % cvar_interval.GetInt() != 0) return;
			
			for (int i = 1; i < MAX_EDICTS; ++i) {
				CBaseEntity *ent = UTIL_EntityByIndex(i);
				if (ent == nullptr) continue;
				
				auto anim = rtti_cast<CBaseAnimating *>(ent);
				if (anim == nullptr) continue;
				
				DrawHitboxes(anim);
			}
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_visualize_hitboxes", "0", FCVAR_NOTIFY,
		"Visualization: draw server-side hitboxes",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
