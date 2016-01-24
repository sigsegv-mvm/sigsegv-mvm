#include "stub/gamerules.h"


IMPL_SENDPROP(bool, CTFGameRules, m_bPlayingMannVsMachine, CTFGameRulesProxy);

GlobalThunk<CGameRules *> g_pGameRules("g_pGameRules");
