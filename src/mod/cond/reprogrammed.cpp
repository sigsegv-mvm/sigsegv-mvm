#include "mod.h"
#include "stub/tfbot.h"
#include "stub/tf_shareddefs.h"
#include "stub/gamerules.h"
#include "re/nextbot.h"


namespace Mod_Cond_Reprogrammed
{
	constexpr uint8_t s_Buf_UpdateMission[] = {
		0xc7, 0x44, 0x24, 0x0c, 0x00, 0x00, 0x00, 0x00, // +0000  mov dword ptr [esp+0xc],false
		0xc7, 0x44, 0x24, 0x08, 0x01, 0x00, 0x00, 0x00, // +0008  mov dword ptr [esp+0x8],true
		0xc7, 0x44, 0x24, 0x04, 0x03, 0x00, 0x00, 0x00, // +0010  mov dword ptr [esp+0x4],TF_TEAM_PVE_INVADERS
		0x89, 0x0c, 0x24,                               // +0018  mov dword ptr [esp],ecx
		0xe8,                                           // +001B  call CollectPlayers<CTFPlayer>
	};
	
	struct CPatch_CMissionPopulator_UpdateMission : public IPatch
	{
		CPatch_CMissionPopulator_UpdateMission() : IPatch(sizeof(s_Buf_UpdateMission)) {}
		
		virtual const char *GetFuncName() const override { return "CMissionPopulator::UpdateMission"; }
		virtual uint32_t GetFuncOffMin() const override { return 0x0000; }
		virtual uint32_t GetFuncOffMax() const override { return 0x0200; } // @ 0x0100
		
		virtual bool GetVerifyInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			buf.CopyFrom(s_Buf_UpdateMission);
			
			return true;
		}
		
		virtual bool GetPatchInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			/* change the teamnum to TEAM_ANY */
			buf.SetDword(0x10 + 4, TEAM_ANY);
			mask.SetDword(0x10 + 4, 0xffffffff);
			
			return true;
		}
	};
	
	
	constexpr uint8_t s_Buf_CheckStuck[] = {
		0x89, 0x04, 0x24,                   // +0000  mov [esp],eax
		0xe8, 0x1e, 0x1d, 0x28, 0x00,       // +0003  call CBaseEntity::GetTeamNumber
		0x83, 0xf8, 0x03,                   // +0008  cmp eax,TF_TEAM_PVE_INVADERS
		0x0f, 0x84, 0xa5, 0x01, 0x00, 0x00, // +000B  jz +0x1a5
	};
//	constexpr uint8_t s_Buf_CheckStuck_after[] = {
//		0x89, 0x04, 0x24,                   // +0000  mov [esp],eax
//		0xff, 0x15, 0xff, 0xff, 0xff, 0xff, // +0003  call [<STUB> CBaseEntity::IsBot]
//		0x90, 0x90,                         // +0009  nop nop
//		0x0f, 0x85, 0xa5, 0x01, 0x00, 0x00, // +000B  jnz +0x1a5
//	};
	
	using FPtr_IsBot = bool (CBasePlayer:: *)() const;
	struct CPatch_CTFGameMovement_CheckStuck : public IPatch
	{
		CPatch_CTFGameMovement_CheckStuck() : IPatch(sizeof(s_Buf_CheckStuck)) {}
		
		virtual const char *GetFuncName() const override { return "CTFGameMovement::CheckStuck"; }
		virtual uint32_t GetFuncOffMin() const override { return 0x0000; }
		virtual uint32_t GetFuncOffMax() const override { return 0x00a0; } // @ 0x008a
		
		virtual bool GetVerifyInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			buf.CopyFrom(s_Buf_CheckStuck);
			
			mask.SetRange(0x03 + 1, 4, 0x00);
			mask.SetRange(0x0b + 2, 4, 0x00);
			
			return true;
		}
		
		virtual bool GetPatchInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			/* indirect call through pointer */
			buf[0x03] = 0xff;
			buf[0x04] = 0x15;
			buf.SetDword(0x03 + 2, (uint32_t)&s_CBasePlayer_IsBot);
			
			/* pad out extra space with NOPs */
			buf[0x09] = 0x90;
			buf[0x0a] = 0x90;
			
			/* invert the jump condition code */
			buf[0x0c] = 0x85;
			
			mask.SetRange(0x03, 0x08, 0xff);
			mask[0x0c] = 0xff;
			
			return true;
		}
		
		static FPtr_IsBot s_CBasePlayer_IsBot;
	};
	FPtr_IsBot CPatch_CTFGameMovement_CheckStuck::s_CBasePlayer_IsBot = &CBasePlayer::IsBot;
	
	
	ConVar cvar_hellmet("sig_cond_reprogrammed_hellmet", "1", FCVAR_NOTIFY,
		"Mod: make some tweaks to TF_COND_REPROGRAMMED that Hell-met requested");
	
	
	void OnAddReprogrammed(CTFPlayer *player)
	{
		DevMsg("OnAddReprogrammed(#%d \"%s\")\n", ENTINDEX(player), player->GetPlayerName());
		
		if (!cvar_hellmet.GetBool()) {
			player->m_Shared->StunPlayer(5.0f, 0.65f, TF_STUNFLAG_NOSOUNDOREFFECT | TF_STUNFLAG_SLOWDOWN, nullptr);
		}
		
		player->ForceChangeTeam(TF_TEAM_RED, false);
		
		/* this used to be in CTFPlayerShared::OnAddReprogrammed on the client
		 * side, but we now have to do it from the server side */
		if (!cvar_hellmet.GetBool()) {
			DispatchParticleEffect("sapper_sentry1_fx", PATTACH_POINT_FOLLOW, player, "head");
		}
		
		if (cvar_hellmet.GetBool()) {
			CTFBot *bot = ToTFBot(player);
			if (bot != nullptr) {
				bot->GetVisionInterface()->ForgetAllKnownEntities();
			}
		}
	}
	
	void OnRemoveReprogrammed(CTFPlayer *player)
	{
		DevMsg("OnRemoveReprogrammed(#%d \"%s\")\n", ENTINDEX(player), player->GetPlayerName());
		
		player->ForceChangeTeam(TF_TEAM_BLUE, false);
		
		/* this is far from ideal; we can only remove ALL particle effects from
		 * the server side */
		if (!cvar_hellmet.GetBool()) {
			StopParticleEffects(player);
		}
		
		if (cvar_hellmet.GetBool()) {
			CTFBot *bot = ToTFBot(player);
			if (bot != nullptr) {
				bot->GetVisionInterface()->ForgetAllKnownEntities();
			}
		}
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
	
	
	DETOUR_DECL_MEMBER(ActionResult<CTFBot>, CTFBotScenarioMonitor_Update, CTFBot *actor, float dt)
	{
		if (actor->m_Shared->InCond(TF_COND_REPROGRAMMED)) {
			return ActionResult<CTFBot>::Continue();
		}
		
		return DETOUR_MEMBER_CALL(CTFBotScenarioMonitor_Update)(actor, dt);
	}
	
	
	DETOUR_DECL_MEMBER(ActionResult<CTFBot>, CTFBotMainAction_Update, CTFBot *actor, float dt)
	{
		auto result = DETOUR_MEMBER_CALL(CTFBotMainAction_Update)(actor, dt);
		
		if (result.transition == ActionTransition::CONTINUE && TFGameRules()->IsMannVsMachineMode() && actor->GetTeamNumber() == TF_TEAM_RED)
		{
			if (actor->ShouldAutoJump()) {
				actor->GetLocomotionInterface()->Jump();
			}
			
			/* ammo regen */
			if (cvar_hellmet.GetBool()) {
				actor->GiveAmmo(100, 1, true);
				actor->GiveAmmo(100, 2, true);
				actor->GiveAmmo(100, 3, true);
				actor->GiveAmmo(100, 4, true);
				actor->GiveAmmo(100, 5, true);
				actor->GiveAmmo(100, 6, true);
			}
		}
		
		return result;
	}
	
	
	DETOUR_DECL_MEMBER(CTFPlayer *, CTFPlayer_FindPartnerTauntInitiator)
	{
		auto player = reinterpret_cast<CTFPlayer *>(this);
		
		if (cvar_hellmet.GetBool() && player->IsBot() && TFGameRules()->IsMannVsMachineMode() && player->m_Shared->InCond(TF_COND_REPROGRAMMED)) {
			return nullptr;
		}
		
		return DETOUR_MEMBER_CALL(CTFPlayer_FindPartnerTauntInitiator)();
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Cond:Reprogrammed")
		{
			MOD_ADD_DETOUR_MEMBER(CTFPlayerShared_OnConditionAdded,   "CTFPlayerShared::OnConditionAdded");
			MOD_ADD_DETOUR_MEMBER(CTFPlayerShared_OnConditionRemoved, "CTFPlayerShared::OnConditionRemoved");
			
			/* fix: disallow reprogrammed bots from auto-switching to FetchFlag etc */
			MOD_ADD_DETOUR_MEMBER(CTFBotScenarioMonitor_Update, "CTFBotScenarioMonitor::Update");
			
			/* fix: allow reprogrammed bots to AutoJump */
			/* fix: make reprogrammed bots have infinite ammo */
			MOD_ADD_DETOUR_MEMBER(CTFBotMainAction_Update, "CTFBotMainAction::Update");
			
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_FindPartnerTauntInitiator, "CTFPlayer::FindPartnerTauntInitiator");
			
			/* fix: make mission populators aware of red-team mission bots */
			this->AddPatch(new CPatch_CMissionPopulator_UpdateMission());
			
			/* fix: make tf_resolve_stuck_players apply to all bots in MvM, rather than blu-team players */
			this->AddPatch(new CPatch_CTFGameMovement_CheckStuck());
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_cond_reprogrammed", "0", FCVAR_NOTIFY,
		"Mod: reimplement TF_COND_REPROGRAMMED",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
