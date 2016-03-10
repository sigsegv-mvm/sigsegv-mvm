#include "mod.h"
#include "stub/tfbot.h"


namespace Mod_Bot_Kill_Before_ForceSpec
{
	// called from:
	// CTFBotMissionSuicideBomber::Detonate
	DETOUR_DECL_MEMBER(void, CTFPlayer_ForceChangeTeam, int iTeamNum, bool b1)
	{
		auto bot = ToTFBot(reinterpret_cast<CTFPlayer *>(this));
		if (bot != nullptr && iTeamNum == TEAM_SPECTATOR && bot->GetTeamNumber() != TEAM_SPECTATOR && bot->IsAlive()) {
			bot->CommitSuicide(false, true);
		}
		
		DETOUR_MEMBER_CALL(CTFPlayer_ForceChangeTeam)(iTeamNum, b1);
	}
	
	
	// called from:
	// CWaveSpawnPopulator::ForceFinish
	// CTFGameRules::FireGameEvent
	// CSquadSpawner::Spawn
	// this one causes bad things to happen when wave failure occurs
	DETOUR_DECL_MEMBER(void, CTFBot_ChangeTeam, int iTeamNum, bool bAutoTeam, bool bSilent)
	{
		auto bot = reinterpret_cast<CTFBot *>(this);
		if (iTeamNum == TEAM_SPECTATOR && bot->GetTeamNumber() != TEAM_SPECTATOR && bot->IsAlive()) {
			bot->CommitSuicide(false, true);
		}
		
		DETOUR_MEMBER_CALL(CTFBot_ChangeTeam)(iTeamNum, bAutoTeam, bSilent);
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Bot:Kill_Before_ForceSpec")
		{
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_ForceChangeTeam, "CTFPlayer::ForceChangeTeam");
		//	MOD_ADD_DETOUR_MEMBER(CTFBot_ChangeTeam,         "CTFBot::ChangeTeam");
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_bot_kill_before_forcespec", "0", FCVAR_NOTIFY,
		"Mod: fix bugs caused by bots not being killed before being switched to spectator",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
