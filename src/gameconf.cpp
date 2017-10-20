#include "gameconf.h"
#include "addr/standard.h"


#define DEBUG_GC 0


static const char *const configs[] = {
	"sigsegv/datamaps",
	"sigsegv/globals",
	"sigsegv/NextBotKnownEntity",
	"sigsegv/NextBotContextualQueryInterface",
	"sigsegv/NextBotEventResponderInterface",
	"sigsegv/NextBotInterface",
	"sigsegv/NextBotBodyInterface",
	"sigsegv/NextBotLocomotionInterface",
	"sigsegv/NextBotVisionInterface",
	"sigsegv/NextBotIntentionInterface",
	"sigsegv/NextBotBehavior",
	"sigsegv/NextBotPlayer",
	"sigsegv/NextBotPlayerBody",
	"sigsegv/NextBotPlayerLocomotion",
	"sigsegv/NextBotPath",
	"sigsegv/NextBotPathFollow",
	"sigsegv/NextBotChasePath",
	"sigsegv/NextBotManager",
	"sigsegv/population",
	"sigsegv/nav",
	"sigsegv/tfplayer",
	"sigsegv/tfbot",
	"sigsegv/tfbot_body",
	"sigsegv/tfbot_locomotion",
	"sigsegv/tfbot_vision",
	"sigsegv/tfbot_behavior",
	"sigsegv/misc",
	"sigsegv/debugoverlay",
	"sigsegv/client",
	"sigsegv/convars",
	nullptr,
};


CSigsegvGameConf g_GCHook;


// =============================================================================
// HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK

// SM's GameConfigManager caches CGameConfig objects by filename, and provides
// no public interface for evicting them from the cache; and there's really no
// other way to do this that doesn't involve violating the interface boundary;
// so, we're just forced to do horrible ugly stupid stuff like this instead...

#include <am-refcounting.h>

class CGameConfig : public ITextListener_SMC, public IGameConfig, public ke::Refcounted<CGameConfig>
{
public:
	bool IsStale() const
	{
		assert(this->GetRefCount() > 0);
		return this->GetRefCount() > 1;
	}
	
	void EvictFromCacheAndClose()
	{
		while (this->GetRefCount() > 1) {
			this->Release();
		}
		gameconfs->CloseGameConfigFile(this);
	}
	
private:
	uintptr_t GetRefCount() const
	{
		return *reinterpret_cast<const uintptr_t *>(static_cast<const ke::Refcounted<CGameConfig> *>(this));
	}
};

// HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK
// =============================================================================


bool CSigsegvGameConf::LoadAll(char *error, size_t maxlen)
{
	gameconfs->AddUserConfigHook("sigsegv", this);
	
	for (const char *const *c_name = configs; *c_name != nullptr; ++c_name) {
		IGameConfig *conf = nullptr;
		
		if (!gameconfs->LoadGameConfigFile(*c_name, &conf, error, maxlen)) {
			/* failed config loads still require a close operation, otherwise
			 * they'll end up as stale entries in the gameconf manager cache */
			gameconfs->CloseGameConfigFile(conf);
			return false;
		} else if (conf == nullptr) {
			return false;
		}
		
		auto conf_internal = static_cast<CGameConfig *>(conf);
		if (conf_internal->IsStale()) {
			Warning("GameData warning: file '%s' is stale; evicting from cache and re-loading.\n", *c_name);
			conf_internal->EvictFromCacheAndClose();
			--c_name;
			continue;
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
	case ParseSection::ADDRS:
		return this->AddrEntry_Abbreviated(key, value);
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
	if (this->m_AddrParsers.find(type) == this->m_AddrParsers.end()) {
		DevMsg("GameData error: addr \"%s\" has unknown type \"%s\"\n", name.c_str(), type.c_str());
		return SMCResult_HaltFail;
	}
	
	auto parser = this->m_AddrParsers.at(type);
	return (this->*parser)();
}


/* shorthand: for sym entries in lib server, allow a "name" "sym" line instead of a whole block */
SMCResult CSigsegvGameConf::AddrEntry_Abbreviated(const char *key, const char *value)
{
	this->AddrEntry_Start(key);
	this->AddrEntry_KeyValue("type", "sym");
	this->AddrEntry_KeyValue("sym",  value);
	return this->AddrEntry_End();
}


void CSigsegvGameConf::AddrEntry_Load_Common(IAddr *addr)
{
	const auto& kv = this->m_AddrEntry_State.m_KeyValues;
	
	if (kv.find("lib") != kv.end()) {
		addr->SetLibrary(LibMgr::Lib_FromString(kv.at("lib").c_str()));
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
	
	auto a = new CAddr_Sym(name, sym);
	this->AddrEntry_Load_Common(a);
	this->m_AddrPtrs.push_back(std::unique_ptr<IAddr>(a));
	return SMCResult_Continue;
}

SMCResult CSigsegvGameConf::AddrEntry_Load_Sym_Regex()
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
	
	auto a = new CAddr_Sym_Regex(name, sym);
	this->AddrEntry_Load_Common(a);
	this->m_AddrPtrs.push_back(std::unique_ptr<IAddr>(a));
	return SMCResult_Continue;
}

SMCResult CSigsegvGameConf::AddrEntry_Load_Fixed()
{
	const auto& name = this->m_AddrEntry_State.m_Name;
	const auto& kv = this->m_AddrEntry_State.m_KeyValues;
	
	for (const std::string& key : { "sym", "addr", "build" }) {
		if (kv.find(key) == kv.end()) {
			DevMsg("GameData error: addr \"%s\" lacks required key \"%s\"\n", name.c_str(), key.c_str());
			return SMCResult_HaltFail;
		}
	}
	
	const auto& sym = kv.at("sym");
	int addr        = stoi(kv.at("addr"), nullptr, 0);
	int build       = stoi(kv.at("build"), nullptr, 0);
	
	auto a = new CAddr_FixedAddr(name, sym, addr, build);
	this->AddrEntry_Load_Common(a);
	this->m_AddrPtrs.push_back(std::unique_ptr<IAddr>(a));
	return SMCResult_Continue;
}

SMCResult CSigsegvGameConf::AddrEntry_Load_Pattern()
{
	const auto& name = this->m_AddrEntry_State.m_Name;
	const auto& kv = this->m_AddrEntry_State.m_KeyValues;
	
	for (const std::string& key : { "sym", "seg", "seek", "mask" }) {
		if (kv.find(key) == kv.end()) {
			DevMsg("GameData error: addr \"%s\" lacks required key \"%s\"\n", name.c_str(), key.c_str());
			return SMCResult_HaltFail;
		}
	}
	
	const auto& sym  = kv.at("sym");
	const auto& seg  = kv.at("seg");
	const auto& seek = kv.at("seek");
	const auto& mask = kv.at("mask");
	
	auto a = new CAddr_Pattern(name, sym, seg, seek, mask);
	this->AddrEntry_Load_Common(a);
	this->m_AddrPtrs.push_back(std::unique_ptr<IAddr>(a));
	return SMCResult_Continue;
}

SMCResult CSigsegvGameConf::AddrEntry_Load_DataDescMap()
{
	const auto& name = this->m_AddrEntry_State.m_Name;
	const auto& kv = this->m_AddrEntry_State.m_KeyValues;
	
	for (const std::string& key : { "sym", "class" }) {
		if (kv.find(key) == kv.end()) {
			DevMsg("GameData error: addr \"%s\" lacks required key \"%s\"\n", name.c_str(), key.c_str());
			return SMCResult_HaltFail;
		}
	}
	
	const auto& sym    = kv.at("sym");
	const auto& c_name = kv.at("class");
	
	auto a = new CAddr_DataDescMap(name, sym, c_name);
	this->AddrEntry_Load_Common(a);
	this->m_AddrPtrs.push_back(std::unique_ptr<IAddr>(a));
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
	int idx            = stoi(kv.at("idx"), nullptr, 0);
	
	auto a = new CAddr_Func_KnownVTIdx(name, sym, vtable, idx);
	this->AddrEntry_Load_Common(a);
	this->m_AddrPtrs.push_back(std::unique_ptr<IAddr>(a));
	return SMCResult_Continue;
}

SMCResult CSigsegvGameConf::AddrEntry_Load_Func_DataMap_VThunk()
{
	const auto& name = this->m_AddrEntry_State.m_Name;
	const auto& kv = this->m_AddrEntry_State.m_KeyValues;
	
	for (const std::string& key : { "sym", "datamap", "func", "vtable" }) {
		if (kv.find(key) == kv.end()) {
			DevMsg("GameData error: addr \"%s\" lacks required key \"%s\"\n", name.c_str(), key.c_str());
			return SMCResult_HaltFail;
		}
	}
	
	const auto& sym     = kv.at("sym");
	const auto& datamap = kv.at("datamap");
	const auto& f_name  = kv.at("func");
	const auto& vtable  = kv.at("vtable");
	
	auto a = new CAddr_Func_DataMap_VThunk(name, sym, datamap, f_name, vtable);
	this->AddrEntry_Load_Common(a);
	this->m_AddrPtrs.push_back(std::unique_ptr<IAddr>(a));
	return SMCResult_Continue;
}

SMCResult CSigsegvGameConf::AddrEntry_Load_Func_EBPPrologue_UniqueRef()
{
	const auto& name = this->m_AddrEntry_State.m_Name;
	const auto& kv = this->m_AddrEntry_State.m_KeyValues;
	
	for (const std::string& key : { "sym", "uniref" }) {
		if (kv.find(key) == kv.end()) {
			DevMsg("GameData error: addr \"%s\" lacks required key \"%s\"\n", name.c_str(), key.c_str());
			return SMCResult_HaltFail;
		}
	}
	
	const auto& sym    = kv.at("sym");
	const auto& uniref = kv.at("uniref");
	
	auto a = new CAddr_Func_EBPPrologue_UniqueRef(name, sym, uniref);
	this->AddrEntry_Load_Common(a);
	this->m_AddrPtrs.push_back(std::unique_ptr<IAddr>(a));
	return SMCResult_Continue;
}

SMCResult CSigsegvGameConf::AddrEntry_Load_Func_EBPPrologue_UniqueStr()
{
	const auto& name = this->m_AddrEntry_State.m_Name;
	const auto& kv = this->m_AddrEntry_State.m_KeyValues;
	
	for (const std::string& key : { "sym", "unistr" }) {
		if (kv.find(key) == kv.end()) {
			DevMsg("GameData error: addr \"%s\" lacks required key \"%s\"\n", name.c_str(), key.c_str());
			return SMCResult_HaltFail;
		}
	}
	
	const auto& sym    = kv.at("sym");
	const auto& unistr = kv.at("unistr");
	
	auto a = new CAddr_Func_EBPPrologue_UniqueStr(name, sym, unistr);
	this->AddrEntry_Load_Common(a);
	this->m_AddrPtrs.push_back(std::unique_ptr<IAddr>(a));
	return SMCResult_Continue;
}

SMCResult CSigsegvGameConf::AddrEntry_Load_Func_EBPPrologue_UniqueStr_KnownVTIdx()
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
	int idx            = stoi(kv.at("idx"), nullptr, 0);
	
	auto a = new CAddr_Func_EBPPrologue_UniqueStr_KnownVTIdx(name, sym, unistr, vtable, idx);
	this->AddrEntry_Load_Common(a);
	this->m_AddrPtrs.push_back(std::unique_ptr<IAddr>(a));
	return SMCResult_Continue;
}

SMCResult CSigsegvGameConf::AddrEntry_Load_Func_EBPPrologue_NonUniqueStr_KnownVTIdx()
{
	const auto& name = this->m_AddrEntry_State.m_Name;
	const auto& kv = this->m_AddrEntry_State.m_KeyValues;
	
	for (const std::string& key : { "sym", "str", "vtable", "idx" }) {
		if (kv.find(key) == kv.end()) {
			DevMsg("GameData error: addr \"%s\" lacks required key \"%s\"\n", name.c_str(), key.c_str());
			return SMCResult_HaltFail;
		}
	}
	
	const auto& sym    = kv.at("sym");
	const auto& str    = kv.at("str");
	const auto& vtable = kv.at("vtable");
	int idx            = stoi(kv.at("idx"), nullptr, 0);
	
	auto a = new CAddr_Func_EBPPrologue_NonUniqueStr_KnownVTIdx(name, sym, str, vtable, idx);
	this->AddrEntry_Load_Common(a);
	this->m_AddrPtrs.push_back(std::unique_ptr<IAddr>(a));
	return SMCResult_Continue;
}

SMCResult CSigsegvGameConf::AddrEntry_Load_Func_EBPPrologue_VProf()
{
	const auto& name = this->m_AddrEntry_State.m_Name;
	const auto& kv = this->m_AddrEntry_State.m_KeyValues;
	
	for (const std::string& key : { "sym", "v_name", "v_group" }) {
		if (kv.find(key) == kv.end()) {
			DevMsg("GameData error: addr \"%s\" lacks required key \"%s\"\n", name.c_str(), key.c_str());
			return SMCResult_HaltFail;
		}
	}
	
	const auto& sym     = kv.at("sym");
	const auto& v_name  = kv.at("v_name");
	const auto& v_group = kv.at("v_group");
	
	auto a = new CAddr_Func_EBPPrologue_VProf(name, sym, v_name, v_group);
	this->AddrEntry_Load_Common(a);
	this->m_AddrPtrs.push_back(std::unique_ptr<IAddr>(a));
	return SMCResult_Continue;
}

SMCResult CSigsegvGameConf::AddrEntry_Load_ConCommandBase(bool is_command)
{
	const auto& name = this->m_AddrEntry_State.m_Name;
	const auto& kv = this->m_AddrEntry_State.m_KeyValues;
	
	for (const std::string& key : { "name" }) {
		if (kv.find(key) == kv.end()) {
			DevMsg("GameData error: addr \"%s\" lacks required key \"%s\"\n", name.c_str(), key.c_str());
			return SMCResult_HaltFail;
		}
	}
	
	const auto& con_name = kv.at("name");
	
	auto a = new CAddr_ConCommandBase(name, con_name, is_command);
	this->AddrEntry_Load_Common(a);
	this->m_AddrPtrs.push_back(std::unique_ptr<IAddr>(a));
	return SMCResult_Continue;
}
