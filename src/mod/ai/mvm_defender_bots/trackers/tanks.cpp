#include "mod/ai/mvm_defender_bots/trackers/tanks.h"
#include "mod/ai/mvm_defender_bots/helpers.h"


namespace Mod::AI::MvM_Defender_Bots
{
	void ForEachTank(const std::function<void(CTFTankBoss *, bool&)>& functor)
	{
		CUtlVector<INextBot *> nextbots;
		TheNextBots().CollectAllBots(&nextbots);
		FOR_EACH_VEC(nextbots, i) {
			auto nextbot = nextbots[i];
			
			auto tank = rtti_cast<CTFTankBoss *>(nextbot);
			if (tank == nullptr) continue;
			
			bool done = false;
			functor(tank, done);
			if (done) break;
		}
	}
	
	
	float GetTankProgress(CTFTankBoss *tank)
	{
		CPathTrack *node = tank->m_hCurrentNode;
		if (node == nullptr) {
			/* deploying bomb */
			return 1.00f;
		} else {
			float dist = tank->m_NodeDists[tank->m_iCurrentNode];
			dist -= (node->GetAbsOrigin() - tank->GetAbsOrigin()).Length();
			
			return Clamp(dist / tank->m_flTotalDistance, 0.00f, 1.00f);
		}
	}


	void TankTracker::Update()
	{
		this->m_TankInfos.clear();
		
		ForEachTank([=](CTFTankBoss *tank, bool& done){
			auto& info = this->m_TankInfos[tank];
			info.progress = GetTankProgress(tank);
		});
	}
}
