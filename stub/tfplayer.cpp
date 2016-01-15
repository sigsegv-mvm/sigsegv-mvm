#include "stub/tfplayer.h"


VFuncThunk<bool (*)(const CBasePlayer *)> CBasePlayer::vt_IsBot("[VT] CBasePlayer", "CBasePlayer::IsBot");

FuncThunk<bool (*)(const CTFPlayer *, int)> CTFPlayer::ft_IsPlayerClass("CTFPlayer::IsPlayerClass");
