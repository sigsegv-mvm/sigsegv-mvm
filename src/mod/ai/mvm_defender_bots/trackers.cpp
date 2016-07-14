#include "mod/ai/mvm_defender_bots/trackers.h"


namespace Mod_AI_MvM_Defender_Bots
{
	CreditTracker TheCreditTracker;
	FlagTracker TheFlagTracker;
	GateTracker TheGateTracker;
	TankTracker TheTankTracker;
	
	
	void UpdateTrackers()
	{
		TheCreditTracker.Frame();
		TheFlagTracker.Frame();
		TheGateTracker.Frame();
		TheTankTracker.Frame();
	}
}
