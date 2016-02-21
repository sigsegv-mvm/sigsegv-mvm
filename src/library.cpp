#include "library.h"


std::map<Library, void *> LibMgr::s_LibPtrs;
std::map<Library, LibInfo> LibMgr::s_LibInfos;
std::map<Library, void *> LibMgr::s_LibHandles;


static std::map<Library, const char *> libnames{
	{ Library::INVALID, "invalid" },
	{ Library::SERVER,  "server" },
	{ Library::ENGINE,  "engine" },
	{ Library::TIER0,   "tier0" },
	{ Library::CLIENT,  "client" },
};


void LibMgr::Load()
{
	for (const auto& pair : libnames) {
		Library lib = pair.first;
		if (lib == Library::INVALID) continue;
		
		void *handle = OpenLibHandle(lib);
		if (handle != nullptr) {
			s_LibHandles[lib] = handle;
		}
	}
}

void LibMgr::Unload()
{
	for (const auto& pair : s_LibHandles) {
		CloseLibHandle(pair.second);
	}
	s_LibHandles.clear();
}


void LibMgr::SetPtr(Library lib, void *ptr)
{
	s_LibPtrs[lib] = ptr;
}

void *LibMgr::GetPtr(Library lib)
{
	return s_LibPtrs.at(lib);
}


const LibInfo& LibMgr::GetInfo(Library lib)
{
	if (s_LibInfos.find(lib) == s_LibInfos.end()) {
		FindInfo(lib);
	}
	
	return s_LibInfos.at(lib);
}


void LibMgr::FindInfo(Library lib)
{
	DynLibInfo dlinfo;
	memset(&dlinfo, 0x00, sizeof(dlinfo));
	assert(g_MemUtils.GetLibraryInfo(GetPtr(lib), dlinfo));
	
	LibInfo info;
	info.baseaddr = (uintptr_t)dlinfo.baseAddress;
	info.len      = (uintptr_t)dlinfo.memorySize;
	
#if defined _WINDOWS
	
	IMAGE_NT_HEADERS *pNtHdr = ImageNtHeader((void *)info.baseaddr);
	IMAGE_SECTION_HEADER *pSectHdr = (IMAGE_SECTION_HEADER *)(pNtHdr + 1);
	
	int num_sect = pNtHdr->FileHeader.NumberOfSections;
	for (int i = 0; i < num_sect; ++i) {
		SegInfo seg;
		seg.off = pSectHdr->VirtualAddress;
		seg.len = pSectHdr->Misc.VirtualSize;
		
		const char *name = (const char *)pSectHdr->Name;
		info.segs[name] = seg;
		
		++pSectHdr;
	}
	
#endif
	
	s_LibInfos[lib] = info;
	
	DevMsg("Library \"%s\": [%08x ~ %08x]\n", libnames.at(lib), info.baseaddr, info.baseaddr + info.len);
	for (const auto& pair : info.segs) {
		DevMsg("  Segment \"%s\": [%08x ~ %08x]\n",
			pair.first.c_str(),
			info.baseaddr + pair.second.off,
			info.baseaddr + pair.second.off + pair.second.len);
	}
}


void *LibMgr::FindSym(Library lib, const char *sym)
{
	void *handle = s_LibHandles.at(lib);
	assert(handle != nullptr);
	return g_MemUtils.ResolveSymbol(handle, sym);
}

void LibMgr::ForEachSym(Library lib, void (*functor)(Symbol *))
{
	void *handle = s_LibHandles.at(lib);
	assert(handle != nullptr);
	g_MemUtils.ForEachSymbol(handle, functor);
}


#if defined _LINUX || defined _OSX

void *LibMgr::OpenLibHandle(Library lib)
{
	void *ptr = GetPtr(lib);
	
	Dl_info info;
	if (dladdr(ptr, &info) == 0) {
		DevMsg("LibMgr::OpenLibHandle: can't find library \"%s\" in memory\n"
			"dladdr error:\n%s\n", libnames.at(lib), dlerror());
		return nullptr;
	}
	
	void *handle = dlopen(info.dli_fname, RTLD_NOW);
	if (handle == nullptr) {
		DevMsg("LibMgr::OpenLibHandle: can't load library \"%s\"\n"
			"dlopen error:\n%s\n", libnames.at(lib), dlerror());
	}
	return handle;
}

void LibMgr::CloseLibHandle(void *handle)
{
	if (handle != nullptr) {
		dlclose(handle);
	}
}

#elif defined _WINDOWS

void *LibMgr::OpenLibHandle(Library lib)
{
	void *ptr = GetPtr(lib);
	
	MEMORY_BASIC_INFORMATION mem;
	if (VirtualQuery(ptr, &mem, sizeof(mem)) == 0) {
		char err[4096];
		libsys->GetPlatformError(err, sizeof(err));
		
		DevMsg("LibMgr::OpenLibHandle: can't find library \"%s\" in memory\n"
			"VirtualQuery error:\n%s\n", libnames.at(lib), err);
		return nullptr;
	}
	
	return mem.AllocationBase;
}

void LibMgr::CloseLibHandle(void *handle)
{
}

#endif


Library LibMgr::WhichLibAtAddr(void *ptr)
{
	auto addr = (uintptr_t)ptr;
	
	for (const auto& pair : s_LibHandles) {
		Library lib = pair.first;
		if (lib == Library::INVALID) continue;
		
		const LibInfo& info = GetInfo(lib);
		
		if (addr >= info.baseaddr && addr < info.baseaddr + info.len) {
			return lib;
		}
	}
	
	return Library::INVALID;
}


Library LibMgr::FromString(const char *str)
{
	for (const auto& pair : libnames) {
		if (strcmp(pair.second, str) == 0) {
			return pair.first;
		}
	}
	
	return Library::INVALID;
}

const char *LibMgr::ToString(Library lib)
{
	for (const auto& pair : libnames) {
		if (pair.first == lib) {
			return pair.second;
		}
	}
	
	return libnames.at(Library::INVALID);
}
