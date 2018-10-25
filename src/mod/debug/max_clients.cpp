#include "mod.h"


namespace Mod::Debug::Max_Clients
{
	class CMod : public IMod, public IFrameUpdateListener
	{
	public:
		CMod() : IMod("Debug:Max_Clients") {}
		
		virtual bool ShouldReceiveFrameEvents() const override { return this->IsEnabled(); }
		
		virtual void FrameUpdatePostEntityThink() override
		{
			static long frame = 0;
			++frame;
			
			if (frame % 7 == 0) {
				ConColorMsg(Color(0xff, 0x00, 0xff, 0xff), "sv.m_nMaxclients = %d\n", sv->GetMaxClients());
			}
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_debug_max_clients", "0", FCVAR_NOTIFY,
		"Debug: figure out what the hell is going on with sv.m_nMaxclients",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
