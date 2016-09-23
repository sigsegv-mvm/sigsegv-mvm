#include "addr/addr.h"
#include "util/misc.h"


void IAddr::Init()
{
	if (this->m_State != State::INITIAL) {
		return;
	}
	
	this->m_State = State::LOADING;
	
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
	assert(state != IAddr::State::LOADING);
	
	if (state == IAddr::State::FAIL) {
		DevMsg("AddrManager::GetAddr FAIL: have but cannot resolve addr for name \"%s\"\n", name);
		return nullptr;
	}
	
	return addr->GetAddr();
}


static ConCommand ccmd_list_addrs("sig_list_addrs", &AddrManager::CC_ListAddrs,
	"List addresses and show their status", FCVAR_NONE);
void AddrManager::CC_ListAddrs(const CCommand& cmd)
{
	MAT_SINGLE_THREAD_BLOCK {
		for (const auto& pair : s_Addrs) {
			const IAddr *addr = pair.second;
			
			switch (addr->GetState()) {
			case IAddr::State::INITIAL:
				Msg("%-8s %s\n", "INITIAL", addr->GetName());
				break;
			case IAddr::State::OK:
				Msg("%08x %s\n", (uintptr_t)addr->GetAddr(), addr->GetName());
				break;
			case IAddr::State::FAIL:
				Msg("%-8s %s\n", "FAIL", addr->GetName());
				break;
			}
		}
	}
}
