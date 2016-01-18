#ifndef _INCLUDE_SIGSEGV_GAMECONF_H_
#define _INCLUDE_SIGSEGV_GAMECONF_H_


#include "addr/addr.h"


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
	
	
	struct
	{
		std::string m_Name;
		std::map<std::string, std::string> m_KeyValues;
	} m_AddrEntry_State;
	
	std::list<std::unique_ptr<IAddr>> m_AddrPtrs;
	
	SMCResult AddrEntry_Start(const char *name);
	SMCResult AddrEntry_KeyValue(const char *key, const char *value);
	SMCResult AddrEntry_End();
	
	std::map<std::string, SMCResult (CSigsegvGameConf::*)()> m_AddrParsers{
		{ "sym",                                &CSigsegvGameConf::AddrEntry_Load_Sym },
		{ "vtable",                             &CSigsegvGameConf::AddrEntry_Load_VTable },
		{ "func knownvtidx",                    &CSigsegvGameConf::AddrEntry_Load_Func_KnownVTIdx },
		{ "func ebpprologue unistr",            &CSigsegvGameConf::AddrEntry_Load_Func_EBPPrologue_UniqueStr },
		{ "func ebpprologue unistr knownvtidx", &CSigsegvGameConf::AddrEntry_Load_Func_EBPPrologue_UniqueStr_KnownVTIdx },
	};
	
	SMCResult AddrEntry_Load_Sym();
	SMCResult AddrEntry_Load_VTable();
	SMCResult AddrEntry_Load_Func_KnownVTIdx();
	SMCResult AddrEntry_Load_Func_EBPPrologue_UniqueStr();
	SMCResult AddrEntry_Load_Func_EBPPrologue_UniqueStr_KnownVTIdx();
};
extern CSigsegvGameConf g_GCHook;


#endif
