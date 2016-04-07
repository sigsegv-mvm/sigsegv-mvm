#include "mod.h"
#include "re/nextbot.h"
#include "re/path.h"
#include "stub/tfbot.h"
#include "stub/gamerules.h"
#include "stub/tfbot_behavior.h"
#include "stub/entities.h"

#include "mod/ai/mvm_defender_bots_helpers.h"
#include "mod/ai/mvm_defender_bots_purchaseupgrades.h"
#include "mod/ai/mvm_defender_bots_gotoupgradestation.h"
#include "mod/ai/mvm_defender_bots_collectmoney.h"
#include "mod/ai/mvm_defender_bots_scout.h"


#define TRACE_ENABLE 1
#define TRACE_TERSE  0
#include "util/trace.h"


namespace Mod_AI_MvM_Defender_Bots
{
	// TODO: is there ever any need to override/spoof TFGameRules()->GetGameType()?
	
	
	DETOUR_DECL_MEMBER(Action<CTFBot> *, CTFBotScenarioMonitor_DesiredScenarioAndClassAction, CTFBot *actor)
	{
		
		if (TFGameRules()->IsMannVsMachineMode() && actor->GetTeamNumber() == TF_TEAM_RED) {
			if (actor->IsPlayerClass(TF_CLASS_SCOUT)) {
				return new CTFBotMvMDefenderScout();
			}
			
			if (actor->IsPlayerClass(TF_CLASS_ENGINEER)) {
				return CTFBotEngineerBuild::New();
			}
			
			if (actor->IsPlayerClass(TF_CLASS_MEDIC)) {
				return CTFBotMedicHeal::New();
			}
			
			if (actor->IsPlayerClass(TF_CLASS_SPY)) {
				return CTFBotSpyInfiltrate::New();
			}
			
			if (actor->IsPlayerClass(TF_CLASS_SNIPER)) {
				auto weapon = actor->GetActiveTFWeapon();
				if (weapon != nullptr && WeaponID_IsSniperRifle(weapon->GetWeaponID())) {
					return CTFBotSniperLurk::New();
				}
			}
			
			return CTFBotSeekAndDestroy::New();
		}
		
		return DETOUR_MEMBER_CALL(CTFBotScenarioMonitor_DesiredScenarioAndClassAction)(actor);
	}
	
	
	// do we need to override actor->IsAllowedToPickUpFlag to false?
	// do we need to throw in some CTFBotDefendPoint for gate defense?
	
	
	DETOUR_DECL_MEMBER(float, CTFBot_GetTimeLeftToCapture)
	{
		if (TFGameRules()->IsMannVsMachineMode()) {
			return FLT_MAX;
		}
		
		return DETOUR_MEMBER_CALL(CTFBot_GetTimeLeftToCapture)();
	}
	
	
	DETOUR_DECL_MEMBER(void, CTFPlayer_HandleCommand_JoinTeam, const char *team)
	{
		DETOUR_MEMBER_CALL(CTFPlayer_HandleCommand_JoinTeam)(team);
		
		auto player = reinterpret_cast<CTFPlayer *>(this);
		
		if (TFGameRules()->IsMannVsMachineMode() && player->GetTeamNumber() == TF_TEAM_RED) {
			CTFBot *bot = ToTFBot(player);
			if (bot != nullptr) {
				(void)bot->GetAutoTeam(5);
			}
		}
	}
	
	
	/* ENHANCEMENT: make defender bots always aware of the bomb carrier(s) as
	 * well as tanks */
	void Enhancement_NotifyDefendersAboutBombs()
	{
		std::set<CBaseEntity *> bombs;
		
		ForEachFlag([&](CCaptureFlag *flag, bool& done){
			CBaseEntity *owner = flag->GetOwnerEntity();
			if (owner == nullptr) return true;
			
			if (owner->GetTeamNumber() != TF_TEAM_BLUE) return true;
			
			CTFBot *carrier = ToTFBot(owner);
			if (carrier == nullptr) return true;
			
			bombs.insert(carrier);
			
			return true;
		});
		
		ForEachTank([&](CTFTankBoss *tank, bool& done){
			if (tank->GetTeamNumber() == TF_TEAM_BLUE) {
				bombs.insert(tank);
			}
			
			return true;
		});
		
		if (bombs.empty()) return;
		
		ForEachDefenderBot([&](CTFBot *bot, bool& done){
			if (!bot->IsAlive()) return true;
			
			IVision *vision = bot->GetVisionInterface();
			for (auto bomb : bombs) {
				vision->AddKnownEntity(bomb);
			}
			
			return true;
		});
	}
	
	
	DETOUR_DECL_MEMBER(ActionResult<CTFBot>, CTFBotTacticalMonitor_OnCommandString, CTFBot *actor, const char *cmd)
	{
		if (V_stricmp(cmd, "gotoupgradestation") == 0) {
			return ActionResult<CTFBot>::SuspendFor(new CTFBotGoToUpgradeStation(), "Going to an upgrade station.");
		}
		if (V_stricmp(cmd, "purchaseupgrades") == 0) {
			return ActionResult<CTFBot>::SuspendFor(new CTFBotPurchaseUpgrades(), "Purchasing upgrades.");
		}
		
		return DETOUR_MEMBER_CALL(CTFBotTacticalMonitor_OnCommandString)(actor, cmd);
	}
	
	
	void GiveLoadout(CTFBot *bot)
	{
		switch (bot->GetPlayerClass()->GetClassIndex()) {
		case TF_CLASS_SCOUT:
			bot->AddItem("Mad Milk");
			bot->AddItem("The Fan O'War");
			break;
		case TF_CLASS_SOLDIER:
			bot->AddItem("The Buff Banner");
			break;
		case TF_CLASS_PYRO:
			bot->AddItem("The Flare Gun");
			break;
		case TF_CLASS_DEMOMAN:
			break;
		case TF_CLASS_HEAVYWEAPONS:
			bot->AddItem("The Sandvich");
			break;
		case TF_CLASS_ENGINEER:
			break;
		case TF_CLASS_MEDIC:
			bot->AddItem("The Crusader's Crossbow");
			bot->AddItem("The Kritzkrieg");
			bot->AddItem("The Ubersaw");
			break;
		case TF_CLASS_SNIPER:
			bot->AddItem("Jarate");
			break;
		case TF_CLASS_SPY:
			break;
		}
	}
	
	
	DETOUR_DECL_MEMBER(void, CTFBot_Spawn)
	{
		DETOUR_MEMBER_CALL(CTFBot_Spawn)();
		
		auto bot = reinterpret_cast<CTFBot *>(this);
		if (bot->GetTeamNumber() == TF_TEAM_RED) {
			GiveLoadout(bot);
		}
	}
	
	
	void AddQuirks_IsBot(IMod *mod);
	void AddQuirks_IsBotOfType(IMod *mod);
	void AddQuirks_GetLastKnownArea(IMod *mod);
	void AddQuirks_MannVsMachineMode(IMod *mod);
	void AddQuirks_SteamID(IMod *mod);
	
	
	namespace CollectMoney
	{
		void AddDetours(IMod *mod);
	}
	
	
	class CMod : public IMod, public IFrameUpdateListener
	{
	public:
		CMod() : IMod("AI:MvM_Defender_Bots")
		{
			MOD_ADD_DETOUR_MEMBER(CTFBotScenarioMonitor_DesiredScenarioAndClassAction, "CTFBotScenarioMonitor::DesiredScenarioAndClassAction");
			
			AddQuirks_IsBot(this);
			AddQuirks_IsBotOfType(this);
			AddQuirks_GetLastKnownArea(this);
			AddQuirks_MannVsMachineMode(this);
			AddQuirks_SteamID(this);
			
			/* QUIRK: bots won't stay in SeekAndDestroy because they always
			* believe the round timer is about to expire */
			MOD_ADD_DETOUR_MEMBER(CTFBot_GetTimeLeftToCapture, "CTFBot::GetTimeLeftToCapture");
			
			/* QUIRK: bots don't get money when they join */
			/* QUIRK: bots remember upgrades when first joining */
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_HandleCommand_JoinTeam, "CTFPlayer::HandleCommand_JoinTeam");
			
			MOD_ADD_DETOUR_MEMBER(CTFBotTacticalMonitor_OnCommandString, "CTFBotTacticalMonitor::OnCommandString");
			
			MOD_ADD_DETOUR_MEMBER(CTFBot_Spawn, "CTFBot::Spawn");
			
			CollectMoney::AddDetours(this);
		}
		
		virtual void OnUnload() override
		{
			/* order matters! */
			
			CTFBotGoToUpgradeStation::UnloadAll();
			CTFBotPurchaseUpgrades::UnloadAll();
			
			CTFBotCollectMoney::UnloadAll();
			CTFBotMarkGiants::UnloadAll();
			CTFBotMvMDefenderScout::UnloadAll();
		}
		
		virtual bool ShouldReceiveFrameEvents() const override { return this->m_bEnabled; }
		
		virtual void FrameUpdatePostEntityThink() override
		{
			static long frame = 0;
			++frame;
			
			if (frame % 22 == 0) {
				Enhancement_NotifyDefendersAboutBombs();
			}
			
			UpdateVisibleCredits();
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
			this->m_bEnabled = enable;
		}
		
	private:
		bool m_bEnabled = false;
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_ai_mvm_defender_bots", "0", FCVAR_NOTIFY,
		"Mod: make red TFBots in MvM do things other than stand around uselessly",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}


// TODO: make all of this stuff work:
// - situationally use jars
// - situationally use sandviches in a way that isn't stupid
// - kritzkrieg
// - fan-o-war
// - if ShouldHurry and !ShouldAttack
//   - and bot has a "go faster" item of some sort (GRU, powerjack, etc)
//   - then use it at that time
// - for all opportunistically used items:
//   - add a new callback, "ShouldUseSituationalItem", so we can control when they're used better
// - DR spy?
// - spy stab giants repeatedly in an intelligent manner
// - phlogistinator


// TODO: make bots try to go to dispensers when they need health or ammo


// TODO: money and respecs when bots join
// need to call CTFPlayer::GetAutoTeam(5) and do nothing with the result
// (from CTFPlayer::HandleCommand_JoinTeam, if MvM mode is active and is a TFBot and is on team 2)


// should maybe warn with a stack trace whenever we push and the count was already at least 1,
// to avoid some surprise nested conditions...


// BUG: medic bots getting stuck in FetchFlag somehow

// TODO: bomb carrier should always be known to defender bots
// TODO: bomb carrier should be highest priority?
// TODO: same stuff for the tank

// TODO: check vision update frequency

// TODO: refill bots' ammo on wave start or end


// TODO: bots need to automatically always be readied-up
// TODO: give medic bots automatic Attributes ProjectileShield so they'll actually use it
// TODO: probably should make red engie bots ignore sentry hints entirely

// TODO: make sure red bots aren't being erroneously affected by:
// - func_nav_avoid
// - tags (including from CTriggerBotTag)

// wishlist:
// - bots buy upgrades (lol...)
//   - make them go to an upgrade station on first spawn and between waves
//   - give them fixed rough upgrade paths
// - auto-equip certain meta-ish items (to the extent that bots can actually use them)
// - scouts track down money?
//   - create new behavior for this that makes them path toward currency packs
//   - whenever a currency pack spawns, have TacticalMonitor suspend for this
//     - or maybe only do this when they run within a certain radius of a currency pack
//     - or maybe only when they see one (enable them as known entities etc)
//   - blinking packs are highest priority, red packs are lowest priority
//   - also prioritize larger packs
// - medics attempt to heal reanimators
// - medics use kritzkrieg intelligently
// - buy back into game if it's urgent
// - spy prioritization for giants
// - spy logic for using sapper on enemies


/* references to MvM gamemode / bots that may need some overriding:

FL_FAKECLIENT
=============
TODO

0x548 CBasePlayer::IsFakeClient
===============================
_ZN12CTFGameRules17IsSpawnPointValidEP11CBaseEntityP11CBasePlayerb21PlayerTeamSpawnMode_t
_ZN12CTFGameRules40PlayerReadyStatus_HaveMinPlayersToEnableEv

0x6ec CBasePlayer::IsBot
========================
_ZN11CBaseObject18FindSnapToBuildPosEPS_
_ZN11CBaseObject18FindSnapToBuildPosEPS_
_ZN12CTFGameRules16ChangePlayerNameEP9CTFPlayerPKc
_ZN12CTFGameRules18ClientDisconnectedEP7edict_t
_ZN12CTFGameRules18ClientDisconnectedEP7edict_t
_ZN12CTFGameRules40PlayerReadyStatus_HaveMinPlayersToEnableEv
_ZN13CObjectSapper5SpawnEv (OF_ALLOW_REPEAT_PLACEMENT)
_ZN13CTFWeaponBase20ApplyOnHitAttributesEP11CBaseEntityP9CTFPlayerRK15CTakeDamageInfo
_ZN14CWeaponMedigun18FindAndHealTargetsEv
_ZN18CPopulationManager8ResetMapEv
_ZN9CTFPlayer20GetMannVsMachineTeamEi
_ZN9CTFPlayer22HandleCommand_JoinTeamEPKc
_ZN9CTFPlayer22HandleCommand_JoinTeamEPKc

0x6f0 CBasePlayer::IsBotOfType
==============================
_ZN14CTriggerBotTag5TouchEP11CBaseEntity
_ZN18CTraceFilterObject15ShouldHitEntityEP13IHandleEntityi
_ZN18CTraceFilterObject15ShouldHitEntityEP13IHandleEntityi
_ZN18CTraceFilterObject15ShouldHitEntityEP13IHandleEntityi
_ZN25CTFBotAttackFlagDefenders6UpdateEP6CTFBotf
_ZNK12CFuncNavCost14IsApplicableToEP20CBaseCombatCharacter
_ZNK14CSpawnLocation15SelectSpawnAreaEv

0x6f4 CBasePlayer::GetBotType
=============================
no references

CTFGameRules::IsPVEModeActive
=============================
none left

CTFGameRules::m_bPlayingMannVsMachine
=====================================
_ZN12CTFBotAttack6UpdateEP6CTFBotf (MvM uses ChasePath instead of Path)
_ZN25CTFBotAttackFlagDefenders6UpdateEP6CTFBotf
_ZN17CTFBotDeliverFlag5OnEndEP6CTFBotP6ActionIS0_E
_ZN17CTFBotDeliverFlag9OnContactEP6CTFBotP11CBaseEntityP10CGameTrace
_ZN17CTFBotDeliverFlag15UpgradeOverTimeEP6CTFBot
_ZN17CTFBotDeliverFlag6UpdateEP6CTFBotf
_ZN15CTFBotFetchFlag6UpdateEP6CTFBotf
_ZN15CTFBotFetchFlag6UpdateEP6CTFBotf
_ZN15CTFBotFetchFlag6UpdateEP6CTFBotf
_ZN24CTeamplayRoundBasedRules17CheckRestartRoundEv
_ZN24CTeamplayRoundBasedRules20State_Think_TEAM_WINEv
_ZN24CTeamplayRoundBasedRules17CheckReadyRestartEv
_ZN24CTeamplayRoundBasedRules20State_Enter_PREROUNDEv
_ZN24CTeamplayRoundBasedRules34GetPotentialPlayersLobbyPlayerInfoER10CUtlVectorI17LobbyPlayerInfo_t10CUtlMemoryIS1_iEEb
_ZN24CTeamplayRoundBasedRules24AreLobbyPlayersConnectedEv
_ZN24CTeamplayRoundBasedRules26AreLobbyPlayersOnTeamReadyEi
_ZN18CTraceFilterObject15ShouldHitEntityEP13IHandleEntityi
_ZN18CTraceFilterObject15ShouldHitEntityEP13IHandleEntityi
_ZN12CTFGameRules17CheckRespawnWavesEv
_ZNK15CTFPlayerShared29CanRecieveMedigunChargeEffectE20medigun_charge_types
*/
