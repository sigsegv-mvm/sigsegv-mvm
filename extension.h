#ifndef _INCLUDE_SIGSEGV_EXTENSION_H_
#define _INCLUDE_SIGSEGV_EXTENSION_H_


/* seriously? */
#define maxlength maxlen


#include "common.h"


extern ICvar *icvar;
extern IEngineTrace *enginetrace;
extern IStaticPropMgrServer *staticpropmgr;

extern CGlobalVars *gpGlobals;

extern ISDKTools *g_pSDKTools;

extern IGameConfig *g_pGameConf;


class CExtSigsegv : public SDKExtension, public IConCommandBaseAccessor
{
public:
	virtual bool SDK_OnLoad(char *error, size_t maxlen, bool late);
	virtual void SDK_OnUnload();
	virtual void SDK_OnAllLoaded();
//	virtual void SDK_OnPauseChange(bool paused);
	virtual bool QueryRunning(char *error, size_t maxlen);
	
#if defined SMEXT_CONF_METAMOD
	virtual bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late);
	virtual bool SDK_OnMetamodUnload(char *error, size_t maxlen);
//	virtual bool SDK_OnMetamodPauseChange(bool paused, char *error, size_t maxlen);
#endif
	
	bool RegisterConCommandBase(ConCommandBase *pCommand);
};


#endif
