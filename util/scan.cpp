#include "util/scan.h"


namespace Scan
{
	bool CheckMasked(const void *ptr, const ByteBuf& pattern, const ByteBuf& mask)
	{
		assert(pattern.GetSize() == mask.GetSize());
		int len = pattern.GetSize();
		
		for (int i = 0; i < len; ++i) {
			uint8_t b_mem  = *((uint8_t *)ptr + i);
			uint8_t b_pat  = pattern[i];
			uint8_t b_mask = mask[i];
			
			if ((b_mem & b_mask) != (b_pat & b_mask)) {
				return false;
			}
		}
		
		return true;
	}
	
	void FindMasked(Library lib, const ByteBuf& pattern, const ByteBuf& mask, int align, std::vector<void *>& matches)
	{
		assert(pattern.GetSize() == mask.GetSize());
		int len = pattern.GetSize();
		
		DynLibInfo info;
		memset(&info, 0x00, sizeof(info));
		assert(g_MemUtils.GetLibraryInfo(LibMgr::GetPtr(lib), info));
		
		matches.clear();
		
		const uint8_t *ptr = (const uint8_t *)info.baseAddress;
		const uint8_t *end = (const uint8_t *)((uintptr_t)info.baseAddress + info.memorySize - len);
		
		assert((uintptr_t)ptr % align == 0);
		
		while (ptr < end) {
			if (CheckMasked(ptr, pattern, mask)) {
				matches.push_back((void *)ptr);
			}
			
			ptr += align;
		}
	}
	
	
	void FindFast(Library lib, const void *pattern, int len, int align, std::vector<void *>& matches)
	{
		DynLibInfo info;
		memset(&info, 0x00, sizeof(info));
		assert(g_MemUtils.GetLibraryInfo(LibMgr::GetPtr(lib), info));
		
		matches.clear();
		
		const uint8_t *ptr = (const uint8_t *)info.baseAddress;
		const uint8_t *end = (const uint8_t *)((uintptr_t)info.baseAddress + info.memorySize - len);
		
		assert((uintptr_t)ptr % align == 0);
		
		while (ptr < end) {
			if (memcmp(ptr, pattern, len) == 0) {
				matches.push_back((void *)ptr);
			}
			
			ptr += align;
		}
	}
	
	void FindString(Library lib, const char *str, std::vector<void *>& matches)
	{
		int len = strlen(str) + 1;
		
		DynLibInfo info;
		memset(&info, 0x00, sizeof(info));
		assert(g_MemUtils.GetLibraryInfo(LibMgr::GetPtr(lib), info));
		
		matches.clear();
		
		const char *ptr = (const char *)info.baseAddress;
		const char *end = (const char *)((uintptr_t)info.baseAddress + info.memorySize - len);
		
		while (ptr < end) {
			if (strcmp(ptr, str) == 0) {
				matches.push_back((void *)ptr);
			}
			
			++ptr;
		}
	}
}
