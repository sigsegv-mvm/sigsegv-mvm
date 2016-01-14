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


void AddrManager::Load()
{
	DevMsg("AddrManager::Load BEGIN\n");
	
	OpenLibHandle(&s_hServer, g_Ext.GetLibPtr_Server(), "server");
	OpenLibHandle(&s_hEngine, g_Ext.GetLibPtr_Engine(), "engine");
	
	for (auto& pair : AutoNameMap<IAddr>::Map()) {
		IAddr *addr = pair.second;
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
	const auto& addrs = AutoNameMap<IAddr>::Map();
	
	auto it = addrs.find(std::string(name));
	if (it == addrs.end()) {
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


void *AddrManager::FindSymbol(const char *lib, const char *sym)
{
	void *handle = nullptr;
	
	if (stricmp(lib, "server") == 0) {
		handle = s_hServer;
	} else if (stricmp(lib, "engine") == 0) {
		handle = s_hEngine;
	} else {
		DevMsg("AddrManager::FindSymbol: unknown lib \"%s\" for symbol \"%s\"\n", lib, sym);
		return nullptr;
	}
	
	if (handle == nullptr) {
		DevMsg("AddrManager::FindSymbol: handle for lib \"%s\" is nullptr\n", lib);
		return nullptr;
	}
	
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
