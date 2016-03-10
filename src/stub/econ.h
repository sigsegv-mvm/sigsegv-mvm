#ifndef _INCLUDE_SIGSEGV_STUB_ECON_H_
#define _INCLUDE_SIGSEGV_STUB_ECON_H_


#include "link/link.h"


class CEconItemView
{
public:
	int GetItemDefIndex() { return vt_GetItemDefIndex(this); }
	
private:
	static MemberVFuncThunk<CEconItemView *, int> vt_GetItemDefIndex;
};


#endif
