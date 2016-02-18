#include "util/prof.h"


#if defined _MSC_VER
bool Prof::s_bFail;
LARGE_INTEGER Prof::s_Freq;
LARGE_INTEGER Prof::s_Count1;
LARGE_INTEGER Prof::s_Count2;
#endif
