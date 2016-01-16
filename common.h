/* precompiled header! */

#ifndef _INCLUDE_SIGSEGV_COMMON_H_
#define _INCLUDE_SIGSEGV_COMMON_H_


#if defined __cplusplus

#if !defined __GNUC__
#define __attribute__(...)
#endif


/* C++ standard library */
#include <algorithm>
#include <functional>
#include <string>

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
extern IEngineTrace *enginetrace;
#include <IStaticPropMgr.h>
extern IStaticPropMgrServer *staticpropmgr;
#include <ivdebugoverlay.h>
extern IVDebugOverlay *debugoverlay;
#include "sdk2013/util_shared.h"
#include <ai_activity.h>
#include <usercmd.h>
#include <npcevent.h>
#include <debugoverlay_shared.h>

/* SourceMod */
#include <ISDKTools.h>
#include "sm/MemoryUtils.h"

/* SourceMod */
#include <smsdk_ext.h>

#endif


#endif
