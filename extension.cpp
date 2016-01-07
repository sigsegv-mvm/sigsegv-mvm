#include "extension.h"
#include "link_nextbot.h"
#include "detours.h"
#include "modmanager.h"


CExtSigsegv g_Ext;
SMEXT_LINK(&g_Ext);


ICvar *icvar;
IEngineTrace *enginetrace;
IStaticPropMgrServer *staticpropmgr;

ISDKTools *g_pSDKTools;

IGameConfig *g_pGameConf;


bool CExtSigsegv::SDK_OnLoad(char *error, size_t maxlen, bool late)
{
	sharesys->AddDependency(myself, "sdktools.ext", true, true);
	
	if (!gameconfs->LoadGameConfigFile("sigsegv", &g_pGameConf, error, maxlen)) {
		return false;
	}
	
	if (!Link_NextBot::InitAll(error, maxlen)) {
		return false;
	}
	
	CDetourManager::Init(g_pSM->GetScriptingEngine(), g_pGameConf);
	
	CModManager::LoadAllMods();
	
	return true;
}

void CExtSigsegv::SDK_OnUnload()
{
	CModManager::UnloadAllMods();
	
	gameconfs->CloseGameConfigFile(g_pGameConf);
}

void CExtSigsegv::SDK_OnAllLoaded()
{
	SM_GET_LATE_IFACE(SDKTOOLS, g_pSDKTools);
}

bool CExtSigsegv::QueryRunning(char *error, size_t maxlen)
{
	SM_CHECK_IFACE(SDKTOOLS, g_pSDKTools);
	
	return true;
}


bool CExtSigsegv::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
	g_pCVar = icvar;
	ConVar_Register(0, this);
	
	GET_V_IFACE_ANY(GetEngineFactory, enginetrace, IEngineTrace, INTERFACEVERSION_ENGINETRACE_SERVER);
	GET_V_IFACE_ANY(GetEngineFactory, staticpropmgr, IStaticPropMgrServer, INTERFACEVERSION_STATICPROPMGR_SERVER);
	
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
