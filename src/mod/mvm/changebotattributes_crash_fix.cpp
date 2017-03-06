#include "mod.h"
#include "stub/tfbot.h"


namespace Mod_MvM_ChangeBotAttributes_Crash_Fix
{
	DETOUR_DECL_STATIC(int, CollectPlayers_CTFBot, CUtlVector<CTFBot *> *playerVector, int team, bool isAlive, bool shouldAppend)
	{
		if (!shouldAppend) {
			playerVector->RemoveAll();
		}
		
		for (int i = 1; i <= gpGlobals->maxClients; ++i) {
			CBasePlayer *player = UTIL_PlayerByIndex(i);
			if (player == nullptr)                                   continue;
			if (FNullEnt(player->edict()))                           continue;
			if (!player->IsPlayer())                                 continue;
			if (!player->IsConnected())                              continue;
			if (team != TEAM_ANY && player->GetTeamNumber() != team) continue;
			if (isAlive && !player->IsAlive())                       continue;
			
			/* actually confirm that they're a CTFBot */
			CTFBot *bot = ToTFBot(player);
			if (bot == nullptr) continue;
			
			playerVector->AddToTail(bot);
		}
		
		return playerVector->Count();
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("MvM:ChangeBotAttributes_Crash_Fix")
		{
			MOD_ADD_DETOUR_STATIC(CollectPlayers_CTFBot, "CollectPlayers<CTFBot>");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_mvm_changebotattributes_crash_fix", "0", FCVAR_NOTIFY,
		"Mod: fix crash in which CPointPopulatorInterface::InputChangeBotAttributes assumes that all blue players are TFBots",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
