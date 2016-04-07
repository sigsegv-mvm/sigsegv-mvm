#ifndef _INCLUDE_SIGSEGV_MOD_AI_MVM_DEFENDER_BOTS_HELPERS_H_
#define _INCLUDE_SIGSEGV_MOD_AI_MVM_DEFENDER_BOTS_HELPERS_H_


#include "stub/tfbot.h"
#include "stub/entities.h"


inline void ForEachPlayer(const std::function<void(CBasePlayer *, bool&)>& functor)
{
	for (int i = 1; i < gpGlobals->maxClients; ++i) {
		CBasePlayer *player = UTIL_PlayerByIndex(i);
		if (player == nullptr)  continue;
		
		bool done = false;
		functor(player, done);
		if (done) break;
	}
}


inline void ForEachDefenderBot(const std::function<void(CTFBot *, bool&)>& functor)
{
	for (int i = 1; i < gpGlobals->maxClients; ++i) {
		CBasePlayer *player = UTIL_PlayerByIndex(i);
		if (player == nullptr) continue;
		
		if (player->GetTeamNumber() != TF_TEAM_RED) continue;
		
		CTFBot *bot = ToTFBot(player);
		if (bot == nullptr) continue;
		
		bool done = false;
		functor(bot, done);
		if (done) break;
	}
}


inline void ForEachTank(const std::function<void(CTFTankBoss *, bool&)>& functor)
{
	CUtlVector<INextBot *> nextbots;
	TheNextBots()->CollectAllBots(&nextbots);
	FOR_EACH_VEC(nextbots, i) {
		auto nextbot = nextbots[i];
		
		auto tank = rtti_cast<CTFTankBoss *>(nextbot);
		if (tank == nullptr) continue;
		
		bool done = false;
		functor(tank, done);
		if (done) break;
	}
}


inline void ForEachFlag(const std::function<void(CCaptureFlag *, bool&)>& functor)
{
	for (int i = 0; i < ICaptureFlagAutoList::AutoList().Count(); ++i) {
		auto flag = rtti_cast<CCaptureFlag *>(ICaptureFlagAutoList::AutoList()[i]);
		if (flag == nullptr) continue;
		
		bool done = false;
		functor(flag, done);
		if (done) break;
	}
}


inline void ForEachCurrencyPack(const std::function<void(CCurrencyPack *, bool&)>& functor)
{
	for (int i = 0; i < ICurrencyPackAutoList::AutoList().Count(); ++i) {
		auto pack = rtti_cast<CCurrencyPack *>(ICurrencyPackAutoList::AutoList()[i]);
		if (pack == nullptr) continue;
		
		bool done = false;
		functor(pack, done);
		if (done) break;
	}
}


#endif
