#include "mod.h"
#include "stub/tfplayer.h"


namespace Mod::Robot::Ticklish
{
	ConVar cvar_giants("sig_robot_ticklish_giants", "0", FCVAR_NOTIFY,
		"Mod: enable tickling for giant robots as well");
	
	
	DETOUR_DECL_MEMBER(bool, CTFPlayer_CanBeForcedToLaugh)
	{
		auto player = reinterpret_cast<CTFPlayer *>(this);
		
		if (!cvar_giants.GetBool()) {
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
