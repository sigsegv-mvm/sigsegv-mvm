#include "mod.h"


namespace Mod::Debug::NextBot_Input
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
		[](IConVar *pConVar, const char *pOldValue, float flOldValue){
			s_Mod.Toggle(static_cast<ConVar *>(pConVar)->GetBool());
		});
}
