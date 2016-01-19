#include "stub/tfplayer.h"


VFuncThunk<bool (*)(const CBasePlayer *)> CBasePlayer::vt_IsBot("[VT] CBasePlayer", "CBasePlayer::IsBot");


bool CTFPlayer::IsPlayerClass(int iClass) const
{
	const CTFPlayerClass *pClass = this->GetPlayerClass();
	if (pClass == nullptr) return false;
	
	return pClass->IsClass(iClass);
}
