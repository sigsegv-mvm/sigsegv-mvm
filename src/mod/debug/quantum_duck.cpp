#include "mod.h"


namespace Mod::Debug::Quantum_Duck
{
	// +CBasePlayer::m_bDucked
	// +CBasePlayer::m_bDucking
	// +CBasePlayer::m_bInDuckJump
	// +CBasePlayer::m_flDuckTime
	// +CBasePlayer::m_flDuckJumpTime
	// +CBasePlayer::m_bDuckToggled
	// +CBasePlayer::m_fFlags         <-- particularly FL_DUCKING, FL_ANIMDUCKING
	// +CBasePlayer::m_afPhysicsFlags <-- particularly PFLAG_DUCKING
	// CBasePlayer::m_vphysicsCollisionState << need prop accessors for these
	// CBasePlayer::m_pShadowStand           << need prop accessors for these
	// CBasePlayer::m_pShadowCrouch          << need prop accessors for these
	
	// see game/server/playerlocaldata.h for descriptions of the meanings of these variables
	
	
	
	// CLIENT ONLY
	// - C_BasePlayer::PostThink: based on GetFlags() & FL_DUCKING, call SetCollisionBounds(...)
	
	// SERVER ONLY
	// - virtual CBasePlayer::Duck (TODO: is there a client equivalent for prediction?)
	// - CBasePlayer::RunCommand
	// - CBasePlayer::PreThink
	// - FixPlayerCrouchStuck (only from CBasePlayer::Restore)
	// - CBasePlayer::PostThink: based on GetFlags() & FL_DUCKING, calls SetCollisionBounds(...)
	// - CBasePlayer::PostThinkVPhysics: based on GetFlags() & FL_DUCKING, may call SetVCollisionState(..., ..., VPHYS_CROUCH)
	// - CBasePlayer::SetupVPhysicsShadow: based on GetFlags & FL_DUCKING, calls SetVCollisionState(..., ..., VPHYS_CROUCH or VPHYS_WALK)
	
	
	// SHARED
	// - CBasePlayer::GetPlayerMins: based on GetFlags() & FL_DUCKING, returns VEC(_DUCK)?_HULL_MIN_SCALED
	// - CBasePlayer::GetPlayerMaxs: based on GetFlags() & FL_DUCKING, returns VEC(_DUCK)?_HULL_MAX_SCALED
	// - CGameMovement::GetPlayerMins: based on parameter or m_Local.m_bDucked, returns VEC(_DUCK)?_HULL_MIN_SCALED
	// - CGameMovement::GetPlayerMaxs: based on parameter or m_Local.m_bDucked, returns VEC(_DUCK)?_HULL_MAX_SCALED
	// - CGameMovement::GetPlayerViewOffset: based on parameter, returns VEC(_DUCK)?_VIEW_SCALED
	// - CGameMovement::FixPlayerCrouchStuck
	// - CGameMovement::CanUnduck
	// - CGameMovement::FinishUnDuck
	// - CGameMovement::UpdateDuckJumpEyeOffset
	// - CGameMovement::FinishUnDuckJump
	// - CGameMovement::FinishDuck
	// - CGameMovement::StartUnDuckJump
	// - CGameMovement::SetDuckedEyeOffset
	// - CGameMovement::Duck (ESPECIALLY THE HACK AT THE END OF THE FUNCTION!!)
	
	
	// tweaks:
	// - debug_latch_reset_onduck (STAGING_ONLY)
	
	
	// overlays:
	// player->CollisionProperty->OBBMins()
	// player->CollisionProperty->OBBMaxs()
	// player->EyePosition()
	// player->GetViewOffset()
	// player origin
	// player wsc
	// draw vphysics shadow
	// draw all boxes that ICollideable will tell us about
	
	
	
	// TODO: find in files for "crouch"
	
	// TODO: find TF specific stuff
	// - any users of CGameMovement::SplineFraction?
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Debug:Quantum_Duck")
		{
			
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_debug_quantum_duck", "0", FCVAR_NOTIFY,
		"Debug: determine why quantum ducking is possible and how to fix it",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
