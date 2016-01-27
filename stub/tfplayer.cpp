#include "stub/tfplayer.h"


VFuncThunk<CBasePlayer, bool (*)(const CBasePlayer *)> CBasePlayer::vt_IsBot("CBasePlayer::IsBot");


IMPL_SENDPROP(int,      CTFPlayerClassShared, m_iClass,         CTFPlayer);
IMPL_SENDPROP(string_t, CTFPlayerClassShared, m_iszClassIcon,   CTFPlayer);
IMPL_SENDPROP(string_t, CTFPlayerClassShared, m_iszCustomModel, CTFPlayer);


IMPL_SENDPROP(float, CTFPlayerShared, m_flRageMeter,    CTFPlayer);
IMPL_SENDPROP(bool,  CTFPlayerShared, m_bRageDraining,  CTFPlayer);
IMPL_SENDPROP(bool,  CTFPlayerShared, m_bInUpgradeZone, CTFPlayer);


IMPL_SENDPROP(CTFPlayerClass,  CTFPlayer, m_PlayerClass, CTFPlayer);
IMPL_SENDPROP(bool,            CTFPlayer, m_bIsMiniBoss, CTFPlayer);
IMPL_SENDPROP(CTFPlayerShared, CTFPlayer, m_Shared,      CTFPlayer);


bool CTFPlayer::IsPlayerClass(int iClass) const
{
	const CTFPlayerClass *pClass = this->GetPlayerClass();
	if (pClass == nullptr) return false;
	
	return pClass->IsClass(iClass);
}
