#include "mod.h"


namespace Mod_Util_Console_Send
{
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Util:Console_Send")
		{
			
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_util_console_send", "0", FCVAR_NOTIFY,
		"Utility: console forwarding: server send",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
