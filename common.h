/* precompiled header! */

#ifndef _INCLUDE_SIGSEGV_COMMON_H_
#define _INCLUDE_SIGSEGV_COMMON_H_


#if defined __cplusplus

#if !defined __GNUC__
#define __attribute__(...)
#endif


class IVEngineServer;
class IServerGameDLL;
class ICvar;
class IEngineTrace;
class IStaticPropMgrServer;
class IVDebugOverlay;
class CGlobalVars;


extern IVEngineServer *engine;
extern IServerGameDLL *gamedll;
extern ICvar *icvar;
extern IEngineTrace *enginetrace;
extern IStaticPropMgrServer *staticpropmgr;
extern IVDebugOverlay *debugoverlay;

extern CGlobalVars *gpGlobals;


/* C++ standard library */
#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <utility>

/* STL */
#include <list>
#include <map>
#include <vector>

/* AMTL */

/* Source SDK */
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
#include "sdk2013/util_shared.h"
#include <ai_activity.h>
#include <usercmd.h>
#include <npcevent.h>
#include <debugoverlay_shared.h>
#include <dt_send.h>
#include <eiface.h>

/* SourceMod */
#include <compat_wrappers.h>
#include <ISDKTools.h>
#include "sm/MemoryUtils.h"

extern SourceMod::ISDKTools *g_pSDKTools;

/* this extension */
#include <smsdk_ext.h>

/* Posix specific */
#if defined _LINUX || defined _OSX
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

#endif


#endif
