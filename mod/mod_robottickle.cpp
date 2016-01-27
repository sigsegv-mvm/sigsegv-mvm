#include "mod.h"
#include "sm/detours.h"
#include "stub/tfplayer.h"


#if defined __GNUC__
#warning TODO: move cvars into class
#endif


namespace Mod_RobotTickle
{
	ConVar cvar_giants("sigsegv_robottickle_giants", "0", FCVAR_NOTIFY,
		"Mod: if set to 0, tickling will only be enabled for non-giant robots");
	
	
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
		CMod() : IMod("RobotTickle")
		{
			MOD_ADD_DETOUR_MEMBER(CTFPlayer, CanBeForcedToLaugh);
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sigsegv_robottickle_enable", "0", FCVAR_NOTIFY,
		"Mod: allow robots to be tickled (e.g. by the Holiday Punch)",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
