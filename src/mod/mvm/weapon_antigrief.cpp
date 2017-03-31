#include "mod.h"
#include "stub/tfplayer.h"
#include "stub/gamerules.h"
#include "util/scope.h"


enum TFStunFlags {};


namespace Mod_MvM_Weapon_AntiGrief
{
	ConVar cvar_scorchshot  ("sig_mvm_weapon_antigrief_scorchshot",   "1", FCVAR_NOTIFY, "Disable knockback and stun effects vs giant robots from the Scorch Shot");
	ConVar cvar_loosecannon ("sig_mvm_weapon_antigrief_loosecannon",  "1", FCVAR_NOTIFY, "Disable knockback and stun effects vs giant robots from the Loose Cannon");
	ConVar cvar_forceanature("sig_mvm_weapon_antigrief_forceanature", "1", FCVAR_NOTIFY, "Disable knockback and stun effects vs giant robots from the Force-A-Nature");
	
	
	static inline bool BotIsAGiant(const CTFPlayer *player)
	{
		return (player->IsMiniBoss() || player->GetModelScale() > 1.0f);
	}
	
	
	RefCount rc_ScorchShot;
	DETOUR_DECL_MEMBER(void, CTFProjectile_Flare_Explode, CGameTrace *tr, CBaseEntity *ent)
	{
		SCOPED_INCREMENT(rc_ScorchShot);
		DETOUR_MEMBER_CALL(CTFProjectile_Flare_Explode)(tr, ent);
	}
	
	
	RefCount rc_LooseCannon;
	DETOUR_DECL_MEMBER(void, CTFGrenadePipebombProjectile_PipebombTouch, CBaseEntity *ent)
	{
		SCOPED_INCREMENT(rc_LooseCannon);
		DETOUR_MEMBER_CALL(CTFGrenadePipebombProjectile_PipebombTouch)(ent);
	}
	
	
	RefCount rc_ForceANature1;
	DETOUR_DECL_MEMBER(void, CTFScatterGun_ApplyPostHitEffects, const CTakeDamageInfo& info, CTFPlayer *victim)
	{
		SCOPED_INCREMENT_IF(rc_ForceANature1, BotIsAGiant(victim));
		DETOUR_MEMBER_CALL(CTFScatterGun_ApplyPostHitEffects)(info, victim);
	}
	RefCount rc_ForceANature2;
	DETOUR_DECL_MEMBER(void, CTFPlayer_ApplyPushFromDamage, const CTakeDamageInfo& info, Vector vec)
	{
		auto player = reinterpret_cast<CTFPlayer *>(this);
		
		SCOPED_INCREMENT_IF(rc_ForceANature2, BotIsAGiant(player));
		DETOUR_MEMBER_CALL(CTFPlayer_ApplyPushFromDamage)(info, vec);
	}
	
	DETOUR_DECL_STATIC(bool, CanScatterGunKnockBack, CTFWeaponBase *scattergun, float damage, float distsqr)
	{
		if (cvar_forceanature.GetBool() && TFGameRules()->IsMannVsMachineMode()) {
			if (rc_ForceANature1 > 0 || rc_ForceANature2 > 0) {
				return false;
			}
		}
		
		return DETOUR_STATIC_CALL(CanScatterGunKnockBack)(scattergun, damage, distsqr);
	}
	
	
	static inline bool ShouldBlock_ScorchShot()  { return (cvar_scorchshot .GetBool() && rc_ScorchShot  > 0); }
	static inline bool ShouldBlock_LooseCannon() { return (cvar_loosecannon.GetBool() && rc_LooseCannon > 0); }
	
	DETOUR_DECL_MEMBER(void, CTFPlayer_ApplyAirBlastImpulse, const Vector& impulse)
	{
		if (ShouldBlock_ScorchShot() || ShouldBlock_LooseCannon()) {
			auto player = reinterpret_cast<CTFPlayer *>(this);
			
			if (BotIsAGiant(player) && TFGameRules()->IsMannVsMachineMode()) {
				return;
			}
		}
		
		DETOUR_MEMBER_CALL(CTFPlayer_ApplyAirBlastImpulse)(impulse);
	}
	
	DETOUR_DECL_MEMBER(void, CTFPlayerShared_StunPlayer, float duration, float slowdown, TFStunFlags flags, CTFPlayer *attacker)
	{
		if (ShouldBlock_ScorchShot() || ShouldBlock_LooseCannon()) {
			auto shared = reinterpret_cast<CTFPlayerShared *>(this);
			auto player = shared->GetOuter();
			
			if (BotIsAGiant(player) && TFGameRules()->IsMannVsMachineMode()) {
				return;
			}
		}
		
		DETOUR_MEMBER_CALL(CTFPlayerShared_StunPlayer)(duration, slowdown, flags, attacker);
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("MvM:Weapon_AntiGrief")
		{
			MOD_ADD_DETOUR_MEMBER(CTFProjectile_Flare_Explode, "CTFProjectile_Flare::Explode");
			
			MOD_ADD_DETOUR_MEMBER(CTFGrenadePipebombProjectile_PipebombTouch, "CTFGrenadePipebombProjectile::PipebombTouch");
			
			MOD_ADD_DETOUR_MEMBER(CTFScatterGun_ApplyPostHitEffects, "CTFScatterGun::ApplyPostHitEffects");
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_ApplyPushFromDamage,     "CTFPlayer::ApplyPushFromDamage");
			MOD_ADD_DETOUR_STATIC(CanScatterGunKnockBack,            "CanScatterGunKnockBack");
			
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_ApplyAirBlastImpulse, "CTFPlayer::ApplyAirBlastImpulse");
			MOD_ADD_DETOUR_MEMBER(CTFPlayerShared_StunPlayer,     "CTFPlayerShared::StunPlayer");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_mvm_weapon_antigrief", "0", FCVAR_NOTIFY,
		"Mod: disable some obnoxious weapon effects in MvM (primarily knockback stuff)",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
