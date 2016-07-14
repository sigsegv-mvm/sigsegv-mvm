#include "mod/ai/mvm_defender_bots/trackers/credits.h"
#include "mod/ai/mvm_defender_bots/helpers.h"


namespace Mod_AI_MvM_Defender_Bots
{
	void ForEachCurrencyPack(const std::function<void(CCurrencyPack *, bool&)>& functor)
	{
		for (int i = 0; i < ICurrencyPackAutoList::AutoList().Count(); ++i) {
			auto pack = rtti_cast<CCurrencyPack *>(ICurrencyPackAutoList::AutoList()[i]);
			if (pack == nullptr) continue;
			
			bool done = false;
			functor(pack, done);
			if (done) break;
		}
	}
	
	
	void CreditTracker::Update()
	{
		/* remove entries for obsolete currency packs */
		for (auto it = this->m_CreditInfos.begin(); it != this->m_CreditInfos.end(); ) {
			if ((*it).first == nullptr) {
				it = this->m_CreditInfos.erase(it);
			} else {
				++it;
			}
		}
		
		/* add/update entries for currently visible currency packs */
		ForEachCurrencyPack([=](CCurrencyPack *pack, bool& done1){
			ForEachPlayer([=](CBasePlayer *player, bool& done2){
				if (!player->IsAlive()) return;
				if (player->GetTeamNumber() != TF_TEAM_RED) return;
				
				if (player->IsAbleToSee(pack, CBaseCombatCharacter::USE_FOV)) {
					this->m_CreditInfos[pack].t_lastseen[player] = gpGlobals->curtime;
				}
			});
		});
		
		/* debug overlay */
		for (const auto& pair1 : this->m_CreditInfos) {
			CCurrencyPack *pack    = pair1.first;
			const CreditInfo& info = pair1.second;
			
			int line = 0;
			for (const auto& pair2 : info.t_lastseen) {
				CBasePlayer *player = pair2.first;
				float t_lastseen    = pair2.second;
				
				NDebugOverlay::EntityText(ENTINDEX(pack), line, CFmtStrN<256>("%-20s %.1f", player->GetPlayerName(), gpGlobals->curtime - t_lastseen),
					this->GetFrameInterval() * gpGlobals->interval_per_tick, 0xff, 0xff, 0xff, 0xff);
				
				bool recent = (gpGlobals->curtime - t_lastseen < 0.2f);
				NDebugOverlay::HorzArrow(player->EyePosition(), pack->GetAbsOrigin(), 1.0f,
					(recent ? 0xff : 0x80),
					(recent ? 0xff : 0x80),
					(recent ? 0xff : 0x80),
					(recent ? 0xff : 0x80),
					true, this->GetFrameInterval() * gpGlobals->interval_per_tick);
				
				++line;
			}
		}
	}
	
	
	CCurrencyPack *CreditTracker::SelectCurrencyPack() const
	{
		// TODO: intelligently choose currency packs
		
		if (this->m_CreditInfos.empty()) {
			return nullptr;
		}
		
		auto it = this->m_CreditInfos.begin();
		std::advance(it, RandomInt(0, this->m_CreditInfos.size() - 1));
		return (*it).first;
	}
}
