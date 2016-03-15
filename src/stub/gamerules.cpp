#include "stub/gamerules.h"


GlobalThunk<CGameRulesProxy *> CGameRulesProxy::s_pGameRulesProxy("CGameRulesProxy::s_pGameRulesProxy");


MemberVFuncThunk<CMultiplayRules *, VoiceCommandMenuItem_t *, CBaseMultiplayerPlayer *, int, int> CMultiplayRules::vt_VoiceCommand(TypeName<CMultiplayRules>(), "CMultiplayRules::VoiceCommand");


IMPL_SENDPROP(bool, CTFGameRules, m_bPlayingMedieval,      CTFGameRulesProxy);
IMPL_SENDPROP(bool, CTFGameRules, m_bPlayingMannVsMachine, CTFGameRulesProxy);

GlobalThunk<CGameRules *> g_pGameRules("g_pGameRules");
