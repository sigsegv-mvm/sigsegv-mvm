#include "mod.h"


namespace Mod_Util_Console_Recv
{
	class CMod : public IMod, public IGameEventListener2
	{
	public:
		CMod() : IMod("Util:Console_Recv") {}
		
		virtual void OnUnload() override
		{
			this->SetEnabled(false);
		}
		
		void SetEnabled(bool enable)
		{
			if (this->m_bEnabled != enable) {
				if (enable) {
					this->Enable();
				} else {
					this->Disable();
				}
				
				this->m_bEnabled = enable;
			}
		}
		
		bool IsEnabled() const { return this->m_bEnabled; }
		
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
		
	private:
		void Enable()
		{
			gameeventmanager->AddListener(this, "console_forward", false);
		}
		void Disable()
		{
			gameeventmanager->RemoveListener(this);
		}
		
		bool m_bEnabled = false;
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_util_console_recv", "0", FCVAR_NOTIFY,
		"Utility: console forwarding: client receive",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
