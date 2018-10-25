#include "mod.h"


namespace Mod::Debug::Scale_Rate
{
	ConVar cvar_hand_scale ("sig_debug_scale_rate_hand_scale",  "1.0", FCVAR_NOTIFY, "Debug: value to be returned by CTFPlayer::GetDesiredHandScale");
	ConVar cvar_head_scale ("sig_debug_scale_rate_head_scale",  "1.0", FCVAR_NOTIFY, "Debug: value to be returned by CTFPlayer::GetDesiredHeadScale");
	ConVar cvar_torso_scale("sig_debug_scale_rate_torso_scale", "1.0", FCVAR_NOTIFY, "Debug: value to be returned by CTFPlayer::GetDesiredTorsoScale");
	DETOUR_DECL_MEMBER(float, CTFPlayer_GetDesiredHandScale)  { return cvar_hand_scale.GetFloat(); }
	DETOUR_DECL_MEMBER(float, CTFPlayer_GetDesiredHeadScale)  { return cvar_head_scale.GetFloat(); }
	DETOUR_DECL_MEMBER(float, CTFPlayer_GetDesiredTorsoScale) { return cvar_torso_scale.GetFloat(); }
	
	ConVar cvar_hand_rate ("sig_debug_scale_rate_hand_rate",  "1.0", FCVAR_NOTIFY, "Debug: value to be returned by CTFPlayer::GetHandScaleSpeed");
	ConVar cvar_head_rate ("sig_debug_scale_rate_head_rate",  "1.0", FCVAR_NOTIFY, "Debug: value to be returned by CTFPlayer::GetHeadScaleSpeed");
	ConVar cvar_torso_rate("sig_debug_scale_rate_torso_rate", "1.0", FCVAR_NOTIFY, "Debug: value to be returned by CTFPlayer::GetTorsoScaleSpeed");
	DETOUR_DECL_MEMBER(float, CTFPlayer_GetHandScaleSpeed)  { return cvar_hand_rate.GetFloat(); }
	DETOUR_DECL_MEMBER(float, CTFPlayer_GetHeadScaleSpeed)  { return cvar_head_rate.GetFloat(); }
	DETOUR_DECL_MEMBER(float, CTFPlayer_GetTorsoScaleSpeed) { return cvar_torso_rate.GetFloat(); }
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Debug:Scale_Rate")
		{
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_GetDesiredHandScale,  "CTFPlayer::GetDesiredHandScale");
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_GetDesiredHeadScale,  "CTFPlayer::GetDesiredHeadScale");
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_GetDesiredTorsoScale, "CTFPlayer::GetDesiredTorsoScale");
			
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_GetHandScaleSpeed,  "CTFPlayer::GetHandScaleSpeed");
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_GetHeadScaleSpeed,  "CTFPlayer::GetHeadScaleSpeed");
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_GetTorsoScaleSpeed, "CTFPlayer::GetTorsoScaleSpeed");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_debug_scale_rate", "0", FCVAR_NOTIFY,
		"Debug: figure out how head/hand/torso scale rates work",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
