#include "mod.h"
#include "stub/tfplayer.h"


namespace Mod_AI_LookAt_OnePlayer
{
	ConVar cvar_name("sig_ai_lookat_oneplayer_name", "", FCVAR_NOTIFY,
		"Mod: player name to look at");
	
	
	DETOUR_DECL_MEMBER(bool, CTFBotVision_IsIgnored, CBaseEntity *ent)
	{
		if (ent != nullptr && ent->IsPlayer()) {
			auto player = static_cast<CBasePlayer *>(ent);
			const char *name = player->GetPlayerName();
			
			if (strstr(name, cvar_name.GetString()) != nullptr) {
				return DETOUR_MEMBER_CALL(CTFBotVision_IsIgnored)(ent);
			}
		}
		
		return true;
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("AI:LookAt_OnePlayer")
		{
			MOD_ADD_DETOUR_MEMBER(CTFBotVision_IsIgnored, "CTFBotVision::IsIgnored");
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_ai_lookat_oneplayer", "0", FCVAR_NOTIFY,
		"Mod: make bots only able to see one player",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
