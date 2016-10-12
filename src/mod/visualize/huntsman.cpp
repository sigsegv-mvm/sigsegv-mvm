#include "mod.h"


namespace Mod_Visualize_Huntsman
{
	ConVar cvar_interval("sig_visualize_huntsman_interval", "2", FCVAR_NOTIFY,
		"Visualization: frame interval");
	
	
	
	
	
	class CMod : public IMod, public IFrameUpdateListener
	{
	public:
		CMod() : IMod("Visualize:Huntsman") {}
		
		virtual bool ShouldReceiveFrameEvents() const override { return this->IsEnabled(); }
		
		virtual void FrameUpdatePostEntityThink() override
		{
			static long frame = 0;
			if (++frame % cvar_interval.GetInt() != 0) return;
			
			// TODO
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_visualize_huntsman", "0", FCVAR_NOTIFY,
		"Visualization: draw huntsman hit detection information",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
