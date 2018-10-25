#include "mod.h"
#include "stub/tfweaponbase.h"
#include "stub/tf_shareddefs.h"
#include "util/iterate.h"


namespace Mod::Tank::Gunslinger_Combo
{
	// TODO: finish this
	// basically, in CTFRobotArm::Smack, just override the vcall to ::IsPlayer so that it returns true for CTFTankBoss's
	
	// currently all the mod does is display debug info about CTFRobotArms
	
	
	class CMod : public IMod, public IFrameUpdateListener
	{
	public:
		CMod() : IMod("Tank:Gunslinger_Combo") {}
		
		virtual bool ShouldReceiveFrameEvents() const override { return this->IsEnabled(); }
		
		virtual void FrameUpdatePostEntityThink() override
		{
			static long frame = 0;
			if (++frame % 2 == 0) return;
			
			ForEachEntityByRTTI<CTFRobotArm>([](CTFRobotArm *arm){
				NDebugOverlay::EntityText(ENTINDEX(arm), 0, CFmtStrN<256>("Punch:     #%d", arm->GetPunchNumber()), 0.030f);
				NDebugOverlay::EntityText(ENTINDEX(arm), 1, CFmtStrN<256>("Time Left: %4.2f", Clamp(gpGlobals->curtime - arm->GetLastPunchTime(), 0.0f, 1.0f)), 0.030f);
				NDebugOverlay::EntityText(ENTINDEX(arm), 2, CFmtStrN<256>("Combo:     %s", (arm->ShouldInflictComboDamage() ? "YES" : "NO")), 0.030f);
				NDebugOverlay::EntityText(ENTINDEX(arm), 3, CFmtStrN<256>("Max Force: %s", (arm->ShouldImpartMaxForce() ? "YES" : "NO")), 0.030f);
			});
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_tank_gunslinger_combo", "0", FCVAR_NOTIFY,
		"Tank: enable gunslinger 3-punch combo functionality",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue){
			s_Mod.Toggle(static_cast<ConVar *>(pConVar)->GetBool());
		});
}
