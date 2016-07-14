#include "mod.h"


namespace Mod_MvM_Explosive_Headshot_On_Everything
{
	
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("MvM:Explosive_Headshot_On_Everything")
		{
			
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_mvm_explosive_headshot_on_everything", "0", FCVAR_NOTIFY,
		"Mod: enable explosive headshot on all hitscan weapons",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
