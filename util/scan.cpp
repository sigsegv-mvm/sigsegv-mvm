#include "util/scan.h"


struct DynLibInfo
{
	void *baseAddress;
	size_t memorySize;
};

class MemoryUtils : public IMemoryUtils
{
public:
	bool GetLibraryInfo(const void *libPtr, DynLibInfo& lib);
};


/* a better version of SourceMod's MemoryUtils::FindPattern
 * (1) we use memcmp like a sane person would
 * (2) no 0x2A wildcard bullshit
 * (3) can find multiple pattern matches
 */
bool MemFindPattern(const void *libPtr, const char *pattern, size_t len, std::vector<void *>& matches)
{
	DynLibInfo lib;
	memset(&lib, 0, sizeof(DynLibInfo));
	
	MemoryUtils *mu = dynamic_cast<MemoryUtils *>(memutils);
	assert(mu != nullptr);
	
	if (!mu->GetLibraryInfo(libPtr, lib)) {
		return false;
	}
	
	char *ptr = reinterpret_cast<char *>(lib.baseAddress);
	char *end = ptr + lib.memorySize - len;
	
	while (ptr < end) {
		if (memcmp(ptr, pattern, len) == 0) {
			matches.push_back(reinterpret_cast<void *>(ptr));
		}
		
		++ptr;
	}
	
	return true;
}


// TODO: REMOVE ME!
bool MemoryUtils::GetLibraryInfo(const void *libPtr, DynLibInfo& lib) { assert(false); return false; }
