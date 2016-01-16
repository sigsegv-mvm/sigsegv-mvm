#ifndef _INCLUDE_SIGSEGV_GAMECONF_H_
#define _INCLUDE_SIGSEGV_GAMECONF_H_


class CSigsegvGameConf : public ITextListener_SMC
{
public:
	bool LoadAll(char *error, size_t maxlen);
	void UnloadAll();
	
private:
	std::vector<IGameConfig *> m_GameConfs;
	
public:
	virtual void ReadSMC_ParseStart() override;
	virtual void ReadSMC_ParseEnd(bool halted, bool failed) override;
	virtual SMCResult ReadSMC_NewSection(const SMCStates *states, const char *name) override;
	virtual SMCResult ReadSMC_KeyValue(const SMCStates *states, const char *key, const char *value) override;
	virtual SMCResult ReadSMC_LeavingSection(const SMCStates *states) override;
	
private:
	enum class ParseSection : int
	{
		ROOT,
		ADDRS,
		ADDRS_ENTRY,
	};
	ParseSection m_Section;
	
	SMCResult AddrEntry_Start(const char *name);
	SMCResult AddrEntry_KeyValue(const char *key, const char *value);
	SMCResult AddrEntry_End();
	
	struct
	{
		const char *m_Name;
		std::map<std::string, std::string> m_KeyValues;
	} m_AddrEntry_State;
};
extern CSigsegvGameConf g_GCHook;


#endif
