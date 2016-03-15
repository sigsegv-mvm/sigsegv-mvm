#include "extension.h"
#include "library.h"
#include "link/link.h"
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


ICvar *icvar                         = nullptr;
ISpatialPartition *partition         = nullptr;
IEngineTrace *enginetrace            = nullptr;
IStaticPropMgrServer *staticpropmgr  = nullptr;
IGameEventManager2 *gameeventmanager = nullptr;
IVDebugOverlay *debugoverlay         = nullptr;

CGlobalVars *gpGlobals         = nullptr;
CBaseEntityList *g_pEntityList = nullptr;

IBaseClientDLL *clientdll          = nullptr;
IMaterialSystem *g_pMaterialSystem = nullptr;

SourcePawn::ISourcePawnEngine *g_pSourcePawn = nullptr;
SourceMod::IExtensionManager *smexts         = nullptr;

//ISDKTools *g_pSDKTools = nullptr;


#if 0
CON_COMMAND_F(sig_unload, "Unload this extension", FCVAR_NONE)
{
	if (smexts != nullptr) {
		smexts->UnloadExtension(myself);
	}
}
#endif


bool CExtSigsegv::SDK_OnLoad(char *error, size_t maxlen, bool late)
{
	g_pSourcePawn = g_pSM->GetScriptingEngine();
	SM_FIND_IFACE_OR_FAIL(EXTENSIONMANAGER, smexts, error, maxlen);
	
	this->EnableColorSpew();
	
//	sharesys->AddDependency(myself, "sdktools.ext", true, true);
//	SM_GET_IFACE(SDKTOOLS, g_pSDKTools);
	
	gameeventmanager->LoadEventsFromFile("resource/sigsegv_events.res");
	
	g_pEntityList = reinterpret_cast<CBaseEntityList *>(gamehelpers->GetGlobalEntityList());
	
	PreScan::DoScans();
	if (!g_GCHook.LoadAll(error, maxlen)) goto fail;
	
	LibMgr::Load();
//	g_Disasm.Load();
	
	RTTI::PreLoad();
	AddrManager::Load();
	
	if (!Link::InitAll()) goto fail;
	
//	CDetourManager::Init(g_pSourcePawn);
	
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
	
	this->DisableColorSpew();
}

void CExtSigsegv::SDK_OnAllLoaded()
{
}

bool CExtSigsegv::QueryRunning(char *error, size_t maxlen)
{
//	SM_CHECK_IFACE(SDKTOOLS, g_pSDKTools);
	
	return true;
}


ConVar cvar_build("sig_build", __DATE__ " " __TIME__, FCVAR_NONE);
bool CExtSigsegv::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	DevMsg("CExtSigsegv: compiled @ " __DATE__ " " __TIME__ "\n");
	
	GET_V_IFACE_CURRENT(GetEngineFactory, engine, IVEngineServer, INTERFACEVERSION_VENGINESERVER);
	GET_V_IFACE_CURRENT(GetServerFactory, gamedll, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);
	
	GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
	g_pCVar = icvar;
	ConVar_Register(0, this);
	
	GET_V_IFACE_CURRENT(GetEngineFactory, partition, ISpatialPartition, INTERFACEVERSION_SPATIALPARTITION);
	GET_V_IFACE_CURRENT(GetEngineFactory, enginetrace, IEngineTrace, INTERFACEVERSION_ENGINETRACE_SERVER);
	GET_V_IFACE_CURRENT(GetEngineFactory, staticpropmgr, IStaticPropMgrServer, INTERFACEVERSION_STATICPROPMGR_SERVER);
	GET_V_IFACE_CURRENT(GetEngineFactory, gameeventmanager, IGameEventManager2, INTERFACEVERSION_GAMEEVENTSMANAGER2);
	
	//GET_V_IFACE_CURRENT(GetEngineFactory, debugoverlay, IVDebugOverlay, VDEBUG_OVERLAY_INTERFACE_VERSION);
	debugoverlay = (IVDebugOverlay *)ismm->VInterfaceMatch(ismm->GetEngineFactory(), VDEBUG_OVERLAY_INTERFACE_VERSION, 0);
	
	if (IsClient()) {
		clientdll = (IBaseClientDLL *)ismm->VInterfaceMatch(GetClientFactory(), CLIENT_DLL_INTERFACE_VERSION, 0);
		g_pMaterialSystem = (IMaterialSystem *)ismm->VInterfaceMatch(GetMaterialSystemFactory(), MATERIAL_SYSTEM_INTERFACE_VERSION, 0);
	}
	
	gpGlobals = ismm->GetCGlobals();
	
	LibMgr::SetPtr(Library::SERVER, (void *)ismm->GetServerFactory(false));
	LibMgr::SetPtr(Library::ENGINE, (void *)ismm->GetEngineFactory(false));
	LibMgr::SetPtr(Library::TIER0,  (void *)&MemAllocScratch);
	LibMgr::SetPtr(Library::CLIENT, (void *)clientdll);
	
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


void CExtSigsegv::EnableColorSpew()
{
#if defined POSIX
	if (engine->IsDedicatedServer()) {
		this->m_pSpewOutputBackup = GetSpewOutputFunc();
		SpewOutputFunc(&ANSIColorSpew);
	}
#endif
}

void CExtSigsegv::DisableColorSpew()
{
#if defined POSIX
	if (engine->IsDedicatedServer()) {
		SpewOutputFunc(this->m_pSpewOutputBackup);
	}
#endif
}


#if defined POSIX
SpewRetval_t ANSIColorSpew(SpewType_t type, const char *pMsg)
{
	Color color = GetSpewOutputColor();
	
	printf("\e[38;2;%d;%d;%dm%s\e[0m", color.r(), color.g(), color.b(), pMsg);
	
	if (type == SPEW_ASSERT) {
		if (getenv("RAISE_ON_ASSERT") == nullptr) {
			return SPEW_DEBUGGER;
		} else {
			return SPEW_CONTINUE;
		}
	} else if (type == SPEW_ERROR) {
		return SPEW_ABORT;
	} else {
		return SPEW_CONTINUE;
	}
}
#endif
