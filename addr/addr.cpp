#include "addr/addr.h"
#include "addr/prescan.h"
#include "abi.h"


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
	
	/* early pass 1: vtables */
	for (auto addr : AutoList<IAddr>::List()) {
		if (dynamic_cast<IAddr_VTable *>(addr) != nullptr) {
			addr->Init();
		}
	}
	
	/* main pass */
	for (auto addr : AutoList<IAddr>::List()) {
		addr->Init();
	}
	
	DevMsg("AddrManager::Load END\n");
}


void *AddrManager::GetAddr(const char *name)
{
	auto it = s_Addrs.find(std::string(name));
	if (it == s_Addrs.end()) {
		DevMsg("AddrManager::GetAddr FAIL: no addr exists with name \"%s\"\n", name);
		return nullptr;
	}
	
	const IAddr *addr = (*it).second;
	
	IAddr::State state = addr->GetState();
	assert(state != IAddr::State::INITIAL);
	
	if (state == IAddr::State::FAIL) {
		DevMsg("AddrManager::GetAddr FAIL: addr for name \"%s\" couldn't resolve\n", name);
		return nullptr;
	}
	
	return addr->GetAddr();
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


bool IAddr_VTable::FindAddrLinux(uintptr_t& addr) const
{
	bool result = IAddr_Sym::FindAddrLinux(addr);
	
	if (result) {
		addr += offsetof(vtable, vfptrs);
	}
	
	return result;
}

bool IAddr_VTable::FindAddrWin(uintptr_t& addr) const
{
	/* STEP 1: get ptr to _TypeDescriptor by finding typeinfo name string */
	std::vector<const void *> scan1_matches;
	for (auto match : PreScan::WinRTTI_Server()) {
		if (strcmp((const char *)match, this->GetWinRTTIStr()) == 0) {
			scan1_matches.push_back(match);
		}
	}
	if (scan1_matches.size() == 0) {
		DevMsg("IAddr_VTable: \"%s\": could not find RTTI string\n", this->GetName());
		return false;
	}
	if (scan1_matches.size() > 1) {
		DevMsg("IAddr_VTable: \"%s\": too many matches (%u) for RTTI string\n", this->GetName(), scan1_matches.size());
	}
	auto *p_TD = (_TypeDescriptor *)((uintptr_t)scan1_matches[0] - offsetof(_TypeDescriptor, name));
	
	/* STEP 2: get ptr to __RTTI_CompleteObjectLocator by finding references to the _TypeDescriptor */
	__RTTI_CompleteObjectLocator seek_COL = {
		0x00000000,
		0x00000000,
		0x00000000,
		p_TD,
	};
	CSingleScan scan2(ScanDir::FORWARD, CLibSegBounds(this->GetLibrary(), ".rdata"), 4, new CBasicScanner(ScanResults::ALL, (const void *)&seek_COL, 0x10));
	if (scan2.Matches().size() == 0) {
		DevMsg("IAddr_VTable: \"%s\": could not find ref to _TypeDescriptor\n", this->GetName());
		return false;
	}
	if (scan2.Matches().size() > 1) {
		DevMsg("IAddr_VTable: \"%s\": too many refs (%u) to _TypeDescriptor\n", this->GetName(), scan2.Matches().size());
	}
	auto *p_COL = (__RTTI_CompleteObjectLocator *)scan2.Matches()[0];
	
	/* STEP 3: get ptr to the vtable itself by finding references to the __RTTI_CompleteObjectLocator */
	CSingleScan scan3(ScanDir::FORWARD, CLibSegBounds(this->GetLibrary(), ".rdata"), 4, new CBasicScanner(ScanResults::ALL, (const void *)&p_COL, 0x4));
	if (scan3.Matches().size() == 0) {
		DevMsg("IAddr_VTable: \"%s\": could not find ref to __RTTI_CompleteObjectLocator\n", this->GetName());
		return false;
	}
	if (scan3.Matches().size() > 1) {
		DevMsg("IAddr_VTable: \"%s\": too many refs (%u) to __RTTI_CompleteObjectLocator\n", this->GetName(), scan3.Matches().size());
	}
	auto p_VT = (void **)((uintptr_t)scan3.Matches()[0] + 0x4);
	
	addr = (uintptr_t)p_VT;
	return true;
}


bool IAddr_Func_KnownVTIdx::FindAddrWin(uintptr_t& addr) const
{
	auto p_VT = (const uintptr_t *)AddrManager::GetAddr(this->GetVTableName());
	if (p_VT == nullptr) {
		DevMsg("IAddr_Func_KnownVTIdx: \"%s\": no addr for vtable\n", this->GetName());
		return false;
	}
	
	addr = p_VT[this->GetVTableIndex()];
	return true;
}


bool IAddr_Func_EBPPrologue_UniqueStr::FindAddrWin(uintptr_t& addr) const
{
	CSingleScan scan1(ScanDir::FORWARD, CLibSegBounds(this->GetLibrary(), ".rdata"), 1, new CStringScanner(ScanResults::ALL, this->GetUniqueStr()));
	if (scan1.Matches().size() != 1) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueStr: \"%s\": found %u matches for ostensibly unique string\n", this->GetName(), scan1.Matches().size());
		return false;
	}
	auto p_str = (const char *)scan1.Matches()[0];
	
	CSingleScan scan2(ScanDir::FORWARD, CLibSegBounds(this->GetLibrary(), ".text"), 1, new CBasicScanner(ScanResults::ALL, (const void *)&p_str, 0x4));
	if (scan2.Matches().size() != 1) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueStr: \"%s\": found %u refs to ostensibly unique string\n", this->GetName(), scan2.Matches().size());
		return false;
	}
	auto p_in_func = (const char **)scan2.Matches()[0];
	
	constexpr uint8_t prologue[] = {
		0x55,       // +0000  push ebp
		0x8b, 0xec, // +0001  mov ebp,esp
	};
	CSingleScan scan3(ScanDir::REVERSE, CAddrOffBounds(p_in_func, -0x1000), 0x10, new CBasicScanner(ScanResults::FIRST, (const void *)prologue, sizeof(prologue)));
	if (scan3.Matches().size() != 1) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueStr: \"%s\": could not locate EBP prologue\n", this->GetName());
		return false;
	}
	auto p_func = (uintptr_t)scan3.Matches()[0];
	
	addr = p_func;
	return true;
}


bool IAddr_Func_EBPPrologue_UniqueStr_KnownVTIdx::FindAddrWin(uintptr_t& addr) const
{
	CSingleScan scan1(ScanDir::FORWARD, CLibSegBounds(this->GetLibrary(), ".rdata"), 1, new CStringScanner(ScanResults::ALL, this->GetUniqueStr()));
	if (scan1.Matches().size() != 1) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueStr_KnownVTIdx: \"%s\": found %u matches for ostensibly unique string\n", this->GetName(), scan1.Matches().size());
		return false;
	}
	auto p_str = (const char *)scan1.Matches()[0];
	
	CSingleScan scan2(ScanDir::FORWARD, CLibSegBounds(this->GetLibrary(), ".text"), 1, new CBasicScanner(ScanResults::ALL, (const void *)&p_str, 0x4));
	if (scan2.Matches().size() != 1) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueStr_KnownVTIdx: \"%s\": found %u refs to ostensibly unique string\n", this->GetName(), scan2.Matches().size());
		return false;
	}
	auto p_in_func = (const char **)scan2.Matches()[0];
	
	constexpr uint8_t prologue[] = {
		0x55,       // +0000  push ebp
		0x8b, 0xec, // +0001  mov ebp,esp
	};
	CSingleScan scan3(ScanDir::REVERSE, CAddrOffBounds(p_in_func, -0x1000), 0x10, new CBasicScanner(ScanResults::FIRST, (const void *)prologue, sizeof(prologue)));
	if (scan3.Matches().size() != 1) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueStr_KnownVTIdx: \"%s\": could not locate EBP prologue\n", this->GetName());
		return false;
	}
	auto p_func = (uintptr_t)scan3.Matches()[0];
	
	auto p_VT = (const uintptr_t *)AddrManager::GetAddr(this->GetVTableName());
	if (p_VT == nullptr) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueStr_KnownVTIdx: \"%s\": no addr for vtable\n", this->GetName());
		return false;
	}
	
	uintptr_t vfptr = p_VT[this->GetVTableIndex()];
	if (vfptr != p_func) {
		DevMsg("IAddr_Func_EBPPrologue_UniqueStr_KnownVTIdx: \"%s\": func addr (0x%08x) doesn't match vtable entry (0x%08x)\n", this->GetName(), p_func, vfptr);
		return false;
	}
	
	addr = p_func;
	return true;
}
