#include "extension.h"
#include "library.h"
#include "link/link.h"
#include "sm/detours.h"
#include "modmanager.h"
#include "addr/addr.h"
#include "gameconf.h"


CExtSigsegv g_Ext;
SMEXT_LINK(&g_Ext);


ICvar *icvar;
IEngineTrace *enginetrace;
IStaticPropMgrServer *staticpropmgr;
IVDebugOverlay *debugoverlay;

CGlobalVars *gpGlobals;

ISDKTools *g_pSDKTools;

IGameConfig *g_pGameConf;


bool CExtSigsegv::SDK_OnLoad(char *error, size_t maxlen, bool late)
{
	sharesys->AddDependency(myself, "sdktools.ext", true, true);
	
	gameconfs->AddUserConfigHook("sigsegv", &g_GCHook);
	if (!gameconfs->LoadGameConfigFile("sigsegv", &g_pGameConf, error, maxlen)) {
		goto fail;
	}
	
	AddrManager::Load();
	
	if (!Link::InitAll(error, maxlen)) {
		goto fail;
	}
	
	CDetourManager::Init(g_pSM->GetScriptingEngine());
	
	CModManager::LoadAllMods();
	
	return true;
	
fail:
	if (g_pGameConf != nullptr) {
		gameconfs->CloseGameConfigFile(g_pGameConf);
	}
	return false;
}

void CExtSigsegv::SDK_OnUnload()
{
	CModManager::UnloadAllMods();
	
	AddrManager::UnLoad();
	
	gameconfs->CloseGameConfigFile(g_pGameConf);
	gameconfs->RemoveUserConfigHook("sigsegv", &g_GCHook);
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
	DevMsg("CExtSigsegv: compiled @ " __DATE__ " " __TIME__ "\n");
	
	GET_V_IFACE_ANY(GetServerFactory, gamedll, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);
	
	GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
	g_pCVar = icvar;
	ConVar_Register(0, this);
	
	GET_V_IFACE_ANY(GetEngineFactory, enginetrace, IEngineTrace, INTERFACEVERSION_ENGINETRACE_SERVER);
	GET_V_IFACE_ANY(GetEngineFactory, staticpropmgr, IStaticPropMgrServer, INTERFACEVERSION_STATICPROPMGR_SERVER);
	
	//GET_V_IFACE_ANY(GetEngineFactory, debugoverlay, IVDebugOverlay, VDEBUG_OVERLAY_INTERFACE_VERSION);
	debugoverlay = (IVDebugOverlay *)ismm->VInterfaceMatch(ismm->GetEngineFactory(), VDEBUG_OVERLAY_INTERFACE_VERSION, 0);
	
	gpGlobals = ismm->GetCGlobals();
	
	LibMgr::SetPtr(Library::SERVER, (void *)ismm->GetServerFactory(false));
	LibMgr::SetPtr(Library::ENGINE, (void *)ismm->GetEngineFactory(false));
	
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
