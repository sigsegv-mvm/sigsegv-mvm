#ifndef _INCLUDE_SIGSEGV_MOD_POP_KV_CONDITIONAL_H_
#define _INCLUDE_SIGSEGV_MOD_POP_KV_CONDITIONAL_H_


#include "util/autolist.h"


class IKVCond : public AutoList<IKVCond>
{
public:
	virtual bool operator()() = 0;
};


#endif
