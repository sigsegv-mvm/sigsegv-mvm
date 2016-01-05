#ifndef _INCLUDE_SIGSEGV_MODMANAGER_H_
#define _INCLUDE_SIGSEGV_MODMANAGER_H_


#include "common.h"
#include "mod.h"


struct ModInfo
{
	ModInfo(IMod *mod) :
		m_pMod(mod) {}
	
	IMod *m_pMod;
	bool m_bLoaded = false;
	bool m_bFailed = false;
	char m_szError[1024];
};


class CModManager
{
public:
	static void Register(IMod *mod);
	static void Unregister(IMod *mod);
	
	static void LoadAllMods();
	static void UnloadAllMods();
	
	static void CC_ListMods(const CCommand& cmd);
	
private:
	static ModInfo *GetInfo(IMod *mod);
	
	static std::list<ModInfo *> s_Mods;
};


#endif
