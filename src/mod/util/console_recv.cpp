#include "mod.h"


namespace Mod_Util_Console_Recv
{
	class CMod : public IMod, public IGameEventListener2
	{
	public:
		CMod() : IMod("Util:Console_Recv") {}
		
		virtual void FireGameEvent(IGameEvent *event)
		{
			if (strcmp(event->GetName(), "console_forward") == 0) {
				int r = event->GetInt("r");
				int g = event->GetInt("g");
				int b = event->GetInt("b");
				const char *text = event->GetString("text");
				
				// TODO
			}
		}
		
		virtual void OnEnable() override
		{
			gameeventmanager->AddListener(this, "console_forward", false);
		}
		virtual void OnDisable() override
		{
			gameeventmanager->RemoveListener(this);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_util_console_recv", "0", FCVAR_NOTIFY,
		"Utility: console forwarding: client receive",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
