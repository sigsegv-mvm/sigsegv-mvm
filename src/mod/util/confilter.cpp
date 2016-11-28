#include "mod.h"


namespace Mod_Util_ConFilter
{
	
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Util:ConFilter")
		{
		//	MOD_ADD_DETOUR_STATIC(Con_ColorPrint, "Con_ColorPrint");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_util_confilter", "0", FCVAR_NOTIFY,
		"Utility: enable enhanced console message filtering system",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}




// enhanced con_filter functionality?
// - regex based
// - multiple include filters
// - multiple exclude filters
// - multiple highlight filters (with color option)

// MUST BE WINDOWS CLIENT COMPATIBLE
