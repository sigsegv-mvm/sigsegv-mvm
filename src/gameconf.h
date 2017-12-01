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
		ADDRS_GROUP,
		ADDRS_GROUP_COMMON,
	};
	ParseSection m_Section;
	
	struct
	{
		std::string m_Name;
		std::map<std::string, std::string> m_KeyValues;
	} m_AddrEntry_State;
	
	struct
	{
		std::map<std::string, std::string> m_CommonKV;
		std::map<std::string, std::string> m_Entries;
	} m_AddrGroup_State;
	
	SMCResult AddrEntry_Start(const char *name);
	SMCResult AddrEntry_KeyValue(const char *key, const char *value);
	SMCResult AddrEntry_End();
	
	SMCResult AddrGroup_Start();
	SMCResult AddrGroup_KeyValue(const char *key, const char *value);
	SMCResult AddrGroup_End();
	
	SMCResult AddrGroup_Common_Start();
	SMCResult AddrGroup_Common_KeyValue(const char *key, const char *value);
	SMCResult AddrGroup_Common_End();
	
	std::list<std::unique_ptr<IAddr>> m_AddrPtrs;
	
	std::map<std::string, SMCResult (CSigsegvGameConf::*)()> m_AddrParsers {
		{ "sym",                                   &CSigsegvGameConf::AddrEntry_Load_Sym },
		{ "sym regex",                             &CSigsegvGameConf::AddrEntry_Load_Sym_Regex },
		{ "fixed",                                 &CSigsegvGameConf::AddrEntry_Load_Fixed },
		{ "pattern",                               &CSigsegvGameConf::AddrEntry_Load_Pattern },
		{ "datamap",                               &CSigsegvGameConf::AddrEntry_Load_DataDescMap },
		{ "func knownvtidx",                       &CSigsegvGameConf::AddrEntry_Load_Func_KnownVTIdx },
		{ "func datamap vthunk",                   &CSigsegvGameConf::AddrEntry_Load_Func_DataMap_VThunk },
		{ "func ebpprologue uniref",               &CSigsegvGameConf::AddrEntry_Load_Func_EBPPrologue_UniqueRef },
		{ "func ebpprologue unistr",               &CSigsegvGameConf::AddrEntry_Load_Func_EBPPrologue_UniqueStr },
		{ "func ebpprologue unistr knownvtidx",    &CSigsegvGameConf::AddrEntry_Load_Func_EBPPrologue_UniqueStr_KnownVTIdx },
		{ "func ebpprologue nonunistr knownvtidx", &CSigsegvGameConf::AddrEntry_Load_Func_EBPPrologue_NonUniqueStr_KnownVTIdx },
		{ "func ebpprologue vprof",                &CSigsegvGameConf::AddrEntry_Load_Func_EBPPrologue_VProf },
		{ "convar",                                &CSigsegvGameConf::AddrEntry_Load_ConVar },
		{ "concommand",                            &CSigsegvGameConf::AddrEntry_Load_ConCommand },
	};
	
	void AddrEntry_Load_Common(IAddr *addr);
	SMCResult AddrEntry_Load_Sym();
	SMCResult AddrEntry_Load_Sym_Regex();
	SMCResult AddrEntry_Load_Fixed();
	SMCResult AddrEntry_Load_Pattern();
	SMCResult AddrEntry_Load_DataDescMap();
	SMCResult AddrEntry_Load_Func_KnownVTIdx();
	SMCResult AddrEntry_Load_Func_DataMap_VThunk();
	SMCResult AddrEntry_Load_Func_EBPPrologue_UniqueRef();
	SMCResult AddrEntry_Load_Func_EBPPrologue_UniqueStr();
	SMCResult AddrEntry_Load_Func_EBPPrologue_UniqueStr_KnownVTIdx();
	SMCResult AddrEntry_Load_Func_EBPPrologue_NonUniqueStr_KnownVTIdx();
	SMCResult AddrEntry_Load_Func_EBPPrologue_VProf();
	
	SMCResult AddrEntry_Load_ConCommandBase(bool is_command);
	SMCResult AddrEntry_Load_ConVar()     { return this->AddrEntry_Load_ConCommandBase(false); }
	SMCResult AddrEntry_Load_ConCommand() { return this->AddrEntry_Load_ConCommandBase(true);  }
};
extern CSigsegvGameConf g_GCHook;


#endif
