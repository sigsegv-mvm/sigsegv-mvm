#include "mod.h"
#include "stub/gamerules.h"
#include "stub/tfbot.h"
#include "util/scope.h"


enum TFStunFlags {};


namespace Mod_MvM_Gib_Improvements
{
	constexpr uint8_t s_Buf[] = {
		0x31, 0xc0,                               // +0000  xor eax,eax
		0x80, 0xba, 0x5e, 0x09, 0x00, 0x00, 0x00, // +0002  cmp byte ptr [edx+m_bPlayingMannVsMachine],0x0
		0x75, 0xde,                               // +0009  jnz -0x22
	};
	
	struct CPatch_CTFPlayer_ShouldGib : public IPatch
	{
		CPatch_CTFPlayer_ShouldGib() : IPatch(sizeof(s_Buf)) {}
		
		virtual const char *GetFuncName() const override { return "CTFPlayer::ShouldGib"; }
		virtual uint32_t GetFuncOffMin() const override  { return 0x0000; }
		virtual uint32_t GetFuncOffMax() const override  { return 0x0080; } // @ 0x003e
		
		virtual bool GetVerifyInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			buf.CopyFrom(s_Buf);
			
			int off_CTFGameRules_m_bPlayingMannVsMachine;
			if (!Prop::FindOffset(off_CTFGameRules_m_bPlayingMannVsMachine, "CTFGameRules", "m_bPlayingMannVsMachine")) return false;
			
			buf.SetDword(0x02 + 2, (uint32_t)off_CTFGameRules_m_bPlayingMannVsMachine);
			
			mask[0x09 + 1] = 0x00;
			
			return true;
		}
		
		virtual bool GetPatchInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			/* NOP out the conditional jump for MvM mode */
			buf.SetRange(0x09, 2, 0x90);
			mask.SetRange(0x09, 2, 0xff);
			
			return true;
		}
	};
	
	
//	ConVar cvar_human_enable("sig_mvm_gib_improvements_human_enable", "0", FCVAR_NOTIFY,
//		"Mod: enable gibbing for human players in MvM mode");
//	
//	ConVar cvar_robot_enable("sig_mvm_gib_improvements_robot_enable", "0", FCVAR_NOTIFY,
//		"Mod: enable gibbing for non-giant robots in MvM mode");
//	
//	ConVar cvar_plasma_fix("sig_mvm_gib_improvements_plasma_fix", "0", FCVAR_NOTIFY,
//		"Mod: fix ragdoll/dissolve problems caused by weapons like Cow Mangler and medigun shield");
	
	
	bool ClassHasDecentGibs(const CTFPlayer *pPlayer)
	{
		switch (pPlayer->GetPlayerClass()->GetClassIndex()) {
		default:
			return true;
			
		case TF_CLASS_ENGINEER: // no gibs
		case TF_CLASS_MEDIC:    // head gib only
		case TF_CLASS_SNIPER:   // head gib only
		case TF_CLASS_SPY:      // head gib only
			return false;
		}
	}
	
	
	int eh_tick = -1;
	std::set<CHandle<CTFPlayer>> eh_victims;
	
	RefCount rc_CTFSniperRifle_ExplosiveHeadShot;
	DETOUR_DECL_MEMBER(void, CTFSniperRifle_ExplosiveHeadShot, CTFPlayer *pAttacker, CTFPlayer *pVictim)
	{
		SCOPED_INCREMENT(rc_CTFSniperRifle_ExplosiveHeadShot);
		DETOUR_MEMBER_CALL(CTFSniperRifle_ExplosiveHeadShot)(pAttacker, pVictim);
	}
	
	DETOUR_DECL_MEMBER(void, CTFPlayerShared_StunPlayer, float duration, float slowdown, TFStunFlags flags, CTFPlayer *attacker)
	{
		auto shared = reinterpret_cast<CTFPlayerShared *>(this);
		
		if (rc_CTFSniperRifle_ExplosiveHeadShot > 0) {
			if (eh_tick != gpGlobals->tickcount) {
				eh_victims.clear();
				eh_tick = gpGlobals->tickcount;
			}
			
			eh_victims.insert(shared->GetOuter());
		}
		
		DETOUR_MEMBER_CALL(CTFPlayerShared_StunPlayer)(duration, slowdown, flags, attacker);
	}
	
	
	DETOUR_DECL_MEMBER(bool, CTFBot_ShouldGib, const CTakeDamageInfo& info)
	{
		auto bot = reinterpret_cast<CTFBot *>(this);
		
		// TODO: determine if the current model of the bot is actually a robot model or not
		// rather than just assuming that (MvM mode + TFBot = robot)
		if (TFGameRules()->IsMannVsMachineMode()) {
			/* giants: always gib */
			if (bot->IsMiniBoss() || bot->GetModelScale() > 1.0f) {
				return true;
			}
			
			/* commons: never gib if they lack a decent number of gibs */
			if (!ClassHasDecentGibs(bot)) {
				return false;
			}
		}
		
		/* explosive headshot gibbing :) */
		if (eh_tick == gpGlobals->tickcount && eh_victims.count(bot) != 0) {
			return true;
		}
		
		/* can't use the vfunc thunk for this call because we specifically
		 * want to call the implementation in CTFPlayer */
		static auto p_CTFPlayer_ShouldGib = MakePtrToMemberFunc<CTFPlayer, bool, const CTakeDamageInfo&>(AddrManager::GetAddr("CTFPlayer::ShouldGib"));
		return (bot->*p_CTFPlayer_ShouldGib)(info);
	}
	
	
	// Problems:
	// 1. TF_CUSTOM_PLASMA (Cow Mangler and medic shield) turns giants into
	//    small ragdolls that don't do the dissolving effect
	// 2. TF_CUSTOM_PLASMA_CHARGED (Cow Mangler charged shot) makes robots gib,
	//    which looks silly for certain bot classes that only have 0-1 gibs
	// 
	// Solutions:
	// 1. For giants, change PLASMA -> PLASMA_CHARGED
	// 2. For classes missing gibs, change PLASMA_CHARGED -> PLASMA
	DETOUR_DECL_MEMBER(void, CTFPlayer_CreateRagdollEntity, bool bShouldGib, bool bBurning, bool bUberDrop, bool bOnGround, bool bYER, bool bGold, bool bIce, bool bAsh, int iCustom, bool bClassic)
	{
		auto player = reinterpret_cast<CTFPlayer *>(this);
		
		CTFBot *bot;
		if (TFGameRules()->IsMannVsMachineMode() && (bot = ToTFBot(player)) != nullptr) {
			if (bot->IsMiniBoss() || bot->GetModelScale() > 1.0f) {
				if (iCustom == TF_DMG_CUSTOM_PLASMA) {
					iCustom = TF_DMG_CUSTOM_PLASMA_CHARGED;
				}
			} else {
				if (iCustom == TF_DMG_CUSTOM_PLASMA_CHARGED && !ClassHasDecentGibs(bot)) {
					iCustom = TF_DMG_CUSTOM_PLASMA;
				}
			}
		}
		
		return DETOUR_MEMBER_CALL(CTFPlayer_CreateRagdollEntity)(bShouldGib, bBurning, bUberDrop, bOnGround, bYER, bGold, bIce, bAsh, iCustom, bClassic);
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("MvM:Gib_Improvements")
		{
			this->AddPatch(new CPatch_CTFPlayer_ShouldGib());
			
			MOD_ADD_DETOUR_MEMBER(CTFSniperRifle_ExplosiveHeadShot, "CTFSniperRifle::ExplosiveHeadShot");
			MOD_ADD_DETOUR_MEMBER(CTFPlayerShared_StunPlayer,       "CTFPlayerShared::StunPlayer");
			
			MOD_ADD_DETOUR_MEMBER(CTFBot_ShouldGib, "CTFBot::ShouldGib");
			
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_CreateRagdollEntity, "CTFPlayer::CreateRagdollEntity [args]");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_mvm_gib_improvements", "0", FCVAR_NOTIFY,
		"Mod: improve various aspects of human and robot gibbing in MvM mode",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
