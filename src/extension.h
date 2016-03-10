#ifndef _INCLUDE_SIGSEGV_EXTENSION_H_
#define _INCLUDE_SIGSEGV_EXTENSION_H_


/* seriously? */
#define maxlength maxlen


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
	
	void EnableColorSpew();
	void DisableColorSpew();
	
private:
	SpewOutputFunc_t m_pSpewOutputBackup = nullptr;
};
extern CExtSigsegv g_Ext;


SpewRetval_t ANSIColorSpew(SpewType_t type, const char *pMsg);


#endif
