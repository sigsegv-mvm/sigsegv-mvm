#include "mod.h"
#include "stub/tfplayer.h"
#include "stub/tf_shareddefs.h"
#include "util/iterate.h"


namespace Mod_Debug_Cond_Stunned
{
	class CMod : public IMod, public IFrameUpdateListener
	{
	public:
		CMod() : IMod("Debug:Cond_Stunned") {}
		
		virtual bool ShouldReceiveFrameEvents() const override { return this->IsEnabled(); }
		
		virtual void FrameUpdatePostEntityThink() override
		{
			static long frame = 0;
			if (++frame % 2 == 0) return;
			
			ForEachTFPlayer([](CTFPlayer *player){
				if (!player->IsAlive()) return;
				
				if (player->m_Shared->InCond(TF_COND_STUNNED)) {
					NDebugOverlay::EntityTextAtPosition(player->EyePosition() + Vector(0.0f, 0.0f, 20.0f), 0, "STUNNED", 0.030f, 0xff, 0x00, 0x00, 0xff);
				} else {
					NDebugOverlay::EntityTextAtPosition(player->EyePosition() + Vector(0.0f, 0.0f, 20.0f), 0, "normal",  0.030f, 0x00, 0xff, 0x00, 0xff);
				}
			});
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_debug_cond_stunned", "0", FCVAR_NOTIFY,
		"Debug: display whether each player is in TF_COND_STUNNED at any given time",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
