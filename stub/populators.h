#ifndef _INCLUDE_SIGSEGV_STUB_POPULATORS_H_
#define _INCLUDE_SIGSEGV_STUB_POPULATORS_H_


#include "link/link.h"


class CMissionPopulator
{
public:
	bool UpdateMission(int mtype)
	{
		return (*ft_UpdateMission)(this, mtype);
	}
	
	void **vtable;
	void *m_Spawner;
	void *m_PopMgr;
	int m_Objective;
	int m_Where;
	
private:
	static FuncThunk<bool (*)(CMissionPopulator *, int)> ft_UpdateMission;
};


#endif
