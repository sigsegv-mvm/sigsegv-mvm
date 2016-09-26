/* precompiled header! */

#ifndef _INCLUDE_SIGSEGV_COMMON_H_
#define _INCLUDE_SIGSEGV_COMMON_H_


#if defined __cplusplus


class IVEngineServer;
class IServerGameDLL;
class IFileSystem;
class IServerGameClients;
class IServer;
class ICvar;
class ISpatialPartition;
class IEngineTrace;
class IStaticPropMgrServer;
class IGameEventManager2;
class INetworkStringTableContainer;
class IEngineSound;
class IVModelInfo;
class IVDebugOverlay;

class IPhysics;
class IPhysicsCollision;

class ISoundEmitterSystemBase;

class IMaterialSystem;

namespace vgui {
	class ISchemeManager;
	class ISurface;
}
class IMatSystemSurface;

class CGlobalVars;
class CBaseEntityList;

class IVEngineClient;
class IBaseClientDLL;

class IEngineTool;
class IServerTools;
class IClientTools;

class IVProfExport;

class IDedicatedExports;

class IMDLCache;

namespace SourcePawn {
	class ISourcePawnEngine;
}
namespace SourceMod {
	class IExtensionManager;
}


extern IVEngineServer *engine;
extern IServerGameDLL *gamedll;
extern IFileSystem *filesystem;
extern IServerGameClients *serverGameClients;
extern IServer *server;
extern ICvar *icvar;
extern ISpatialPartition *partition;
extern IEngineTrace *enginetrace;
extern IStaticPropMgrServer *staticpropmgr;
extern IGameEventManager2 *gameeventmanager;
extern INetworkStringTableContainer *networkstringtable;
extern IEngineSound *enginesound;
extern IVModelInfo *modelinfo;
extern IVDebugOverlay *debugoverlay;

extern IPhysics *physics;
extern IPhysicsCollision *physcollision;

extern ISoundEmitterSystemBase *soundemitterbase;

extern IMaterialSystem *g_pMaterialSystem;

extern vgui::ISchemeManager *g_pVGuiSchemeManager;

extern vgui::ISurface *g_pVGuiSurface;
extern IMatSystemSurface *g_pMatSystemSurface;

extern CGlobalVars *gpGlobals;
extern CBaseEntityList *g_pEntityList;

extern IVEngineClient *engineclient;
extern IBaseClientDLL *clientdll;

extern IEngineTool *enginetools;
extern IServerTools *servertools;
extern IClientTools *clienttools;

extern IVProfExport *vprofexport;

extern IDedicatedExports *dedicated;

extern IMDLCache *mdlcache;

extern SourcePawn::ISourcePawnEngine *g_pSourcePawn;
extern SourceMod::IExtensionManager *smexts;


/* C++ standard library */
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <numeric>
#include <regex>
#include <string>
#include <thread>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <cinttypes>
using namespace std::literals;


/* STL */
#include <deque>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>


/* Boost */
//#include <boost/asio.hpp>
//#include <boost/thread.hpp>


/* Posix specific */
#if defined _LINUX || defined _OSX

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <libunwind.h>
#define HAVE_DECL_BASENAME 1
#include <libiberty/demangle.h>
#include <libelf.h>

#endif


/* Windows specific */
#if defined _WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#pragma warning(disable:4091)
#include <Dbghelp.h>
#pragma warning(default:4091)

#include <Winsock2.h>
#include <WS2tcpip.h>

/* namespace clash between Windows CreateEvent macro and IGameEventManager2::CreateEvent */
#if defined CreateEvent
#undef CreateEvent
#endif

#endif


/* LZ4 */
#include <lz4.h>


/* Capstone */
#include <capstone.h>


/* Approximate Nearest Neighbor */
#include <ANN/ANN.h>


/* Source's public/video/ivideoservices.h is naughty and uses the preprocessor
 * to override nullptr; so prevent that header from being included */
#define IVIDEOSERVICES_H
class IVideoRecorder;


/* Source SDK */
#define RAD_TELEMETRY_DISABLED
#undef   NO_STRING_T
#undef WEAK_STRING_T
#include <Color.h>
#include <string_t.h>
#include <annotations.h>
#include <basetypes.h>
#include <mathlib/vector.h>
#include <utlmemory.h>
#include <utlstring.h>
#include <utlvector.h>
#include <utlrbtree.h>
#include <utlmap.h>
#include <utlbuffer.h>
#include <shareddefs.h>
#include <icvar.h>
#include <dbg.h>
#include "sdk2013/convar.h"
#include <fmtstr.h>
#include <KeyValues.h>
#include <ehandle.h>
#include <datamap.h>
#include <predictioncopy.h>
#include <takedamageinfo.h>
#include <iserverunknown.h>
#include <iserverentity.h>
#include <edict.h>
#include <IEngineTrace.h>
#include <IStaticPropMgr.h>
#include <ivdebugoverlay.h>
#include "sdk2013/debugoverlay_shared.h"
#include <util_shared.h>
#include <ai_activity.h>
#include <usercmd.h>
#include <npcevent.h>
#include <dt_send.h>
#include <eiface.h>
#include <ispatialpartition.h>
#include <random.h>
#include <ai_activity.h>
#include <igameevents.h>
#include <inetmessage.h>
#include <inetchannel.h>
#include <irecipientfilter.h>
//#include <usermessages.h>
#include <interface.h>
#include <cdll_int.h>
#include <iclientnetworkable.h>
#include <utldict.h>
#include <mp_shareddefs.h>
#include <materialsystem/imaterialsystem.h>
#include <materialsystem/imaterial.h>
#include <particle_parse.h>
#include <SoundEmitterSystem/isoundemittersystembase.h>
#include <IEngineSound.h>
#include <igamesystem.h>
#include <vphysics_interface.h>
#include <ivmodelrender.h>
#include <vcollide_parse.h>
#include <steam/steamclientpublic.h>
#include <netadr.h>
#include <toolframework/ienginetool.h>
#include <toolframework/itoolentity.h>
#include <vprof.h>
#include <networkstringtabledefs.h>
#include <valve_minmax_off.h>
#include <stringpool.h>
#include <filesystem.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <VGuiMatSurface/IMatSystemSurface.h>
#include <IKeyValuesSystem.h>
#include <model_types.h>
#include <idedicatedexports.h>
#include <icommandline.h>
#include <iserver.h>
#include <iclient.h>
#include <datacache/imdlcache.h>

#define DECLARE_PREDICTABLE()
#include <collisionproperty.h>


/* AMTL */
//#include <amtl/am-string.h>


/* MetaMod */


/* SourceMod */
#include <compat_wrappers.h>
#include <ISDKTools.h>
#include "sm/MemoryUtils.h"

//extern SourceMod::ISDKTools *g_pSDKTools;


/* this extension */
#include <smsdk_ext.h>


//#define bf_write old_bf_write
//#define bf_read old_bf_read


#endif


#endif
