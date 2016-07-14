#include "mod.h"
#include "mod/ai/mvm_defender_bots/helpers.h"
#include "mod/ai/mvm_defender_bots/actions/prewave.h"


// console commands
// client-side cmd: "player_ready_toggle"
// server-side cmd: "tournament_player_readystate %d"

// to check if a player is ready:
// TeamplayRoundBasedRules()->IsPlayerReady(ENTINDEX(player))

// to toggle ready status:
// TFGameRules()->PlayerReadyStatus_UpdatePlayerState(player, bool)

// CTeamplayRoundBasedRules
// 0x385 bool  m_bAwaitingReadyRestart
// 0x388 float m_flRestartRoundTime
// 0x410 bool  m_bTeamReady[32]
// 0x432 bool  m_bPlayerReady[33]
// 0x458 float m_flCountdownTime



// TODO:
// integrate this back into CTFBotMvMDefender, as a special case (check TFGameRules()->InSetup() or whatever it is exactly)
// we can't have this be separate, because we don't want to suspend child actions
// (e.g. medic heal, engie build, whatever)



namespace Mod_AI_MvM_Defender_Bots
{
	ActionResult<CTFBot> CTFBotPreWave::OnStart(CTFBot *actor, Action<CTFBot> *action)
	{
		return ActionResult<CTFBot>::Continue();
	}
	
	ActionResult<CTFBot> CTFBotPreWave::Update(CTFBot *actor, float dt)
	{
		TFGameRules()->PlayerReadyStatus_UpdatePlayerState(actor, this->IsReady(actor));
		
		return ActionResult<CTFBot>::Continue();
	}
	
	void CTFBotPreWave::OnEnd(CTFBot *actor, Action<CTFBot> *action)
	{
		
	}
	
	
	bool CTFBotPreWave::IsReady(CTFBot *actor)
	{
		/* medic: only ready up when fully charged */
		if (actor->IsPlayerClass(TF_CLASS_MEDIC)) {
			return (actor->MedicGetChargeLevel() >= 1.00f);
		}
		
		// TODO: engineer: only ready up when stuff is built
		// TODO: demo: only ready up when stickies are down, if applicable
		
		// TODO: all classes: only ready up when upgrades are purchased and
		// they've finished moving up to the front area
		
		return true;
	}
}


// QUIRKS

// 54c IsFakeClient
// 6f0 IsBot
// 6f4 IsBotOfType
// 6f8 GetBotType

// TODO
// CTFGCServerSystem::PreClientUpdate
// updates sv_visiblemaxplayers
// 1 instance of IsBot()
// might need a tweak? check this
