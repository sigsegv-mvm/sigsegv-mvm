#ifndef _INCLUDE_SIGSEGV_MEM_PROTECT_H_
#define _INCLUDE_SIGSEGV_MEM_PROTECT_H_


#include "util/misc.h"


class MemProtModifier
{
public:
	enum Flags : unsigned int
	{
		MPROT_NONE = 0b000,
		
		MPROT_R = 0b001,
		MPROT_W = 0b010,
		MPROT_X = 0b100,
		
		MPROT_RW = (MPROT_R | MPROT_W),
		MPROT_RX = (MPROT_R | MPROT_X),
		MPROT_WX = (MPROT_W | MPROT_X),
		
		MPROT_RWX = (MPROT_R | MPROT_W | MPROT_X),
	};
	
	MemProtModifier(const void *addr, size_t len, Flags f_old, Flags f_new) :
		m_pAddr(addr), m_nLen(len), m_nOldFlags(f_old), m_nNewFlags(f_new)
	{
		this->ApplyFlags(this->m_nNewFlags);
	}
	~MemProtModifier()
	{
		this->ApplyFlags(this->m_nOldFlags);
	}
	
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
	
	const void *m_pAddr;
	size_t m_nLen;
	Flags m_nOldFlags;
	Flags m_nNewFlags;
};


#if defined _WINDOWS

inline DWORD MemProtModifier::TranslateFlags(MemProtModifier::Flags flags) const
{
	assert((flags & ~MPROT_RWX) == 0);
	
	switch (flags) {
	case MPROT_NONE: return PAGE_NOACCESS;
	case MPROT_R:    return PAGE_READONLY;
	case MPROT_W:    return PAGE_READWRITE;         // closest approximation
	case MPROT_X:    return PAGE_EXECUTE;
	case MPROT_RW:   return PAGE_READWRITE;
	case MPROT_RX:   return PAGE_EXECUTE_READ;
	case MPROT_WX:   return PAGE_EXECUTE_READWRITE; // closest approximation
	case MPROT_RWX:  return PAGE_EXECUTE_READWRITE;
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
	assert((flags & ~MPROT_RWX) == 0);
	
	int prot = PROT_NONE;
	if ((flags & MPROT_R) != 0) prot |= PROT_READ;
	if ((flags & MPROT_W) != 0) prot |= PROT_WRITE;
	if ((flags & MPROT_X) != 0) prot |= PROT_EXEC;
	return prot;
}

inline void MemProtModifier::ApplyFlags(MemProtModifier::Flags flags) const
{
	int new_prot = this->TranslateFlags(flags);
	
	uintptr_t addr_begin = (uintptr_t)this->m_pAddr;
	uintptr_t addr_end   = (uintptr_t)this->m_pAddr + this->m_nLen;
	
	addr_begin = RoundDownToPowerOfTwo(addr_begin, GetPageSize());
	addr_end   = RoundUpToPowerOfTwo  (addr_end,   GetPageSize());
	
	assert(mprotect((void *)addr_begin, (addr_end - addr_begin), new_prot) == 0);
}

#endif


template<MemProtModifier::Flags F_OLD, MemProtModifier::Flags F_NEW>
class MemProtModifierTmpl : public MemProtModifier
{
public:
	MemProtModifierTmpl(const void *addr, size_t len) :
		MemProtModifier(addr, len, F_OLD, F_NEW) {}
};


/* convenience aliases */
using MemProtModifier_RO_RW  = MemProtModifierTmpl<MemProtModifier::MPROT_R,  MemProtModifier::MPROT_RW>;
using MemProtModifier_RO_RX  = MemProtModifierTmpl<MemProtModifier::MPROT_R,  MemProtModifier::MPROT_RX>;
using MemProtModifier_RO_RWX = MemProtModifierTmpl<MemProtModifier::MPROT_R,  MemProtModifier::MPROT_RWX>;
using MemProtModifier_RW_RWX = MemProtModifierTmpl<MemProtModifier::MPROT_RW, MemProtModifier::MPROT_RWX>;
using MemProtModifier_RX_RWX = MemProtModifierTmpl<MemProtModifier::MPROT_RX, MemProtModifier::MPROT_RWX>;


/* automatic scope block thingies that won't be destructed before the scope ends */
#define MemProtModifier_RO_RW( ADDR, LEN) const auto& _mpm__##__COUNTER__ = MemProtModifier_RO_RW( ADDR, LEN)
#define MemProtModifier_RO_RX( ADDR, LEN) const auto& _mpm__##__COUNTER__ = MemProtModifier_RO_RX( ADDR, LEN)
#define MemProtModifier_RO_RWX(ADDR, LEN) const auto& _mpm__##__COUNTER__ = MemProtModifier_RO_RWX(ADDR, LEN)
#define MemProtModifier_RW_RWX(ADDR, LEN) const auto& _mpm__##__COUNTER__ = MemProtModifier_RW_RWX(ADDR, LEN)
#define MemProtModifier_RX_RWX(ADDR, LEN) const auto& _mpm__##__COUNTER__ = MemProtModifier_RX_RWX(ADDR, LEN)


#endif
