#include "gameconf.h"


#define DEBUG_GC 0


constexpr const char *const configs[] = {
	"sigsegv/sigsegv.vtable",
	"sigsegv/sigsegv.nextbot.action",
	"sigsegv/sigsegv.nextbot.contextualquery",
	"sigsegv/sigsegv.nextbot.eventresponder",
	"sigsegv/sigsegv.nextbot.player",
	"sigsegv/sigsegv.population",
	"sigsegv/sigsegv.tfbot",
	"sigsegv/sigsegv.tfbot.behavior",
	"sigsegv/sigsegv.misc",
	nullptr,
};


CSigsegvGameConf g_GCHook;


bool CSigsegvGameConf::LoadAll(char *error, size_t maxlen)
{
	gameconfs->AddUserConfigHook("sigsegv", this);
	
	for (const char *const *c_name = configs; *c_name != nullptr; ++c_name) {
		IGameConfig *conf = nullptr;
		
		if (!gameconfs->LoadGameConfigFile(*c_name, &conf, error, maxlen) || conf == nullptr) {
			return false;
		}
		
		this->m_GameConfs.push_back(conf);
	}
	
	return true;
}

void CSigsegvGameConf::UnloadAll()
{
	for (auto conf : this->m_GameConfs) {
		gameconfs->CloseGameConfigFile(conf);
	}
	this->m_GameConfs.clear();
	
	gameconfs->RemoveUserConfigHook("sigsegv", this);
}


void CSigsegvGameConf::ReadSMC_ParseStart()
{
#if DEBUG_GC
	DevMsg("GC ParseStart\n");
#endif
	
	this->m_Section = ParseSection::ROOT;
}

void CSigsegvGameConf::ReadSMC_ParseEnd(bool halted, bool failed)
{
#if DEBUG_GC
	DevMsg("GC ParseEnd\n");
#endif
}

SMCResult CSigsegvGameConf::ReadSMC_NewSection(const SMCStates *states, const char *name)
{
#if DEBUG_GC
	DevMsg("GC NewSection \"%s\"\n", name);
#endif
	
	switch (this->m_Section) {
	case ParseSection::ROOT:
		if (strcmp(name, "addrs") == 0) {
			this->m_Section = ParseSection::ADDRS;
			return SMCResult_Continue;
		}
		break;
	case ParseSection::ADDRS:
		this->m_Section = ParseSection::ADDRS_ENTRY;
		return this->AddrEntry_Start(name);
	}
	
	return SMCResult_HaltFail;
}

SMCResult CSigsegvGameConf::ReadSMC_KeyValue(const SMCStates *states, const char *key, const char *value)
{
#if DEBUG_GC
	DevMsg("GC KeyValue \"%s\" \"%s\"\n", key, value);
#endif
	
	switch (this->m_Section) {
	case ParseSection::ADDRS_ENTRY:
		return this->AddrEntry_KeyValue(key, value);
	}
	
	return SMCResult_HaltFail;
}

SMCResult CSigsegvGameConf::ReadSMC_LeavingSection(const SMCStates *states)
{
#if DEBUG_GC
	DevMsg("GC LeavingSection\n");
#endif
	
	switch (this->m_Section) {
	case ParseSection::ADDRS:
		this->m_Section = ParseSection::ROOT;
		return SMCResult_Continue;
	case ParseSection::ADDRS_ENTRY:
		this->m_Section = ParseSection::ADDRS;
		return this->AddrEntry_End();
	}
	
	return SMCResult_HaltFail;
}


SMCResult CSigsegvGameConf::AddrEntry_Start(const char *name)
{
#if DEBUG_GC
	DevMsg("GC AddrEntry_Start \"%s\"\n", name);
#endif
	
	this->m_AddrEntry_State.m_Name = name;
	this->m_AddrEntry_State.m_KeyValues.clear();
	
	return SMCResult_Continue;
}

SMCResult CSigsegvGameConf::AddrEntry_KeyValue(const char *key, const char *value)
{
#if DEBUG_GC
	DevMsg("GC AddrEntry_KeyValue \"%s\" \"%s\"\n", key, value);
#endif
	
	std::string s_key(key);
	std::string s_value(value);
	
	this->m_AddrEntry_State.m_KeyValues[s_key] = s_value;
	
	return SMCResult_Continue;
}

SMCResult CSigsegvGameConf::AddrEntry_End()
{
	const auto& name = this->m_AddrEntry_State.m_Name;
	const auto& kv = this->m_AddrEntry_State.m_KeyValues;
	
#if DEBUG_GC
	DevMsg("GC AddrEntry_End\n");
	
	DevMsg("CSigsegvGameConf: addr \"%s\" {", name);
	for (const auto& pair : kv) {
		DevMsg(" \"%s\" => \"%s\" ", pair.first.c_str(), pair.second.c_str());
	}
	DevMsg("}\n");
#endif
	
	for (const std::string& key : { "type" }) {
		if (kv.find(key) == kv.end()) {
			DevMsg("GameData error: addr \"%s\" lacks required key \"%s\"\n", name.c_str(), key.c_str());
			return SMCResult_HaltFail;
		}
	}
	
	const std::string& type = kv.at("type");
	if (type == "sym") {
		return this->AddrEntry_Load_Sym();
	} else if (type == "vtable") {
		return this->AddrEntry_Load_VTable();
	} else if (type == "func knownvtidx") {
		return this->AddrEntry_Load_Func_KnownVTIdx();
	} else if (type == "func unistr ebpprologue knownvtidx") {
		return this->AddrEntry_Load_Func_UniqueStr_EBPPrologue_KnownVTIdx();
	} else {
		DevMsg("GameData error: addr \"%s\" has unknown type \"%s\"\n", name.c_str(), type.c_str());
		return SMCResult_HaltFail;
	}
}


SMCResult CSigsegvGameConf::AddrEntry_Load_Sym()
{
	const auto& name = this->m_AddrEntry_State.m_Name;
	const auto& kv = this->m_AddrEntry_State.m_KeyValues;
	
	for (const std::string& key : { "sym" }) {
		if (kv.find(key) == kv.end()) {
			DevMsg("GameData error: addr \"%s\" lacks required key \"%s\"\n", name.c_str(), key.c_str());
			return SMCResult_HaltFail;
		}
	}
	
	const auto& sym = kv.at("sym");
	
	auto addr = new CAddr_Sym(name, sym);
//	DevMsg("new CAddr_Sym(name:\"%s\", sym:\"%s\") @ 0x%08x\n",
//		addr->GetName(), addr->GetSymbol(), (uintptr_t)addr);
	this->m_AddrPtrs.push_back(std::unique_ptr<IAddr>(addr));
	
	return SMCResult_Continue;
}

SMCResult CSigsegvGameConf::AddrEntry_Load_VTable()
{
	const auto& name = this->m_AddrEntry_State.m_Name;
	const auto& kv = this->m_AddrEntry_State.m_KeyValues;
	
	for (const std::string& key : { "sym", "winrtti" }) {
		if (kv.find(key) == kv.end()) {
			DevMsg("GameData error: addr \"%s\" lacks required key \"%s\"\n", name.c_str(), key.c_str());
			return SMCResult_HaltFail;
		}
	}
	
	const auto& sym     = kv.at("sym");
	const auto& winrtti = kv.at("winrtti");
	
	auto addr = new CAddr_VTable(name, sym, winrtti);
//	DevMsg("new CAddr_VTable(name:\"%s\", sym:\"%s\", winrtti:\"%s\") @ 0x%08x\n",
//		addr->GetName(), addr->GetSymbol(), addr->GetWinRTTIStr(), (uintptr_t)addr);
	this->m_AddrPtrs.push_back(std::unique_ptr<IAddr>(addr));
	
	return SMCResult_Continue;
}

SMCResult CSigsegvGameConf::AddrEntry_Load_Func_KnownVTIdx()
{
	const auto& name = this->m_AddrEntry_State.m_Name;
	const auto& kv = this->m_AddrEntry_State.m_KeyValues;
	
	for (const std::string& key : { "sym", "vtable", "idx" }) {
		if (kv.find(key) == kv.end()) {
			DevMsg("GameData error: addr \"%s\" lacks required key \"%s\"\n", name.c_str(), key.c_str());
			return SMCResult_HaltFail;
		}
	}
	
	const auto& sym    = kv.at("sym");
	const auto& vtable = kv.at("vtable");
	int idx            = stoi(kv.at("idx"));
	
	auto addr = new CAddr_Func_KnownVTIdx(name, sym, vtable, idx);
	this->m_AddrPtrs.push_back(std::unique_ptr<IAddr>(addr));
	
	return SMCResult_Continue;
}

SMCResult CSigsegvGameConf::AddrEntry_Load_Func_UniqueStr_EBPPrologue_KnownVTIdx()
{
	const auto& name = this->m_AddrEntry_State.m_Name;
	const auto& kv = this->m_AddrEntry_State.m_KeyValues;
	
	for (const std::string& key : { "sym", "unistr", "vtable", "idx" }) {
		if (kv.find(key) == kv.end()) {
			DevMsg("GameData error: addr \"%s\" lacks required key \"%s\"\n", name.c_str(), key.c_str());
			return SMCResult_HaltFail;
		}
	}
	
	const auto& sym    = kv.at("sym");
	const auto& unistr = kv.at("unistr");
	const auto& vtable = kv.at("vtable");
	int idx            = stoi(kv.at("idx"));
	
	auto addr = new CAddr_Func_UniqueStr_EBPPrologue_KnownVTIdx(name, unistr, sym, vtable, idx);
	this->m_AddrPtrs.push_back(std::unique_ptr<IAddr>(addr));
	
	return SMCResult_Continue;
}
