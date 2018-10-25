#include "mod.h"


namespace Mod::MvM::Disposable_Dispenser
{
	
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("MvM:Disposable_Dispenser")
		{
			
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_mvm_disposable_dispenser", "0", FCVAR_NOTIFY,
		"Mod: replace the disposable mini-sentry with a disposable mini-dispenser",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue){
			s_Mod.Toggle(static_cast<ConVar *>(pConVar)->GetBool());
		});
}
