#include "mod.h"
#include "stub/tfbot.h"
#include "stub/tf_shareddefs.h"
#include "stub/entities.h"
#include "util/iterate.h"


namespace Mod_Bot_Spy_SelectRandomReachableEnemy
{
////////////////////////////////////////////////////////////////////////////////
////// The actual implementation in the game ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//	CTFPlayer *CTFBot::SelectRandomReachableEnemy()
//	{
//		CUtlVector<CTFPlayer *> alive_enemies;
//		CollectPlayers(&alive_enemies, bot->GetOpposingTFTeamNumber(), true);
//		
//		CUtlVector<CTFPlayer *> alive_enemies_not_in_spawn;
//		FOR_EACH_VEC(alive_enemies, i) {
//			CTFPlayer *enemy = alive_enemies[i];
//			
//			if (!PointInRespawnRoom(enemy, enemy->WorldSpaceCenter(), false)) {
//				alive_enemies_not_in_spawn.AddToTail(enemy);
//			}
//		}
//		
//		if (!alive_enemies_not_in_spawn.IsEmpty()) {
//			return alive_enemies_not_in_spawn.Random();
//		} else {
//			return nullptr;
//		}
//	}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
	
	
	bool IsSpyUndetectible(CTFPlayer *pPlayer, CTFPlayer *pSpy)
	{
		// return false if any of these are true:
		// - InCond(TF_COND_STEALTHED_BLINK)
		// - InCond(TF_COND_BURNING)
		// - InCond(TF_COND_BLEEDING)
		// - InCond(TF_COND_URINE)
		// - milked?
		// - wet?
		// - TODO: look thru entire cond list
		// ^^^^^ hang on, wait a sec, don't these only matter when stealthed, and not matter when disguised?
		
		// return true if both of these are true:
		// - IsStealthed()
		// - GetPercentInvisible() >= 0.75f
		
		// return true if any of these are true:
		// - InCond(TF_COND_DISGUISING)
		// - !InCond(TF_COND_DISGUISED)
		// - GetDisguiseTeam() != pPlayer->GetTeamNumber()
		
		
		// TODO:
		// - probably split this out into two separate functions:
		//   - IsSpyFullyStealthed
		//   - IsSpyFullyDisguised
	}
	
	
	
	
	
	bool ShouldBotNoticePlayer(CTFBot *bot, CTFPlayer *player)
	{
		/* teammates are always noticed */
		if (bot->GetTeamNumber() != player->GetTeamNumber()) return true;
		
		if (player->IsPlayerClass(TF_CLASS_SPY)) {
			
		}
		
		return true;
	}
	
	
	DETOUR_DECL_MEMBER(CTFPlayer *, CTFBot_SelectRandomReachableEnemy)
	{
		auto bot = reinterpret_cast<CTFBot *>(this);
		
		
	}
	
	
	// ISSUE: the callers of SelectRandomReachableEnemy may hang onto the enemy
	// they choose for a while; so if a spy is visible when a bot begins an AI
	// action, and then goes invisible, they might still get chased
	
	
	class CMod : public IMod, public IFrameUpdateListener
	{
	public:
		CMod() : IMod("Bot:Spy_SelectRandomReachableEnemy")
		{
			
		}
		
		virtual bool ShouldReceiveFrameEvents() const override { return this->IsEnabled(); }
		
		virtual void FrameUpdatePostEntityThink() override
		{
			static long frame = 0;
			if (++frame % 2 == 0) return;
			
		//	ForEachTFPlayer([](CTFPlayer *player){
		//		
		//	});
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_bot_spy_selectrandomreachableenemy", "0", FCVAR_NOTIFY,
		"Mod: debug/fix bad spy logic in CTFBot::SelectRandomReachableEnemy",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
