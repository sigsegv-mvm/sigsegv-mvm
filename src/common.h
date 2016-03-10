/* precompiled header! */

#ifndef _INCLUDE_SIGSEGV_COMMON_H_
#define _INCLUDE_SIGSEGV_COMMON_H_


#if defined __cplusplus

//#if !defined __GNUC__
//#define __attribute__(...)
//#endif


class IVEngineServer;
class IServerGameDLL;
class ICvar;
class ISpatialPartition;
class IEngineTrace;
class IStaticPropMgrServer;
class IVDebugOverlay;
class IGameEventManager2;
class IBaseClientDLL;
class CGlobalVars;
class CBaseEntityList;

extern IVEngineServer *engine;
extern IServerGameDLL *gamedll;
extern IBaseClientDLL *clientdll;
extern ICvar *icvar;
extern ISpatialPartition *partition;
extern IEngineTrace *enginetrace;
extern IStaticPropMgrServer *staticpropmgr;
extern IGameEventManager2 *gameeventmanager;
extern IVDebugOverlay *debugoverlay;

extern CGlobalVars *gpGlobals;
extern CBaseEntityList *g_pEntityList;


/* C++ standard library */
#include <algorithm>
#include <functional>
#include <memory>
#include <mutex>
#include <regex>
#include <string>
#include <thread>
#include <type_traits>
#include <typeinfo>
#include <utility>

/* STL */
#include <list>
#include <map>
#include <set>
#include <vector>

/* AMTL */
#include <amtl/am-string.h>

/* Boost */
//#include <boost/thread.hpp>

/* Source SDK */
#include <string_t.h>
#include "sdk2013/annotations.h"
#include "sdk2013/basetypes.h"
#include <tier0/dbg.h>
#include <tier1/convar.h>
#include "sdk2013/fmtstr.h"
#include <tier1/KeyValues.h>
#include <tier1/utlvector.h>
#include <shareddefs.h>
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
#include "sdk2013/util_shared.h"
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
#include <tier1/utldict.h>
#include <mp_shareddefs.h>

#define DECLARE_PREDICTABLE()
#include <collisionproperty.h>

/* MetaMod */

/* SourceMod */
#include <compat_wrappers.h>
#include <ISDKTools.h>
#include "sm/MemoryUtils.h"

//extern SourceMod::ISDKTools *g_pSDKTools;

/* this extension */
#include <smsdk_ext.h>

/* Posix specific */
#if defined _LINUX || defined _OSX
#include <unistd.h>
#include <sys/mman.h>
#include <libunwind.h>
#define HAVE_DECL_BASENAME 1
#include <libiberty/demangle.h>
#endif

/* Windows specific */
#if defined _WINDOWS
#pragma warning(disable:4091)
#include <Dbghelp.h>
#pragma warning(default:4091)
#endif

/* Capstone */
#include <capstone.h>

/* namespace clash between Windows CreateEvent macro and IGameEventManager2::CreateEvent */
#if defined CreateEvent
#undef CreateEvent
#endif


#define bf_write old_bf_write
#define bf_read old_bf_read

#endif


#endif
