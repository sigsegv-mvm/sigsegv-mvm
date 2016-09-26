#include "mod/ai/mvm_defender_bots/trackers/gates.h"
#include "mod/ai/mvm_defender_bots/helpers.h"


namespace Mod_AI_MvM_Defender_Bots
{
	void ForEachControlPoint(const std::function<void(CTeamControlPoint *, bool&)>& functor)
	{
		auto& masters = g_hControlPointMasters.GetRef();
		if (masters.IsEmpty()) return;
		
		CTeamControlPointMaster *master = masters[0];
		CUtlMap<int, CTeamControlPoint *>& points = master->m_ControlPoints;
		
		FOR_EACH_MAP_FAST(points, i) {
			auto point = points[i];
			
			bool done = false;
			functor(point, done);
			if (done) break;
		}
	}

	
	void GateTracker::Update()
	{
		this->m_GateInfos.clear();
		
		ForEachControlPoint([=](CTeamControlPoint *point, bool& done){
			auto& info = this->m_GateInfos[point];
			
			NDebugOverlay::EntityText(ENTINDEX(point), 0, "GATE", gpGlobals->interval_per_tick, 0xff, 0xff, 0xff, 0xff);
		});
	}
}
