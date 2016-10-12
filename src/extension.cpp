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
#include "factory.h"
#include "concolor.h"
#include "re/nextbot.h"
#include "version.h"
#include "convar_restore.h"
#include "stub/igamesystem.h"


CExtSigsegv g_Ext;
SMEXT_LINK(&g_Ext);

IFileSystem *filesystem                          = nullptr;
IServerGameClients *serverGameClients            = nullptr;
ICvar *icvar                                     = nullptr;
IServer *sv                                      = nullptr;
ISpatialPartition *partition                     = nullptr;
IEngineTrace *enginetrace                        = nullptr;
IStaticPropMgrServer *staticpropmgr              = nullptr;
IGameEventManager2 *gameeventmanager             = nullptr;
INetworkStringTableContainer *networkstringtable = nullptr;
IEngineSound *enginesound                        = nullptr;
IVModelInfo *modelinfo                           = nullptr;
IVDebugOverlay *debugoverlay                     = nullptr;

IPlayerInfoManager *playerinfomanager = nullptr;
IBotManager *botmanager               = nullptr;

IPhysics *physics                = nullptr;
IPhysicsCollision *physcollision = nullptr;

ISoundEmitterSystemBase *soundemitterbase = nullptr;

IMaterialSystem *g_pMaterialSystem = nullptr;

vgui::ISchemeManager *g_pVGuiSchemeManager = nullptr;

vgui::ISurface *g_pVGuiSurface         = nullptr;
IMatSystemSurface *g_pMatSystemSurface = nullptr;

CGlobalVars *gpGlobals         = nullptr;
CBaseEntityList *g_pEntityList = nullptr;

IVEngineClient *engineclient = nullptr;
IBaseClientDLL *clientdll    = nullptr;

SourcePawn::ISourcePawnEngine *g_pSourcePawn = nullptr;
SourceMod::IExtensionManager *smexts         = nullptr;

//ISDKTools *g_pSDKTools = nullptr;

IEngineTool *enginetools  = nullptr;
IServerTools *servertools = nullptr;
IClientTools *clienttools = nullptr;

IVProfExport *vprofexport = nullptr;

IDedicatedExports *dedicated = nullptr;

IMDLCache *mdlcache = nullptr;


bool CExtSigsegv::SDK_OnLoad(char *error, size_t maxlen, bool late)
{
	g_pSourcePawn = g_pSM->GetScriptingEngine();
	SM_FIND_IFACE_OR_FAIL(EXTENSIONMANAGER, smexts, error, maxlen);
	
	ColorSpew::Enable();
	
//	sharesys->AddDependency(myself, "sdktools.ext", true, true);
//	SM_GET_IFACE(SDKTOOLS, g_pSDKTools);
	
	if (gameeventmanager != nullptr) {
		gameeventmanager->LoadEventsFromFile("resource/sigsegv_events.res");
	}
	
	this->LoadSoundOverrides();
	
	g_pEntityList = reinterpret_cast<CBaseEntityList *>(gamehelpers->GetGlobalEntityList());
	
	PreScan::DoScans();
	if (!g_GCHook.LoadAll(error, maxlen)) goto fail;
	
	LibMgr::Load();
//	g_Disasm.Load();
	
	RTTI::PreLoad();
	AddrManager::Load();
	
	if (!Link::InitAll()) goto fail;
	
	Prop::PreloadAll();
	
	g_ModManager.Load();
	
	if (IGameSystem_IsLinked()) {
		IGameSystem::Add(this);
		this->m_bGameSystemAdded = true;
	} else {
		Warning("Skipping IGameSystem::Add (not linked)!\n");
	}
	
//	for (int i = 0; i < 255; ++i) {
//		ConColorMsg(Color(0xff, i, 0x00), "%02x%02x%02x\n", 0xff, i, 0x00);
//	}
	
	return true;
	
fail:
	g_GCHook.UnloadAll();
	return false;
}

void CExtSigsegv::SDK_OnUnload()
{
	ConVar_Restore::Save();
	
	if (this->m_bGameSystemAdded) {
		IGameSystem::Remove(this);
	}
	
	IHotplugAction::UnloadAll();
	
	g_ModManager.Unload();
	
	LibMgr::Unload();
	
	g_GCHook.UnloadAll();
	
	ColorSpew::Disable();
}

void CExtSigsegv::SDK_OnAllLoaded()
{
}

bool CExtSigsegv::QueryRunning(char *error, size_t maxlen)
{
//	SM_CHECK_IFACE(SDKTOOLS, g_pSDKTools);
	
	return true;
}


#define GET_IFACE_OPTIONAL(factory, var, name) \
	var = reinterpret_cast<decltype(var)>(ismm->VInterfaceMatch(factory##Factory(), name, -1))

#define GET_IFACE_REQUIRED(factory, var, name) \
	var = reinterpret_cast<decltype(var)>(ismm->VInterfaceMatch(factory##Factory(), name, -1)); \
	if (var == nullptr) { \
		if (error != nullptr && maxlen != 0) { \
			ismm->Format(error, maxlen, "Could not find interface: %s", name); \
		} \
		return false; \
	}


bool CExtSigsegv::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	DevMsg("CExtSigsegv: compiled @ %s %s\n", GetBuildDate(), GetBuildTime());
	
	GET_IFACE_REQUIRED(Engine,     engine,            INTERFACEVERSION_VENGINESERVER);
	GET_IFACE_REQUIRED(Server,     gamedll,           INTERFACEVERSION_SERVERGAMEDLL);
	GET_IFACE_REQUIRED(FileSystem, filesystem,        FILESYSTEM_INTERFACE_VERSION);
	GET_IFACE_REQUIRED(Server,     serverGameClients, INTERFACEVERSION_SERVERGAMECLIENTS);
	
	sv = engine->GetIServer();
	
	GET_IFACE_REQUIRED(Engine, icvar, CVAR_INTERFACE_VERSION);
	g_pCVar = icvar;
	ConVar_Register(0, this);
	
	GET_IFACE_REQUIRED(Engine, partition,          INTERFACEVERSION_SPATIALPARTITION);
	GET_IFACE_REQUIRED(Engine, enginetrace,        INTERFACEVERSION_ENGINETRACE_SERVER);
	GET_IFACE_REQUIRED(Engine, staticpropmgr,      INTERFACEVERSION_STATICPROPMGR_SERVER);
	GET_IFACE_REQUIRED(Engine, gameeventmanager,   INTERFACEVERSION_GAMEEVENTSMANAGER2);
	GET_IFACE_REQUIRED(Engine, networkstringtable, INTERFACENAME_NETWORKSTRINGTABLESERVER);
	GET_IFACE_REQUIRED(Engine, enginesound,        IENGINESOUND_SERVER_INTERFACE_VERSION);
	GET_IFACE_REQUIRED(Engine, modelinfo,          VMODELINFO_SERVER_INTERFACE_VERSION);
	
	GET_IFACE_REQUIRED(Server, playerinfomanager, INTERFACEVERSION_PLAYERINFOMANAGER);
	GET_IFACE_REQUIRED(Server, botmanager,        INTERFACEVERSION_PLAYERBOTMANAGER);
	GET_IFACE_REQUIRED(Server, servertools,       VSERVERTOOLS_INTERFACE_VERSION);
	
	GET_IFACE_REQUIRED(Physics, physics,       VPHYSICS_INTERFACE_VERSION);
	GET_IFACE_REQUIRED(Physics, physcollision, VPHYSICS_COLLISION_INTERFACE_VERSION);
	
	GET_IFACE_OPTIONAL(Engine, debugoverlay, VDEBUG_OVERLAY_INTERFACE_VERSION);
	GET_IFACE_OPTIONAL(Engine, enginetools,  VENGINETOOL_INTERFACE_VERSION);
	
	if (SoundEmitterSystemFactory() != nullptr) {
		GET_IFACE_OPTIONAL(SoundEmitterSystem, soundemitterbase, SOUNDEMITTERSYSTEM_INTERFACE_VERSION);
	}
	
	if (MaterialSystemFactory() != nullptr) {
		GET_IFACE_OPTIONAL(MaterialSystem, g_pMaterialSystem, MATERIAL_SYSTEM_INTERFACE_VERSION);
	}
	
	if (VGUIFactory() != nullptr) {
		GET_IFACE_OPTIONAL(VGUI, g_pVGuiSchemeManager, VGUI_SCHEME_INTERFACE_VERSION);
	}
	
	if (VGUIMatSurfaceFactory() != nullptr) {
		GET_IFACE_OPTIONAL(VGUIMatSurface, g_pVGuiSurface,      VGUI_SURFACE_INTERFACE_VERSION);
		GET_IFACE_OPTIONAL(VGUIMatSurface, g_pMatSystemSurface, MAT_SYSTEM_SURFACE_INTERFACE_VERSION);
	}
	
	if (ClientFactory() != nullptr) {
		GET_IFACE_REQUIRED(Engine, engineclient, VENGINE_CLIENT_INTERFACE_VERSION);
		GET_IFACE_REQUIRED(Client, clientdll,    CLIENT_DLL_INTERFACE_VERSION);
		GET_IFACE_OPTIONAL(Client, clienttools,  VCLIENTTOOLS_INTERFACE_VERSION);
	}
	
	if (DedicatedFactory() != nullptr) {
		GET_IFACE_OPTIONAL(Dedicated, dedicated, VENGINE_DEDICATEDEXPORTS_API_VERSION);
	}
	
	if (DataCacheFactory() != nullptr) {
		GET_IFACE_OPTIONAL(DataCache, mdlcache, MDLCACHE_INTERFACE_VERSION);
	}
	
	GET_IFACE_REQUIRED(Engine, vprofexport, "VProfExport001");
	
	gpGlobals = ismm->GetCGlobals();
	
	LibMgr::SetPtr(Library::SERVER,         ServerFactory());
	LibMgr::SetPtr(Library::ENGINE,         EngineFactory());
	LibMgr::SetPtr(Library::DEDICATED,      dedicated);
	LibMgr::SetPtr(Library::TIER0,          &MemAllocScratch);
	LibMgr::SetPtr(Library::CLIENT,         clientdll);
	LibMgr::SetPtr(Library::VGUIMATSURFACE, g_pVGuiSurface);
	
	return true;
}

bool CExtSigsegv::SDK_OnMetamodUnload(char *error, size_t maxlen)
{
	return true;
}


bool CExtSigsegv::RegisterConCommandBase(ConCommandBase *pCommand)
{
	ConVar_Restore::Register(pCommand);
	
	META_REGCVAR(pCommand);
	return true;
}


void CExtSigsegv::LevelInitPreEntity()
{
	this->LoadSoundOverrides();
	
	g_ModManager.LevelInitPreEntity();
}

void CExtSigsegv::LevelInitPostEntity()
{
	g_ModManager.LevelInitPostEntity();
}

void CExtSigsegv::LevelShutdownPreEntity()
{
	g_ModManager.LevelShutdownPreEntity();
}

void CExtSigsegv::LevelShutdownPostEntity()
{
	g_ModManager.LevelShutdownPostEntity();
}

void CExtSigsegv::FrameUpdatePreEntityThink()
{
	g_ModManager.FrameUpdatePreEntityThink();
}

void CExtSigsegv::FrameUpdatePostEntityThink()
{
	g_ModManager.FrameUpdatePostEntityThink();
}


void CExtSigsegv::LoadSoundOverrides()
{
	if (soundemitterbase != nullptr) {
		soundemitterbase->AddSoundOverrides("scripts/sigsegv_sound_overrides.txt", true);
	}
}


//ConVar cvar_build("sig_build", __DATE__ " " __TIME__, FCVAR_NONE);
CON_COMMAND(sig_build, "")
{
	Msg("%s %s\n", GetBuildDate(), GetBuildTime());
}
