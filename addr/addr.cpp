#include "addr/addr.h"
#include "addr/prescan.h"
#include "abi.h"
#include "util/rtti.h"


void IAddr::Init()
{
	if (this->m_State != State::INITIAL) {
		return;
	}
	
#if defined _LINUX
	bool result = this->FindAddrLinux(this->m_iAddr);
#elif defined _OSX
	bool result = this->FindAddrOSX(this->m_iAddr);
#elif defined _WINDOWS
	bool result = this->FindAddrWin(this->m_iAddr);
#else
	bool result = false;
#endif
	
	if (result) {
		this->m_State = State::OK;
//		DevMsg("IAddr::Init \"%s\" OK 0x%08x\n", this->GetName(), this->m_iAddr);
	} else {
		this->m_State = State::FAIL;
		DevMsg("IAddr::Init \"%s\" FAIL\n", this->GetName());
	}
}


std::map<std::string, IAddr *> AddrManager::s_Addrs;
bool AddrManager::s_bLoading = false;


void AddrManager::Load()
{
	DevMsg("AddrManager::Load BEGIN\n");
	
	for (auto addr : AutoList<IAddr>::List()) {
		std::string name(addr->GetName());
		
		if (s_Addrs.find(name) == s_Addrs.end()) {
			s_Addrs[name] = addr;
		} else {
			DevMsg("AddrManager::Load: duplicate addr for \"%s\"\n", addr->GetName());
		}
	}
	
#if 0
	/* early pass 1: vtables */
	for (auto addr : AutoList<IAddr>::List()) {
		if (dynamic_cast<IAddr_VTable *>(addr) != nullptr) {
			addr->Init();
		}
	}
#endif
	
	s_bLoading = true;
	
	/* main pass */
	for (auto addr : AutoList<IAddr>::List()) {
		addr->Init();
	}
	
	s_bLoading = false;
	
	DevMsg("AddrManager::Load END\n");
}


void *AddrManager::GetAddr(const char *name)
{
	auto it = s_Addrs.find(std::string(name));
	if (it == s_Addrs.end()) {
		DevMsg("AddrManager::GetAddr FAIL: no addr exists with name \"%s\"\n", name);
		return nullptr;
	}
	
	IAddr *addr = (*it).second;
	
	/* try to handle dependency ordering somewhat automatically */
	if (s_bLoading && addr->GetState() == IAddr::State::INITIAL) {
		addr->Init();
	}
	
	IAddr::State state = addr->GetState();
	assert(state != IAddr::State::INITIAL);
	
	if (state == IAddr::State::FAIL) {
		DevMsg("AddrManager::GetAddr FAIL: have but cannot resolve addr for name \"%s\"\n", name);
		return nullptr;
	}
	
	return addr->GetAddr();
}


static ConCommand ccmd_list_addrs("sigsegv_list_addrs", &AddrManager::CC_ListAddrs,
	"List addresses and show their status", FCVAR_NONE);
void AddrManager::CC_ListAddrs(const CCommand& cmd)
{
	for (const auto& pair : s_Addrs) {
		const IAddr *addr = pair.second;
		
		switch (addr->GetState()) {
		case IAddr::State::INITIAL:
			Msg("%-8s %s\n", "INITIAL", addr->GetName());
			break;
		case IAddr::State::OK:
			Msg("%08x %s\n", addr->GetAddr(), addr->GetName());
			break;
		case IAddr::State::FAIL:
			Msg("%-8s %s\n", "FAIL", addr->GetName());
			break;
		}
	}
}


bool IAddr_Sym::FindAddrLinux(uintptr_t& addr) const
{
	void *sym_addr = LibMgr::FindSym(this->GetLibrary(), this->GetSymbol());
	if (sym_addr == nullptr) {
		return false;
	}
	
	addr = (uintptr_t)sym_addr;
	return true;
}


bool IAddr_DataDescMap::FindAddrWin(uintptr_t& addr) const
{
	const char *p_str = Scan::FindUniqueConstStr(this->GetClassName());
	if (p_str == nullptr) {
		DevMsg("IAddr_DataDescMap: \"%s\": failed to find class name string\n", this->GetName());
		return false;
	}
	
	CSingleScan<ScanDir::FORWARD, 4> scan1(CLibSegBounds(this->GetLibrary(), ".data"), new CBasicScanner(ScanResults::ALL, (const void *)&p_str, 0x4));
	struct GetDataDescMap { uint8_t buf[6]; };
	std::vector<IScanner *> scanners;
	for (auto match : scan1.Matches()) {
		GetDataDescMap gddm;
		gddm.buf[0x00] = 0xb8; // mov eax,[????????]
		*(uint32_t *)(&gddm.buf[0x01]) = (uint32_t)match - offsetof(datamap_t, dataClassName);
		gddm.buf[0x05] = 0xc3; // ret
		
		scanners.push_back(new CBasicScanner(ScanResults::ALL, (const void *)&gddm, 0x6));
	}
	
	CMultiScan<ScanDir::FORWARD, 0x10> scan2(CLibSegBounds(this->GetLibrary(), ".text"), scanners);
	std::vector<const void *> results;
	for (auto scanner : scanners) {
		if (scanner->Matches().size() == 1) {
			results.push_back(scanner->Matches()[0]);
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
	auto p_ref = AddrManager::GetAddr(this->GetUniqueSymbol());
	if (p_ref == nullptr) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueRef: \"%s\": no addr for ostensibly unique symbol\n", this->GetName());
		return false;
	}
	
	CSingleScan<ScanDir::FORWARD, 1> scan1(CLibSegBounds(this->GetLibrary(), ".text"), new CBasicScanner(ScanResults::ALL, (const void *)&p_ref, 0x4));
	if (scan1.Matches().size() != 1) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueRef: \"%s\": found %u refs to ostensibly unique symbol\n", this->GetName(), scan1.Matches().size());
		return false;
	}
	auto p_in_func = (const char **)scan1.Matches()[0];
	
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
	const char *p_str = Scan::FindUniqueConstStr(this->GetUniqueStr());
	if (p_str == nullptr) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueStr: \"%s\": failed to find ostensibly unique string\n", this->GetName());
		return false;
	}
	
	CSingleScan<ScanDir::FORWARD, 1> scan1(CLibSegBounds(this->GetLibrary(), ".text"), new CBasicScanner(ScanResults::ALL, (const void *)&p_str, 0x4));
	if (scan1.Matches().size() != 1) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueStr: \"%s\": found %u refs to ostensibly unique string\n", this->GetName(), scan1.Matches().size());
		return false;
	}
	auto p_in_func = (const char **)scan1.Matches()[0];
	
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
	const char *p_str = Scan::FindUniqueConstStr(this->GetUniqueStr());
	if (p_str == nullptr) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueStr_KnownVTIdx: \"%s\": failed to find ostensibly unique string\n", this->GetName());
		return false;
	}
	
	CSingleScan<ScanDir::FORWARD, 1> scan1(CLibSegBounds(this->GetLibrary(), ".text"), new CBasicScanner(ScanResults::ALL, (const void *)&p_str, 0x4));
	if (scan1.Matches().size() != 1) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueStr_KnownVTIdx: \"%s\": found %u refs to ostensibly unique string\n", this->GetName(), scan1.Matches().size());
		return false;
	}
	auto p_in_func = (const char **)scan1.Matches()[0];
	
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
	const char *p_name = Scan::FindUniqueConstStr(this->GetVProfName());
	if (p_name == nullptr) {
		DevMsg("IAddr_Func_EBPPrologue_VProf: \"%s\": failed to find name string\n", this->GetName());
		return false;
	}
	
	const char *p_group = Scan::FindUniqueConstStr(this->GetVProfGroup());
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
	CSingleScan<ScanDir::FORWARD, 1> scan1(CLibSegBounds(this->GetLibrary(), ".text"), new CBasicScanner(ScanResults::FIRST, (const void *)vprof, sizeof(vprof)));
	if (scan1.Matches().size() != 1) {
		DevMsg("IAddr_Func_EBPPrologue_VProf: \"%s\": could not locate VPROF_BUDGET\n", this->GetName());
		return false;
	}
	auto p_in_func = scan1.Matches()[0];
	
	auto p_func = Scan::FindFuncPrologue(p_in_func);
	if (p_func == nullptr) {
		DevMsg("IAddr_Func_EBPPrologue_VProf: \"%s\": could not locate EBP prologue\n", this->GetName());
		return false;
	}
	
	addr = (uintptr_t)p_func;
	return true;
}
