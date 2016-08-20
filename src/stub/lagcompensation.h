#ifndef _INCLUDE_SIGSEGV_STUB_LAGCOMPENSATION_H_
#define _INCLUDE_SIGSEGV_STUB_LAGCOMPENSATION_H_


#include "link/link.h"


#define lagcompensation __donotuse_lagcompensation
#include <../server/ilagcompensationmanager.h>
#undef lagcompensation


extern GlobalThunk<ILagCompensationManager *> lagcompensation;


#endif
