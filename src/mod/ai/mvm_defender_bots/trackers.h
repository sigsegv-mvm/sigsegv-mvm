#ifndef _INCLUDE_SIGSEGV_MOD_AI_MVM_DEFENDER_BOTS_TRACKERS_H_
#define _INCLUDE_SIGSEGV_MOD_AI_MVM_DEFENDER_BOTS_TRACKERS_H_


namespace Mod::AI::MvM_Defender_Bots
{
	class ITracker
	{
	public:
		virtual ~ITracker() {}
		
		virtual int GetFrameInterval() const = 0;
		virtual void Update() = 0;
		
		void Frame()
		{
			if (this->m_nFrame % this->GetFrameInterval() == 0) {
				this->Update();
			}
			
			++this->m_nFrame;
		}
		
	private:
		int m_nFrame = 0;
	};
	
	
	class CreditTracker; extern CreditTracker TheCreditTracker;
	class FlagTracker;   extern FlagTracker   TheFlagTracker;
	class GateTracker;   extern GateTracker   TheGateTracker;
	class TankTracker;   extern TankTracker   TheTankTracker;
	
	
	void UpdateTrackers();
}


#include "mod/ai/mvm_defender_bots/trackers/credits.h"
#include "mod/ai/mvm_defender_bots/trackers/flags.h"
#include "mod/ai/mvm_defender_bots/trackers/gates.h"
#include "mod/ai/mvm_defender_bots/trackers/tanks.h"


#endif
