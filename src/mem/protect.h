#ifndef _INCLUDE_SIGSEGV_MEM_PROTECT_H_
#define _INCLUDE_SIGSEGV_MEM_PROTECT_H_


// TODO: deal with edge cases where the requested address range consists of
// multiple pages which do not all share the exact same flags

// TODO: get rid of asserts as much as possible

// TODO: figure out a way to query the page flags on Linux/OSX
// (for Linux, libprocps looks like a somewhat reasonable option)

// another option would be to check which executable segment the address range
// is in and to then use that as a heuristic:
//  .text is normally RX
//  .data is normally RW
// etc.


class MemUnprotector
{
public:
	MemUnprotector(const void *addr, size_t len) :
		m_pAddr(addr), m_nLen(len)
	{
		this->m_nOldFlags = this->GetFlags();
		
		if (!this->IsWritable()) {
			this->MakeWritable();
			this->m_bShouldReprotect = true;
		}
	}
	~MemUnprotector()
	{
		if (this->m_bShouldReprotect) {
			this->RestoreFlags();
		}
	}
	
private:
#if defined _WINDOWS
	typedef DWORD pageflags_t;
#else
	typedef long pageflags_t;
#endif
	
	pageflags_t GetFlags();
	void SetFlags(pageflags_t flags);
	
	bool IsWritable();
	void MakeWritable();
	void RestoreFlags();
	
	const void *m_pAddr;
	size_t m_nLen;
	
	bool m_bShouldReprotect = false;
	pageflags_t m_nOldFlags;
};


#if defined _WINDOWS

inline MemUnprotector::pageflags_t MemUnprotector::GetFlags()
{
	MEMORY_BASIC_INFORMATION info;
	assert(VirtualQuery(this->m_pAddr, &info, sizeof(info)) != 0);
	return info.Protect;
}

inline void MemUnprotector::SetFlags(pageflags_t flags)
{
	pageflags_t old_flags;
	assert(VirtualProtect((LPVOID)this->m_pAddr, this->m_nLen, flags, &old_flags) != 0);
}


inline bool MemUnprotector::IsWritable()
{
	if ((this->m_nOldFlags & PAGE_READWRITE) != 0)         return true;
	if ((this->m_nOldFlags & PAGE_WRITECOPY) != 0)         return true;
	if ((this->m_nOldFlags & PAGE_EXECUTE_READWRITE) != 0) return true;
	if ((this->m_nOldFlags & PAGE_EXECUTE_WRITECOPY) != 0) return true;
	
	return false;
}

inline void MemUnprotector::MakeWritable()
{
	if ((this->m_nOldFlags & PAGE_READONLY) != 0) {
		this->SetFlags(PAGE_READWRITE);
	} else if ((this->m_nOldFlags & (PAGE_EXECUTE | PAGE_EXECUTE_READ)) != 0) {
		this->SetFlags(PAGE_EXECUTE_READWRITE);
	}
}

inline void MemUnprotector::RestoreFlags()
{
	this->SetFlags(this->m_nOldFlags);
}

#else

inline long GetPageSize()
{
	long page_size = sysconf(_SC_PAGESIZE);
	assert(page_size != -1L);
	return page_size;
}

/* Posix annoyingly lacks an easy way to query page flags, so we'll just have to
 * change everything to RWX and leave it that way permanently... */

inline MemUnprotector::pageflags_t MemUnprotector::GetFlags()
{
	return 0L;
}

inline void MemUnprotector::SetFlags(pageflags_t flags)
{
}


inline bool MemUnprotector::IsWritable()
{
	return false;
}

inline void MemUnprotector::MakeWritable()
{
	static long page_size = GetPageSize();
	
	long adj = (long)this->m_pAddr & (page_size - 1L);
	
	void *addr = (void *)((uintptr_t)this->m_pAddr - adj);
	size_t len = this->m_nLen + adj;
	
	assert(mprotect(addr, len, PROT_READ | PROT_WRITE | PROT_EXEC) == 0);
}

inline void MemUnprotector::RestoreFlags()
{
}

#endif



#if 0

#if defined _WINDOWS

#define MEM_WRITABLE(addr, len) \
	...
#define MEM_REVERT(addr, len) \
	...


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


#endif
