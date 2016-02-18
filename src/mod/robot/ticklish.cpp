#include "mod.h"
#include "sm/detours.h"
#include "stub/tfplayer.h"


#if defined __GNUC__
#warning TODO: move cvars into class
#endif


namespace Mod_Robot_Ticklish
{
	ConVar cvar_giants("sig_robot_ticklish_giants", "0", FCVAR_NOTIFY,
		"Mod: tickling will only be enabled for giant robots if set to 1");
	
	
	DETOUR_DECL_MEMBER(bool, CTFPlayer_CanBeForcedToLaugh)
	{
		if (!cvar_giants.GetBool()) {
			CTFPlayer *player = reinterpret_cast<CTFPlayer *>(this);
			return !player->IsMiniBoss();
		}
		
		return true;
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Robot:Ticklish")
		{
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_CanBeForcedToLaugh, "CTFPlayer::CanBeForcedToLaugh");
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_robot_ticklish", "0", FCVAR_NOTIFY,
		"Mod: allow robots to be tickled (e.g. by the Holiday Punch)",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
