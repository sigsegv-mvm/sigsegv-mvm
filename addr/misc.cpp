#include "addr/addr.h"


class CAddr_Sym : public IAddr
{
public:
	virtual bool FindAddrLinux(uintptr_t& addr) const override final
	{
		void *sym_addr = AddrManager::FindSymbol(this->GetLibrary(), this->GetSymbol());
		if (sym_addr == nullptr) {
			return false;
		}
		
		addr = (uintptr_t)sym_addr;
		return true;
	}
	
protected:
	virtual const char *GetSymbol() const = 0;
};


#define _ADDR_SYM(name, namestr, sym) \
	class CAddr_Base__##name : public CAddr_Sym \
	{ \
	public:
		virtual const char *GetName() const override final { return namestr; } \
	protected: \
		virtual const char *GetSymbol() const override final { return sym; } \
	}; \
	struct CAddr__##name; \
	static CAddr__##name addr_##name; \
	struct CAddr__##name final : public CAddr_Base__##name
	// syntax highlighting is broken {};

#define ADDR_SYM_GLOBAL(name, sym) _ADDR_SYM(name, #name, sym)
#define ADDR_SYM_MEMBER(obj, member, sym) _ADDR_SYM(obj##_##member, #obj "::" #member, sym)


ADDR_SYM_MEMBER(CTFPlayer, CanBeForcedToLaugh, "_ZN9CTFPlayer18CanBeForcedToLaughEv")
{
	virtual bool FindAddrWin(uintptr_t& addr) const override
	{
		// TODO
		return false;
	}
}
