#include "extension.h"
#include "library.h"
#include "link/link.h"
#include "sm/detours.h"
#include "mod.h"
#include "addr/addr.h"
#include "addr/prescan.h"
#include "gameconf.h"
#include "prop.h"
#include "util/rtti.h"
#include "disasm/disasm.h"
#include "client.h"


CExtSigsegv g_Ext;
SMEXT_LINK(&g_Ext);


ICvar *icvar;
IBaseClientDLL *clientdll;
ISpatialPartition *partition;
IEngineTrace *enginetrace;
IStaticPropMgrServer *staticpropmgr;
IGameEventManager2 *gameeventmanager;
//IVDebugOverlay *debugoverlay;

CGlobalVars *gpGlobals;
CBaseEntityList *g_pEntityList;

//ISDKTools *g_pSDKTools;


bool CExtSigsegv::SDK_OnLoad(char *error, size_t maxlen, bool late)
{
//	sharesys->AddDependency(myself, "sdktools.ext", true, true);
//	SM_GET_IFACE(SDKTOOLS, g_pSDKTools);
	
	gameeventmanager->LoadEventsFromFile("resource/sigsegv_events.res");
	
	g_pEntityList = reinterpret_cast<CBaseEntityList *>(gamehelpers->GetGlobalEntityList());
	
	PreScan::DoScans();
	if (!g_GCHook.LoadAll(error, maxlen)) goto fail;
	
	LibMgr::Load();
	g_Disasm.Load();
	
	RTTI::PreLoad();
	AddrManager::Load();
	
	if (!Link::InitAll()) goto fail;
	
	CDetourManager::Init(g_pSM->GetScriptingEngine());
	
	Prop::PreloadAll();
	
	CModManager::LoadAllMods();
	
	return true;
	
fail:
	g_GCHook.UnloadAll();
	return false;
}

void CExtSigsegv::SDK_OnUnload()
{
	CModManager::UnloadAllMods();
	
	LibMgr::Unload();
	
	g_GCHook.UnloadAll();
}

void CExtSigsegv::SDK_OnAllLoaded()
{
}

bool CExtSigsegv::QueryRunning(char *error, size_t maxlen)
{
//	SM_CHECK_IFACE(SDKTOOLS, g_pSDKTools);
	
	return true;
}


bool CExtSigsegv::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	DevMsg("CExtSigsegv: compiled @ " __DATE__ " " __TIME__ "\n");
	
	GET_V_IFACE_ANY(GetEngineFactory, engine, IVEngineServer, INTERFACEVERSION_VENGINESERVER);
	GET_V_IFACE_ANY(GetServerFactory, gamedll, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);
	
	GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
	g_pCVar = icvar;
	ConVar_Register(0, this);
	
	GET_V_IFACE_ANY(GetEngineFactory, partition, ISpatialPartition, INTERFACEVERSION_SPATIALPARTITION);
	GET_V_IFACE_ANY(GetEngineFactory, enginetrace, IEngineTrace, INTERFACEVERSION_ENGINETRACE_SERVER);
	GET_V_IFACE_ANY(GetEngineFactory, staticpropmgr, IStaticPropMgrServer, INTERFACEVERSION_STATICPROPMGR_SERVER);
	GET_V_IFACE_ANY(GetEngineFactory, gameeventmanager, IGameEventManager2, INTERFACEVERSION_GAMEEVENTSMANAGER2);
	
	//GET_V_IFACE_ANY(GetEngineFactory, debugoverlay, IVDebugOverlay, VDEBUG_OVERLAY_INTERFACE_VERSION);
	//debugoverlay = (IVDebugOverlay *)ismm->VInterfaceMatch(ismm->GetEngineFactory(), VDEBUG_OVERLAY_INTERFACE_VERSION, 0);
	
	if (GetClientFactory() != nullptr) {
		clientdll = (IBaseClientDLL *)ismm->VInterfaceMatch(GetClientFactory(), CLIENT_DLL_INTERFACE_VERSION, 0);
	}
	
	gpGlobals = ismm->GetCGlobals();
	
	LibMgr::SetPtr(Library::SERVER, (void *)ismm->GetServerFactory(false));
	LibMgr::SetPtr(Library::ENGINE, (void *)ismm->GetEngineFactory(false));
	LibMgr::SetPtr(Library::TIER0,  (void *)&MemAllocScratch);
	LibMgr::SetPtr(Library::CLIENT, (void *)GetClientFactory());
	
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
