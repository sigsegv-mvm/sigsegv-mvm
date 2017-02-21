#include "mod.h"
#include "stub/gamerules.h"
#include "stub/tfbot.h"
#include "stub/tf_shareddefs.h"
#include "util/clientmsg.h"
#include "util/iterate.h"

// TODO: move to common.h
#include <igamemovement.h>
#include <in_buttons.h>


namespace Mod_MvM_JoinTeam_Blue_Allow
{
	ConVar cvar_max("sig_mvm_jointeam_blue_allow_max", "-1", FCVAR_NOTIFY,
		"Blue humans in MvM: max humans to allow on blue team (-1 for no limit)");
	
	ConVar cvar_spawn_protection("sig_mvm_bluhuman_spawn_protection", "1", FCVAR_NOTIFY,
		"Blue humans in MvM: enable spawn protection invulnerability");
	ConVar cvar_spawn_no_shoot("sig_mvm_bluhuman_spawn_noshoot", "1", FCVAR_NOTIFY,
		"Blue humans in MvM: when spawn protection invulnerability is enabled, disallow shooting from spawn");
	ConVar cvar_infinite_cloak("sig_mvm_bluhuman_infinite_cloak", "1", FCVAR_NOTIFY,
		"Blue humans in MvM: enable infinite spy cloak meter");
	ConVar cvar_infinite_cloak_deadringer("sig_mvm_bluhuman_infinite_cloak_deadringer", "0", FCVAR_NOTIFY,
		"Blue humans in MvM: enable infinite spy cloak meter (Dead Ringer)");
	
	
	bool IsMvMBlueHuman(CTFPlayer *player)
	{
		if (player == nullptr)                       return false;
		if (player->GetTeamNumber() != TF_TEAM_BLUE) return false;
		if (player->IsBot())                         return false;
		if (!TFGameRules()->IsMannVsMachineMode())   return false;
		
		return true;
	}
	
	int GetMvMBlueHumanCount()
	{
		int count = 0;
		
		ForEachTFPlayer([&](CTFPlayer *player){
			if (IsMvMBlueHuman(player)) {
				++count;
			}
		});
		
		return count;
	}
	
	bool IsInBlueSpawnRoom(CTFPlayer *player)
	{
		player->UpdateLastKnownArea();
		auto area = static_cast<CTFNavArea *>(player->GetLastKnownArea());
		return (area != nullptr && area->HasTFAttributes(BLUE_SPAWN_ROOM));
	}
	
	
	DETOUR_DECL_MEMBER(int, CTFGameRules_GetTeamAssignmentOverride, CTFPlayer *pPlayer, int iWantedTeam, bool b1)
	{
		/* it's important to let the call happen, because pPlayer->m_nCurrency
		 * is set to its proper value in the call (stupid, but whatever) */
		auto iResult = DETOUR_MEMBER_CALL(CTFGameRules_GetTeamAssignmentOverride)(pPlayer, iWantedTeam, b1);
		
		if (TFGameRules()->IsMannVsMachineMode() && iWantedTeam == TF_TEAM_BLUE && iResult != iWantedTeam) {
			if (cvar_max.GetInt() < 0 || GetMvMBlueHumanCount() < cvar_max.GetInt()) {
				DevMsg("Player #%d \"%s\" requested team %d but was forced onto team %d; overriding to allow them to join team %d.\n",
					ENTINDEX(pPlayer), pPlayer->GetPlayerName(), iWantedTeam, iResult, iWantedTeam);
				iResult = iWantedTeam;
			} else {
				DevMsg("Player #%d \"%s\" requested team %d but was forced onto team %d; would have overridden to allow joining team %d but limit has been met.\n",
					ENTINDEX(pPlayer), pPlayer->GetPlayerName(), iWantedTeam, iResult, iWantedTeam);
				ClientMsg(pPlayer, "Cannot join team blue: the maximum number of human players on blue team has already been met.\n");
			}
		}
		
		return iResult;
	}
	
	
	DETOUR_DECL_STATIC(CTFReviveMarker *, CTFReviveMarker_Create, CTFPlayer *player)
	{
		if (IsMvMBlueHuman(player)) {
			return nullptr;
		}
		
		return DETOUR_STATIC_CALL(CTFReviveMarker_Create)(player);
	}
	
	DETOUR_DECL_MEMBER(bool, CTFPlayer_ClientCommand, const CCommand& args)
	{
		auto player = reinterpret_cast<CTFPlayer *>(this);
		
		if (FStrEq(args[0], "upgrade")) {
			if (IsMvMBlueHuman(player) && IsInBlueSpawnRoom(player)) {
				player->m_Shared->m_bInUpgradeZone = true;
			}
			
			return true;
		}
		
		return DETOUR_MEMBER_CALL(CTFPlayer_ClientCommand)(args);
	}
	
	DETOUR_DECL_MEMBER(void, CTFPlayer_OnNavAreaChanged, CNavArea *enteredArea, CNavArea *leftArea)
	{
		auto player = reinterpret_cast<CTFPlayer *>(this);
		
		DETOUR_MEMBER_CALL(CTFPlayer_OnNavAreaChanged)(enteredArea, leftArea);
		
		if (IsMvMBlueHuman(player) &&
			(enteredArea == nullptr ||  static_cast<CTFNavArea *>(enteredArea)->HasTFAttributes(BLUE_SPAWN_ROOM)) &&
			(leftArea    == nullptr || !static_cast<CTFNavArea *>(leftArea)   ->HasTFAttributes(BLUE_SPAWN_ROOM))) {
			player->m_Shared->m_bInUpgradeZone = false;
		}
	}
	
	DETOUR_DECL_MEMBER(void, CTFGameRules_ClientCommandKeyValues, edict_t *pEntity, KeyValues *pKeyValues)
	{
		if (FStrEq(pKeyValues->GetName(), "MvM_UpgradesDone")) {
			CTFPlayer *player = ToTFPlayer(GetContainingEntity(pEntity));
			if (IsMvMBlueHuman(player)) {
				player->m_Shared->m_bInUpgradeZone = false;
			}
		}
		
		DETOUR_MEMBER_CALL(CTFGameRules_ClientCommandKeyValues)(pEntity, pKeyValues);
	}
	
	
//	DETOUR_DECL_MEMBER(void, CPlayerMove_StartCommand, CBasePlayer *player, CUserCmd *ucmd)
//	{
//		DETOUR_MEMBER_CALL(CPlayerMove_StartCommand)(player, ucmd);
//		
//		DevMsg("CPlayerMove::StartCommand(#%d): buttons = %08x\n", ENTINDEX(player), ucmd->buttons);
//		
//		/* ideally we'd either do this or not do this based on the value of
//		 * CanBotsAttackWhileInSpawnRoom in g_pPopulationManager, but tracking
//		 * down the offset of that boolean is more work than it's worth */
//		CTFPlayer *tfplayer = ToTFPlayer(player);
//		if (cvar_spawn_protection.GetBool() && cvar_spawn_no_shoot.GetBool() && IsMvMBlueHuman(tfplayer) && IsInBlueSpawnRoom(tfplayer)) {
//			ucmd->buttons &= ~(IN_ATTACK | IN_ATTACK2 | IN_ATTACK3);
//			DevMsg("- stripped attack buttons: %08x\n", ucmd->buttons);
//		}
//	}
	
	
	// TODO: fix fast engie re-deploy only working on red
	
	
	class CMod : public IMod, public IFrameUpdateListener
	{
	public:
		CMod() : IMod("MvM:JoinTeam_Blue_Allow")
		{
			MOD_ADD_DETOUR_MEMBER(CTFGameRules_GetTeamAssignmentOverride, "CTFGameRules::GetTeamAssignmentOverride");
			
			MOD_ADD_DETOUR_STATIC(CTFReviveMarker_Create,                 "CTFReviveMarker::Create");
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_ClientCommand,                "CTFPlayer::ClientCommand");
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_OnNavAreaChanged,             "CTFPlayer::OnNavAreaChanged");
			MOD_ADD_DETOUR_MEMBER(CTFGameRules_ClientCommandKeyValues,    "CTFGameRules::ClientCommandKeyValues");
			
		//	MOD_ADD_DETOUR_MEMBER(CPlayerMove_StartCommand,               "CPlayerMove::StartCommand");
		}
		
		virtual bool ShouldReceiveFrameEvents() const override { return this->IsEnabled(); }
		
		virtual void FrameUpdatePostEntityThink() override
		{
			if (TFGameRules()->IsMannVsMachineMode()) {
				ForEachTFPlayer([](CTFPlayer *player){
					if (player->GetTeamNumber() != TF_TEAM_BLUE) return;
					if (player->IsBot())                         return;
					
					if (cvar_spawn_protection.GetBool()) {
						if (IsInBlueSpawnRoom(player)) {
							player->m_Shared->AddCond(TF_COND_INVULNERABLE_HIDE_UNLESS_DAMAGED, 0.500f);
							
							if (cvar_spawn_no_shoot.GetBool()) {
								player->m_Shared->m_flStealthNoAttackExpire = gpGlobals->curtime + 0.500f;
								// alternative method: set m_Shared->m_bFeignDeathReady to true
							}
						}
					}
					
					if (cvar_infinite_cloak.GetBool()) {
						bool should_refill_cloak = true;
						
						if (!cvar_infinite_cloak_deadringer.GetBool()) {
							/* check for attribute "set cloak is feign death" */
							auto invis = rtti_cast<CTFWeaponInvis *>(player->Weapon_GetWeaponByType(TF_WPN_TYPE_PDA2));
							if (invis != nullptr && CAttributeManager::AttribHookValue<int>(0, "set_weapon_mode", invis) == 1) {
								should_refill_cloak = false;
							}
						}
						
						if (should_refill_cloak) {
							player->m_Shared->m_flCloakMeter = 100.0f;
						}
					}
				});
			}
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_mvm_jointeam_blue_allow", "0", FCVAR_NOTIFY,
		"Mod: permit client command 'jointeam blue' from human players",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
