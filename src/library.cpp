#include "library.h"


std::map<Library, void *> LibMgr::s_LibPtrs;
std::map<Library, LibInfo> LibMgr::s_LibInfos;
std::map<Library, void *> LibMgr::s_LibHandles;


static std::map<Library, const char *> libnames{
	{ Library::INVALID,        "invalid"        },
	{ Library::SERVER,         "server"         },
	{ Library::ENGINE,         "engine"         },
	{ Library::DEDICATED,      "dedicated"      },
	{ Library::TIER0,          "tier0"          },
	{ Library::CLIENT,         "client"         },
	{ Library::VGUIMATSURFACE, "vguimatsurface" },
};


void LibMgr::Load()
{
	for (const auto& pair : libnames) {
		Library lib = pair.first;
		if (lib == Library::INVALID) continue;
		
		void *handle = OpenLibHandle(lib);
		if (handle != nullptr) {
			s_LibHandles[lib] = handle;
			
			/* pre-populate the library info structs */
			(void)GetInfo(lib);
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
	
#if defined _LINUX
	g_MemUtils.ForEachSection(s_LibHandles[lib], [&](const Elf32_Shdr *shdr, const char *name){
		SegInfo seg;
		seg.off = shdr->sh_addr;
		seg.len = shdr->sh_size;
		
		info.segs[name] = seg;
	});
#elif defined _WINDOWS
	g_MemUtils.ForEachSection(s_LibHandles[lib], [&](const IMAGE_SECTION_HEADER *pSectHdr){
		SegInfo seg;
		seg.off = pSectHdr->VirtualAddress;
		seg.len = pSectHdr->Misc.VirtualSize;
		
		auto name = (const char *)pSectHdr->Name;
		info.segs[name] = seg;
	});
#endif
	
	s_LibInfos[lib] = info;
	
	DevMsg("Library %-34s [ %08x %08x ]\n", libnames.at(lib), info.baseaddr, info.baseaddr + info.len);
	for (const auto& pair : info.segs) {
		DevMsg("  %-40s [ %08x %08x ]\n",
			pair.first.c_str(),
			info.baseaddr + pair.second.off,
			info.baseaddr + pair.second.off + pair.second.len);
	}
}


void *LibMgr::FindSym(Library lib, const char *sym)
{
	if (s_LibHandles.count(lib) == 0) return nullptr;
	
	void *handle = s_LibHandles.at(lib);
	if (handle == nullptr) return nullptr;
	
	return g_MemUtils.ResolveSymbol(handle, sym);
}

std::tuple<bool, std::string, void *> LibMgr::FindSymRegex(Library lib, const char *pattern, std::regex::flag_type flags)
{
	std::regex filter(pattern);
	
	std::vector<Symbol *> matches;
	
	LibMgr::ForEachSym(lib, [&](Symbol *sym){
		std::string name(sym->buffer(), sym->length);
		
		if (std::regex_search(name, filter, std::regex_constants::match_any)) {
			matches.push_back(sym);
		}
	});
	
	if (matches.size() == 1) {
		std::string name(matches[0]->buffer(), matches[0]->length);
		return std::make_tuple(true, name, matches[0]->address);
	} else {
		return std::make_tuple(false, nullptr, nullptr);
	}
}

void LibMgr::ForEachSym(Library lib, const std::function<void(Symbol *)>& functor)
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
		if (stricmp(pair.second, str) == 0) {
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

size_t LibMgr::MaxStringLen()
{
	size_t max = 0;
	
	for (const auto& pair : libnames) {
		max = Max(max, strlen(pair.second));
	}
	
	return max;
}
