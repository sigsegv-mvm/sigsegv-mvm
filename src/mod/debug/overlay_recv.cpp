#include "mod.h"
#include "stub/debugoverlay.h"


namespace Mod_Debug_Overlay_Recv
{
	class CMod : public IMod, public IGameEventListener2
	{
	public:
		CMod() : IMod("Debug:Overlay_Recv")
		{
		}
		virtual ~CMod()
		{
			this->StopListening();
		}
		
		void SetEnabled(bool enable)
		{
			DevMsg("%s\n", __FUNCTION__);
			
			if (this->m_bEnabled == enable) return;
			
			if (enable) {
				this->StartListening();
			} else {
				this->StopListening();
			}
			
			this->m_bEnabled = enable;
		}
		
		virtual void FireGameEvent(IGameEvent *event) override
		{
			DevMsg("%s\n", __FUNCTION__);
			
		/*	const char *name = event->GetName();
			
			if (strcmp(name, "overlay_AddEntityTextOverlay") == 0) {
				DevMsg("FireGameEvent(\"%s\")\n", "overlay_AddEntityTextOverlay");
				
				debugoverlay->AddEntityTextOverlay(
					event->GetInt("ent_index"),
					event->GetInt("line_offset"),
					event->GetFloat("duration"),
					event->GetInt("r"),
					event->GetInt("g"),
					event->GetInt("b"),
					event->GetInt("a"),
					"%s", event->GetString("text"));
			}*/
		}
		
	private:
		void StartListening()
		{
			DevMsg("%s\n", __FUNCTION__);
/*			if (gameeventmanager != nullptr) {
				DevMsg("AddListener(\"%s\")\n", "overlay_AddEntityTextOverlay");
				gameeventmanager->AddListener(this, "overlay_AddEntityTextOverlay", true);
			}*/
		}
		void StopListening()
		{
			DevMsg("%s\n", __FUNCTION__);
/*			if (gameeventmanager != nullptr) {
				gameeventmanager->RemoveListener(this);
			}*/
		}
		
		bool m_bEnabled = false;
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_debug_overlay_recv", "0", FCVAR_NOTIFY,
		"Debug: overlay forwarding: client receive",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			DevMsg("cvar change callback: sig_debug_overlay_recv\n");
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
