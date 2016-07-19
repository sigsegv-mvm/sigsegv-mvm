#ifndef _INCLUDE_SIGSEGV_EXTENSION_H_
#define _INCLUDE_SIGSEGV_EXTENSION_H_


/* seriously? */
#define maxlength maxlen


class CExtSigsegv :
	public SDKExtension,
	public IMetamodListener,
	public IConCommandBaseAccessor,
	public CBaseGameSystemPerFrame
{
public:
	virtual bool SDK_OnLoad(char *error, size_t maxlen, bool late) override;
	virtual void SDK_OnUnload() override;
	virtual void SDK_OnAllLoaded() override;
//	virtual void SDK_OnPauseChange(bool paused) override;
	virtual bool QueryRunning(char *error, size_t maxlen) override;
	
#if defined SMEXT_CONF_METAMOD
	virtual bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late) override;
	virtual bool SDK_OnMetamodUnload(char *error, size_t maxlen) override;
//	virtual bool SDK_OnMetamodPauseChange(bool paused, char *error, size_t maxlen) override;
#endif
	
	virtual bool RegisterConCommandBase(ConCommandBase *pCommand) override;
	
private:
	virtual void LevelInitPreEntity() override;
	virtual void LevelInitPostEntity() override;
	virtual void LevelShutdownPreEntity() override;
	virtual void LevelShutdownPostEntity() override;
	virtual void FrameUpdatePreEntityThink() override;
	virtual void FrameUpdatePostEntityThink() override;
	
	void LoadSoundOverrides();
};
extern CExtSigsegv g_Ext;


#endif
