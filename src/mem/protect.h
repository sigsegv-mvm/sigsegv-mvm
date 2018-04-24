#ifndef _INCLUDE_SIGSEGV_MEM_PROTECT_H_
#define _INCLUDE_SIGSEGV_MEM_PROTECT_H_


template<MemProtModifier::Flags F_OLD, MemProtModifier::Flags F_NEW>
class MemProtModifier
{
public:
	MemProtModifier(const void *addr, size_t len) :
		m_pAddr(addr), m_nLen(len)
	{
		this->ApplyNewFlags();
	}
	~MemProtModifier()
	{
		this->ApplyOldFlags();
	}
	
	enum Flags : unsigned int
	{
		PROT_NONE = 0b000,
		
		PROT_R = 0b001,
		PROT_W = 0b010,
		PROT_X = 0b100,
		
		PROT_RW = (PROT_R | PROT_W),
		PROT_RX = (PROT_R | PROT_X),
		PROT_WX = (PROT_W | PROT_X),
		
		PROT_RWX = (PROT_R | PROT_W | PROT_X),
	};
	
private:
#if !defined _WINDOWS
	static size_t GetPageSize();
#endif
	
#if defined _WINDOWS
	DWORD TranslateFlags(Flags flags) const;
#else
	int TranslateFlags(Flags flags) const;
#endif
	
	void ApplyFlags(Flags flags) const;
	
	void ApplyOldFlags() const { this->ApplyFlags(F_OLD); }
	void ApplyNewFlags() const { this->ApplyFlags(F_NEW); }
	
	const void *m_pAddr;
	size_t m_nLen;
};


#if defined _WINDOWS

inline DWORD MemProtModifier::TranslateFlags(MemProtModifier::Flags flags) const
{
	using Flags = MemProtModifier::Flags;
	
	assert((flags & ~Flags::RWX) == 0);
	
	switch (flags) {
	case Flags::PROT_NONE: return PAGE_NOACCESS;
	case Flags::PROT_R:    return PAGE_READONLY;
	case Flags::PROT_W:    return PAGE_READWRITE;         // closest approximation
	case Flags::PROT_X:    return PAGE_EXECUTE;
	case Flags::PROT_RW:   return PAGE_READWRITE;
	case Flags::PROT_RX:   return PAGE_EXECUTE_READ;
	case Flags::PROT_WX:   return PAGE_EXECUTE_READWRITE; // closest approximation
	case Flags::PROT_RWX:  return PAGE_EXECUTE_READWRITE;
	}
}

inline void MemProtModifier::ApplyFlags(MemProtModifier::Flags flags) const
{
	DWORD old_prot;
	DWORD new_prot = this->TranslateFlags(flags);
	
	assert(VirtualProtect((LPVOID)this->m_pAddr, this->m_nLen, new_prot, &old_prot) == 0);
}

#else

inline size_t MemProtModifier::GetPageSize()
{
	static size_t page_size = []{
		long sc_pagesize = sysconf(_SC_PAGESIZE);
		assert(sc_pagesize != -1L);
		return sc_pagesize;
	}();
	return page_size;
}

inline int MemProtModifier::TranslateFlags(MemProtModifier::Flags flags) const
{
	using Flags = MemProtModifier::Flags;
	
	assert((flags & ~Flags::RWX) == 0);
	
	int prot = PROT_NONE;
	if ((flags & Flags::R) != 0) prot |= PROT_READ;
	if ((flags & Flags::W) != 0) prot |= PROT_WRITE;
	if ((flags & Flags::X) != 0) prot |= PROT_EXEC;
	return prot;
}

inline void MemProtModifier::ApplyFlags(MemProtModifier::Flags flags) const
{
	int new_prot = this->TranslateFlags(flags);
	
	uintptr_t addr_begin = (uintptr_t)this->m_pAddr;
	uintptr_t addr_end   = (uintptr_t)this->m_pAddr + this->m_nLen;
	
	RoundDownToPowerOfTwo(addr_begin, GetPageSize());
	RoundUpToPowerOfTwo  (addr_end,   GetPageSize());
	
	assert(mprotect((void *)addr_begin, (addr_end - addr_begin), new_prot) == 0);
}

#endif


/* convenience aliases */
using MemProtModifier_RO_RW  = MemProtModifier<MemProtModifier::R,  MemProtModifier::RW>;
using MemProtModifier_RO_RX  = MemProtModifier<MemProtModifier::R,  MemProtModifier::RX>;
using MemProtModifier_RO_RWX = MemProtModifier<MemProtModifier::R,  MemProtModifier::RWX>;
using MemProtModifier_RW_RWX = MemProtModifier<MemProtModifier::RW, MemProtModifier::RWX>;
using MemProtModifier_RX_RWX = MemProtModifier<MemProtModifier::RX, MemProtModifier::RWX>;


#endif
