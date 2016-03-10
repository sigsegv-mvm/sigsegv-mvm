#ifndef _INCLUDE_SIGSEGV_MEM_PROTECT_H_
#define _INCLUDE_SIGSEGV_MEM_PROTECT_H_


#if defined _WINDOWS

inline void MemProtect(void *addr, size_t len, bool protect)
{
	DWORD old;
	DWORD prot = (protect ? PAGE_EXECUTE_READ : PAGE_EXECUTE_READWRITE);
	VirtualProtect(addr, len, prot, &old);
}

#else

inline long GetPageSize()
{
	long page_size = sysconf(_SC_PAGESIZE);
	assert(page_size != -1);
	return page_size;
}

inline void MemProtect(void *addr, size_t len, bool protect)
{
	static long page_size = GetPageSize();
	
	long adj = (long)addr & (page_size - 1);
	
	addr = (void *)((uintptr_t)addr - adj);
	len += adj;
	
	int prot = PROT_READ | PROT_EXEC | (protect ? 0 : PROT_WRITE);
	mprotect(addr, len, prot);
}

#endif


#endif
