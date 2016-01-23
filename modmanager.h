#ifndef _INCLUDE_SIGSEGV_MODMANAGER_H_
#define _INCLUDE_SIGSEGV_MODMANAGER_H_


#include "mod.h"


class CModManager
{
public:
	static void LoadAllMods();
	static void UnloadAllMods();
	
	static void CC_ListMods(const CCommand& cmd);
};


#endif
