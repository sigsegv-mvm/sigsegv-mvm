#include "util/scan.h"


/* a better version of SourceMod's MemoryUtils::FindPattern
 * (1) we use memcmp like a sane person would
 * (2) no 0x2A wildcard bullshit
 * (3) can find multiple pattern matches
 */
bool MemFindPattern(const char *lib, const char *pattern, size_t len, std::vector<void *>& matches)
{
	void *lib_ptr = nullptr;
	if (stricmp(lib, "server") == 0) {
		lib_ptr = g_Ext.GetLibPtr_Server();
	} else if (stricmp(lib, "engine") == 0) {
		lib_ptr = g_Ext.GetLibPtr_Engine();
	}
	assert(lib_ptr != nullptr);
	
	DynLibInfo info;
	memset(&info, 0, sizeof(DynLibInfo));
	
	assert(g_MemUtils.GetLibraryInfo(lib_ptr, info));
	
	char *ptr = reinterpret_cast<char *>(info.baseAddress);
	char *end = ptr + info.memorySize - len;
	
	while (ptr < end) {
		if (memcmp(ptr, pattern, len) == 0) {
			matches.push_back(reinterpret_cast<void *>(ptr));
		}
		
		++ptr;
	}
	
	return true;
}
