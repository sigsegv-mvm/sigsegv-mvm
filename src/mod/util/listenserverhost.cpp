#include "mod.h"


namespace Mod_Util_ListenServerHost
{
	ConVar cvar_index("sig_util_listenserverhost_index", "1", FCVAR_NOTIFY,
		"Entity index of the \"listen server host\" player");
	DETOUR_DECL_STATIC(CBasePlayer *, UTIL_GetListenServerHost)
	{
		return UTIL_PlayerByIndex(cvar_index.GetInt());
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Util:ListenServerHost")
		{
			MOD_ADD_DETOUR_STATIC(UTIL_GetListenServerHost, "UTIL_GetListenServerHost");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_util_listenserverhost", "0", FCVAR_NOTIFY,
		"Utility: override UTIL_GetListenServerHost on a dedicated server",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
