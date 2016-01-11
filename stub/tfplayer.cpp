#include "stub/tfplayer.h"


CProp_SendProp<CTFPlayer, CTFPlayerShared> CTFPlayer::m_Shared("CTFPlayer", "m_Shared");

FuncThunk<bool (*)(const CTFPlayer *, int)> CTFPlayer::ft_IsPlayerClass("CTFPlayer::IsPlayerClass");
