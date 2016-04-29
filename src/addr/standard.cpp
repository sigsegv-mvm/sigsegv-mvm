#include "addr/standard.h"
#include "mem/scan.h"
#include "util/rtti.h"


bool IAddr_Sym::FindAddrLinux(uintptr_t& addr) const
{
	void *sym_addr = LibMgr::FindSym(this->GetLibrary(), this->GetSymbol());
	if (sym_addr == nullptr) {
		return false;
	}
	
	addr = (uintptr_t)sym_addr;
	return true;
}


bool IAddr_FixedAddr::FindAddrWin(uintptr_t& addr) const
{
	if (engine->GetServerVersion() != this->GetServerVersion()) {
		DevMsg("IAddr_FixedAddr: \"%s\": version mismatch (%d vs %d)\n", this->GetName(), engine->GetServerVersion(), this->GetServerVersion());
		return false;
	}
	
	const LibInfo& info = LibMgr::GetInfo(this->GetLibrary());
	addr = info.baseaddr + this->GetAddress();
	return true;
}


bool IAddr_DataDescMap::FindAddrWin(uintptr_t& addr) const
{
	struct GetDataDescMap {
		bool operator==(const GetDataDescMap& that) const { return (memcmp(this, &that, sizeof(*this)) == 0); }
		uint8_t buf[6];
	};
	
	using ClassNameRefScanner   = CTypeScanner<ScanDir::FORWARD, ScanResults::ALL,  0x4, const char *>;
	using GetDataDescMapScanner = CTypeScanner<ScanDir::FORWARD, ScanResults::ALL, 0x10, GetDataDescMap>;
	
	const char *p_str = Scan::FindUniqueConstStr(this->GetLibrary(), this->GetClassName());
	if (p_str == nullptr) {
		DevMsg("IAddr_DataDescMap: \"%s\": failed to find class name string\n", this->GetName());
		return false;
	}
	
	CScan<ClassNameRefScanner> scan1(CLibSegBounds(this->GetLibrary(), ".data"), p_str);
	std::vector<GetDataDescMapScanner *> scanners;
	for (auto match : scan1.Matches()) {
		GetDataDescMap gddm;
		gddm.buf[0x00] = 0xb8; // mov eax,[????????]
		*(uint32_t *)(&gddm.buf[0x01]) = (uint32_t)match - offsetof(datamap_t, dataClassName);
		gddm.buf[0x05] = 0xc3; // ret
		
		scanners.push_back(new GetDataDescMapScanner(CLibSegBounds(this->GetLibrary(), ".text"), gddm));
	}
	
	CMultiScan<GetDataDescMapScanner> scan2(scanners);
	std::vector<const void *> results;
	for (auto scanner : scanners) {
		if (scanner->ExactlyOneMatch()) {
			results.push_back(scanner->FirstMatch());
		}
	}
	if (results.size() != 1) {
		DevMsg("IAddr_DataDescMap: \"%s\": found %u matches for GetDataDescMap func\n", this->GetName(), results.size());
		return false;
	}
	
	addr = *(uintptr_t *)((uintptr_t)results[0] + 1);
	return true;
}


bool IAddr_Func_KnownVTIdx::FindAddrWin(uintptr_t& addr) const
{
	auto p_VT = RTTI::GetVTable(this->GetVTableName());
	if (p_VT == nullptr) {
		DevMsg("IAddr_Func_KnownVTIdx: \"%s\": no addr for vtable\n", this->GetName());
		return false;
	}
	
	addr = (uintptr_t)p_VT[this->GetVTableIndex()];
	return true;
}


bool IAddr_Func_DataMap_VThunk::FindAddrWin(uintptr_t& addr) const
{
	auto p_DM = (const datamap_t *)AddrManager::GetAddr(this->GetDataMapName());
	if (p_DM == nullptr) {
		DevMsg("IAddr_Func_DataMap_VThunk: \"%s\": no addr for datamap\n", this->GetName());
		return false;
	}
	
	bool found_entry = false;
	void *p_VThunk = nullptr;
	for (int i = 0; i < p_DM->dataNumFields; ++i) {
		const typedescription_t *td = p_DM->dataDesc + i;
		
		if (td->fieldName != nullptr && strcmp(td->fieldName, this->GetFuncName()) == 0) {
			union {
				inputfunc_t inputFunc;
				void *voidptr;
			} u;
			
			u.inputFunc = td->inputFunc;
			p_VThunk = u.voidptr;
			
			found_entry = true;
			break;
		}
	}
	if (!found_entry) {
		DevMsg("IAddr_Func_DataMap_VThunk: \"%s\": could not find func entry in datamap\n", this->GetName());
		return false;
	}
	
	constexpr uint8_t thunk[] = {
		0x8b, 0x01, // mov eax,[ecx]
		0xff, 0xa0, // jmp dword ptr [eax+0x????????]
	};
	if (memcmp(p_VThunk, thunk, 0x4) != 0) {
		DevMsg("IAddr_Func_DataMap_VThunk: \"%s\": virtual thunk doesn't look quite right\n", this->GetName());
		return false;
	}
	uint32_t vt_off = *(uint32_t *)((uintptr_t)p_VThunk + 0x4);
	if (vt_off % 4 != 0) {
		DevMsg("IAddr_Func_DataMap_VThunk: \"%s\": vt_off 0x%x isn't a multiple of 4\n", this->GetName(), vt_off);
		return false;
	}
	uint32_t vt_idx = vt_off / 4;
	
	auto p_VT = RTTI::GetVTable(this->GetVTableName());
	if (p_VT == nullptr) {
		DevMsg("IAddr_Func_DataMap_VThunk: \"%s\": no addr for vtable\n", this->GetName());
		return false;
	}
	
	addr = (uintptr_t)p_VT[vt_idx];
	return true;
}


bool IAddr_Func_EBPPrologue_UniqueRef::FindAddrWin(uintptr_t& addr) const
{
	using SymRefScanner = CTypeScanner<ScanDir::FORWARD, ScanResults::ALL, 1, const void *>;
	
	auto p_ref = AddrManager::GetAddr(this->GetUniqueSymbol());
	if (p_ref == nullptr) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueRef: \"%s\": no addr for ostensibly unique symbol\n", this->GetName());
		return false;
	}
	
	CScan<SymRefScanner> scan1(CLibSegBounds(this->GetLibrary(), ".text"), p_ref);
	if (!scan1.ExactlyOneMatch()) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueRef: \"%s\": found %u refs to ostensibly unique symbol\n", this->GetName(), scan1.Matches().size());
		return false;
	}
	auto p_in_func = (const char **)scan1.FirstMatch();
	
	auto p_func = Scan::FindFuncPrologue(p_in_func);
	if (p_func == nullptr) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueRef: \"%s\": could not locate EBP prologue\n", this->GetName());
		return false;
	}
	
	addr = (uintptr_t)p_func;
	return true;
}


bool IAddr_Func_EBPPrologue_UniqueStr::FindAddrWin(uintptr_t& addr) const
{
	using StrRefScanner = CTypeScanner<ScanDir::FORWARD, ScanResults::ALL, 1, const char *>;
	
	const char *p_str = Scan::FindUniqueConstStr(this->GetLibrary(), this->GetUniqueStr());
	if (p_str == nullptr) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueStr: \"%s\": failed to find ostensibly unique string\n", this->GetName());
		return false;
	}
	
	CScan<StrRefScanner> scan1(CLibSegBounds(this->GetLibrary(), ".text"), p_str);
	if (!scan1.ExactlyOneMatch()) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueStr: \"%s\": found %u refs to ostensibly unique string\n", this->GetName(), scan1.Matches().size());
		return false;
	}
	auto p_in_func = (const char **)scan1.FirstMatch();
	
	auto p_func = Scan::FindFuncPrologue(p_in_func);
	if (p_func == nullptr) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueStr: \"%s\": could not locate EBP prologue\n", this->GetName());
		return false;
	}
	
	addr = (uintptr_t)p_func;
	return true;
}


bool IAddr_Func_EBPPrologue_UniqueStr_KnownVTIdx::FindAddrWin(uintptr_t& addr) const
{
	using StrRefScanner = CTypeScanner<ScanDir::FORWARD, ScanResults::ALL, 1, const char *>;
	
	const char *p_str = Scan::FindUniqueConstStr(this->GetLibrary(), this->GetUniqueStr());
	if (p_str == nullptr) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueStr_KnownVTIdx: \"%s\": failed to find ostensibly unique string\n", this->GetName());
		return false;
	}
	
	CScan<StrRefScanner> scan1(CLibSegBounds(this->GetLibrary(), ".text"), p_str);
	if (!scan1.ExactlyOneMatch()) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueStr_KnownVTIdx: \"%s\": found %u refs to ostensibly unique string\n", this->GetName(), scan1.Matches().size());
		return false;
	}
	auto p_in_func = (const char **)scan1.FirstMatch();
	
	auto p_func = Scan::FindFuncPrologue(p_in_func);
	if (p_func == nullptr) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueStr_KnownVTIdx: \"%s\": could not locate EBP prologue\n", this->GetName());
		return false;
	}
	
	auto p_VT = RTTI::GetVTable(this->GetVTableName());
	if (p_VT == nullptr) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueStr_KnownVTIdx: \"%s\": no addr for vtable\n", this->GetName());
		return false;
	}
	
	auto vfptr = p_VT[this->GetVTableIndex()];
	if (vfptr != p_func) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueStr_KnownVTIdx: \"%s\": func addr (0x%08x) doesn't match vtable entry (0x%08x)\n", this->GetName(), p_func, vfptr);
		return false;
	}
	
	addr = (uintptr_t)p_func;
	return true;
}


bool IAddr_Func_EBPPrologue_VProf::FindAddrWin(uintptr_t& addr) const
{
#if defined __GNUC__
#warning FIXME: VPROF_BUDGET finder is not reliable due to using first scan result
#endif
	using VProfScanner = CBasicScanner<ScanDir::FORWARD, ScanResults::FIRST, 1>;
	
	const char *p_name = Scan::FindUniqueConstStr(this->GetLibrary(), this->GetVProfName());
	if (p_name == nullptr) {
		DevMsg("IAddr_Func_EBPPrologue_VProf: \"%s\": failed to find name string\n", this->GetName());
		return false;
	}
	
	const char *p_group = Scan::FindUniqueConstStr(this->GetLibrary(), this->GetVProfGroup());
	if (p_group == nullptr) {
		DevMsg("IAddr_Func_EBPPrologue_VProf: \"%s\": failed to find group string\n", this->GetName());
		return false;
	}
	
	uint8_t vprof[] = {
		0x68, 0x00, 0x00, 0x00, 0x00, // push 0x????????
		0x68, 0x00, 0x00, 0x00, 0x00, // push 0x????????
	};
	*(const char **)(vprof + 0x01) = p_name;
	*(const char **)(vprof + 0x06) = p_group;
	CScan<VProfScanner> scan1(CLibSegBounds(this->GetLibrary(), ".text"), (const void *)vprof, sizeof(vprof));
	if (!scan1.ExactlyOneMatch()) {
		DevMsg("IAddr_Func_EBPPrologue_VProf: \"%s\": could not locate VPROF_BUDGET\n", this->GetName());
		return false;
	}
	auto p_in_func = scan1.FirstMatch();
	
	auto p_func = Scan::FindFuncPrologue(p_in_func);
	if (p_func == nullptr) {
		DevMsg("IAddr_Func_EBPPrologue_VProf: \"%s\": could not locate EBP prologue\n", this->GetName());
		return false;
	}
	
	addr = (uintptr_t)p_func;
	return true;
}


#if 0
bool IAddr_Func_EBPPrologue_UniqueConVar::FindAddrWin(uintptr_t& addr) const
{
	using ConVarRefScanner = CTypeScanner<ScanDir::FORWARD, ScanResults::ALL, 1, uintptr_t>;
	
	ConVarRef cvref(this->GetConVarName());
	if (!cvref.IsValid()) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueConVar: \"%s\": no convar named \"%s\" exists\n", this->GetName(), this->GetConVarName());
		return false;
	}
	
	ConVar *cvar = static_cast<ConVar *>(cvref.GetLinkedConVar());
	cvar = *(ConVar **)((uintptr_t)cvar + offsetof(ConVar, m_pParent));
	
	uintptr_t p_cvar;
	switch (this->GetConVarType()) {
	case DataType::INT:
		p_cvar = (uintptr_t)cvar + offsetof(ConVar, m_nValue);
		break;
	case DataType::FLOAT:
		p_cvar = (uintptr_t)cvar + offsetof(ConVar, m_fValue);
		break;
	case DataType::STRING:
		p_cvar = (uintptr_t)cvar + offsetof(ConVar, m_pszString);
		break;
	default:
		DevMsg("IAddr_Func_EBPPrologue_UniqueConVar: \"%s\": invalid data type\n", this->GetName());
		return false;
	}
	
//	const char *p_str = Scan::FindUniqueConstStr(this->GetLibrary(), this->GetUniqueStr());
//	if (p_str == nullptr) {
//		DevMsg("IAddr_Func_EBPPrologue_UniqueStr: \"%s\": failed to find ostensibly unique string\n", this->GetName());
//		return false;
//	}
//	
//	CScan<StrRefScanner> scan1(CLibSegBounds(this->GetLibrary(), ".text"), p_str);
//	if (!scan1.ExactlyOneMatch()) {
//		DevMsg("IAddr_Func_EBPPrologue_UniqueStr: \"%s\": found %u refs to ostensibly unique string\n", this->GetName(), scan1.Matches().size());
//		return false;
//	}
//	auto p_in_func = (const char **)scan1.FirstMatch();
	
	auto p_func = Scan::FindFuncPrologue(p_in_func);
	if (p_func == nullptr) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueStr: \"%s\": could not locate EBP prologue\n", this->GetName());
		return false;
	}
	
	addr = (uintptr_t)p_func;
	return true;
}
#endif


bool IAddr_Pattern::FindAddrWin(uintptr_t& addr) const
{
	using PatternScanner = CMaskedScanner<ScanDir::FORWARD, ScanResults::ALL, 1>;
	
	const char *str_seek = this->GetPattern();
	const char *str_mask = this->GetMask();
	
	size_t strlen_seek = strlen(str_seek);
	size_t strlen_mask = strlen(str_mask);
	
	if (strlen_seek != strlen_mask) {
		DevMsg("IAddr_Pattern: \"%s\": pattern and mask have differing lengths\n", this->GetName());
		return false;
	}
	
	if (strlen_seek % 2 == 1) {
		DevMsg("IAddr_Pattern: \"%s\": pattern and mask have odd number of characters\n", this->GetName());
		return false;
	}
	
	size_t len = strlen_seek / 2;
	
	ByteBuf seek(len);
	ByteBuf mask(len);
	
	for (size_t i = 0; i < len; ++i) {
		char buf[3];
		buf[2] = '\0';
		
		memcpy(buf, str_seek + (i * 2), 2);
		seek[i] = std::stoi(buf, nullptr, 0x10);
		
		memcpy(buf, str_mask + (i * 2), 2);
		mask[i] = std::stoi(buf, nullptr, 0x10);
	}
	
	CScan<PatternScanner> scan1(CLibSegBounds(this->GetLibrary(), this->GetSegment()), seek, mask);
	if (!scan1.ExactlyOneMatch()) {
		DevMsg("IAddr_Pattern: \"%s\": found %u pattern matches\n", this->GetName(), scan1.Matches().size());
		return false;
	}
	
	addr = (uintptr_t)scan1.FirstMatch();
	return true;
}
