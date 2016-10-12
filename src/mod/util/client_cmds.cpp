#include "mod.h"


namespace Mod_Util_Client_Cmds
{
	
	
	
	
	
	
	class CMod : public IMod, public IFrameUpdateListener
	{
	public:
		CMod() : IMod("Util:Client_Cmds")
		{
			
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_util_client_cmds", "0", FCVAR_NOTIFY,
		"Utility: enable client cheat commands for testing",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
