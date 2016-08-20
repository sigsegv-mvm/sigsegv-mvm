#include "mod.h"


namespace Mod_Debug_NextBot_Input
{
	
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Debug:NextBot_Input")
		{
			// TODO
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_debug_nextbot_input", "0", FCVAR_NOTIFY,
		"Debug: show INextBotPlayerInput information",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
