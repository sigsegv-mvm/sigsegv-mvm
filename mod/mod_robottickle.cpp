#include "mod.h"
#include "sm/detours.h"
#include "stub/stub.h"


DETOUR_DECL_MEMBER(bool, CTFPlayer_CanBeForcedToLaugh)
{
	
	
	return DETOUR_MEMBER_CALL(CTFPlayer_CanBeForcedToLaugh)();
}


class CMod_RobotTickle : public IMod
{
public:
	CMod_RobotTickle() : IMod("RobotTickle")
	{
		
	}
	
	void SetEnabled(bool enable)
	{
		this->ToggleAllDetours(enable);
	}
};
static CMod_RobotTickle s_Mod;


/*static ConVar cvar_enable("sigsegv_robottickle_enable", "0", FCVAR_NOTIFY,
	"Mod: auto-collect credits that land in the bots' spawn area",
	[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
		ConVarRef var(pConVar);
		s_Mod.SetEnabled(var.GetBool());
	});
*/
