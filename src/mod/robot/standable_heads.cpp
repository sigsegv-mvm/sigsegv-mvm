#include "mod.h"
#include "util/scope.h"
#include "sm/detours.h"
#include "stub/gamerules.h"
#include "stub/tfplayer.h"


namespace Mod_Robot_Standable_Heads
{
	RefCount rc_TFPlayerThink;
	DETOUR_DECL_MEMBER(void, CTFPlayer_TFPlayerThink)
	{
		SCOPED_INCREMENT(rc_TFPlayerThink);
		DETOUR_MEMBER_CALL(CTFPlayer_TFPlayerThink)();
	}
	
	DETOUR_DECL_MEMBER(void, CTFPlayer_ApplyAbsVelocityImpulse, const Vector *v1)
	{
		if (rc_TFPlayerThink > 0 && v1->z == 100.0f &&
			TFGameRules() && TFGameRules()->IsMannVsMachineMode()) {
			CTFPlayer *player = reinterpret_cast<CTFPlayer *>(this);
			
			CBaseEntity *groundent = player->GetGroundEntity();
			if (groundent != nullptr && groundent->IsPlayer()) {
				return;
			}
		}
		
		DETOUR_MEMBER_CALL(CTFPlayer_ApplyAbsVelocityImpulse)(v1);
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Robot:Standable_Heads")
		{
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_TFPlayerThink,           "CTFPlayer::TFPlayerThink");
		//	MOD_ADD_DETOUR_MEMBER(CTFPlayer_ApplyAbsVelocityImpulse, "CTFPlayer::ApplyAbsVelocityImpulse");
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_robot_standable_heads", "0", FCVAR_NOTIFY,
		"Mod: remove the sliding force that prevents players from standing on robots' heads",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
