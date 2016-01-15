#include "addr/addr.h"


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
		DevMsg("IAddr::Init \"%s\" OK 0x%08x\n", this->GetName(), this->m_iAddr);
	} else {
		this->m_State = State::FAIL;
		DevMsg("IAddr::Init \"%s\" FAIL\n", this->GetName());
	}
}


std::map<std::string, IAddr *> AddrManager::s_Addrs;

void *AddrManager::s_hServer = nullptr;
void *AddrManager::s_hEngine = nullptr;


void AddrManager::Load()
{
	DevMsg("AddrManager::Load BEGIN\n");
	
	OpenLibHandle(&s_hServer, LibMgr::GetPtr(Library::SERVER), "server");
	OpenLibHandle(&s_hEngine, LibMgr::GetPtr(Library::ENGINE), "engine");
	
	for (auto addr : AutoList<IAddr>::List()) {
		std::string name(addr->GetName());
		
		assert(s_Addrs.find(name) == s_Addrs.end());
		s_Addrs[name] = addr;
	}
	
	/* early init pass to ensure vtables are ready */
	for (auto addr : AutoList<IAddr>::List()) {
		if (addr->ShouldInitFirst()) {
			addr->Init();
		}
	}
	
	for (auto addr : AutoList<IAddr>::List()) {
		addr->Init();
	}
	
	DevMsg("AddrManager::Load END\n");
}

void AddrManager::UnLoad()
{
	CloseLibHandle(&s_hServer);
	CloseLibHandle(&s_hEngine);
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


void *AddrManager::FindSymbol(Library lib, const char *sym)
{
	void *handle = nullptr;
	switch (lib) {
	case Library::SERVER:
		handle = s_hServer;
		break;
	case Library::ENGINE:
		handle = s_hEngine;
		break;
	}
	assert(handle != nullptr);
	
	return g_MemUtils.ResolveSymbol(handle, sym);
}


#if defined _LINUX || defined _OSX

void AddrManager::OpenLibHandle(void **handle, void *ptr, const char *name)
{
	Dl_info info;
	
	if (dladdr(ptr, &info) == 0) {
		DevMsg("AddrManager::OpenLibHandle: can't find library \"%s\" in memory\n"
			"dladdr error:\n%s\n", name, dlerror());
		
		*handle = nullptr;
		return;
	}
	
	*handle = dlopen(info.dli_fname, RTLD_NOW);
	if (*handle == nullptr) {
		DevMsg("AddrManager::OpenLibHandle: can't load library \"%s\"\n"
			"dlopen error:\n%s\n", name, dlerror());
		
		return;
	}
}

void AddrManager::CloseLibHandle(void **handle)
{
	if (*handle != nullptr) {
		dlclose(*handle);
		*handle = nullptr;
	}
}

#elif defined _WINDOWS

void AddrManager::OpenLibHandle(void **handle, void *ptr, const char *name)
{
	MEMORY_BASIC_INFORMATION mem;
	
	if (VirtualQuery(ptr, &mem, sizeof(mem)) == 0) {
		char err[4096];
		libsys->GetPlatformError(err, sizeof(err));
		
		DevMsg("AddrManager::OpenLibHandle: can't find library \"%s\" in memory\n"
			"VirtualQuery error:\n%s\n", name, err);
		
		*handle = nullptr;
		return;
	}
	
	*handle = mem.AllocationBase;
}

void AddrManager::CloseLibHandle(void **handle)
{
	*handle = nullptr;
}

#endif
