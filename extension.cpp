#include "common.h"


std::set<IMod *> g_Mods;


CExtSigsegv g_Ext;
SMEXT_LINK(&g_Ext);


ICvar *icvar;

IGameConfig *g_pGameConf;


bool CExtSigsegv::SDK_OnLoad(char *error, size_t maxlen, bool late)
{
	if (!gameconfs->LoadGameConfigFile("sigsegv", &g_pGameConf, error, maxlen)) {
		return false;
	}
	
	CDetourManager::Init(g_pSM->GetScriptingEngine(), g_pGameConf);
	
	FOR_EACH_MOD {
		if (!mod->OnLoad()) {
			return false;
		}
	}
	
	return true;
}

void CExtSigsegv::SDK_OnUnload()
{
	FOR_EACH_MOD {
		mod->OnUnload();
	}
	
	gameconfs->CloseGameConfigFile(g_pGameConf);
}


bool CExtSigsegv::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
	g_pCVar = icvar;
	ConVar_Register(0, this);
	
	return true;
}

bool CExtSigsegv::SDK_OnMetamodUnload(char *error, size_t maxlen)
{
	return true;
}


bool CExtSigsegv::RegisterConCommandBase(ConCommandBase *pCommand)
{
	META_REGCVAR(pCommand);
	return true;
}
