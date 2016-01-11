#include "stub/tfbot.h"


FuncThunk<ILocomotion * (*)(const CTFBot *)> CTFBot::ft_GetLocomotionInterface("CTFBot::GetLocomotionInterface");
FuncThunk<IBody * (*)(const CTFBot *)>       CTFBot::ft_GetBodyInterface      ("CTFBot::GetBodyInterface");
FuncThunk<IVision * (*)(const CTFBot *)>     CTFBot::ft_GetVisionInterface    ("CTFBot::GetVisionInterface");
FuncThunk<IIntention * (*)(const CTFBot *)>  CTFBot::ft_GetIntentionInterface ("CTFBot::GetIntentionInterface");
