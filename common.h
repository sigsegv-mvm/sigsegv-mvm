#ifndef _INCLUDE_SIGSEGV_COMMON_H_
#define _INCLUDE_SIGSEGV_COMMON_H_


/* C++ standard library */
#include <algorithm>
#include <functional>

/* C++ non-standard */
#include <cxxabi.h>

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
#include <predictioncopy.h>
#include <takedamageinfo.h>
#include <iserverunknown.h>
#include <iserverentity.h>
#include <edict.h>
#include <IEngineTrace.h>
extern IEngineTrace *enginetrace;
#include <IStaticPropMgr.h>
extern IStaticPropMgrServer *staticpropmgr;
#include "sdk2013/util_shared.h"
#include <datamap.h>

/* SourceMod */
#include <ISDKTools.h>

/* SourceMod */
#include <smsdk_ext.h>


#endif
