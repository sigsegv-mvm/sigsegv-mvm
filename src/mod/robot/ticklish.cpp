#include "mod.h"
#include "stub/tfplayer.h"


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
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_robot_ticklish", "0", FCVAR_NOTIFY,
		"Mod: allow robots to be tickled (e.g. by the Holiday Punch)",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
