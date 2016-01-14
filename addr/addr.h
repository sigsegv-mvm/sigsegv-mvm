#ifndef _INCLUDE_SIGSEGV_ADDR_ADDR_H_
#define _INCLUDE_SIGSEGV_ADDR_ADDR_H_


#include "common.h"
#include "library.h"
#include "util/autolist.h"


class AddrManager
{
public:
	static void Load();
	static void UnLoad();
	
	static void *GetAddr(const char *name);
	
	static void *FindSymbol(Library lib, const char *sym);
	
private:
	AddrManager() {}
	
	static void OpenLibHandle(void **handle, void *ptr, const char *name);
	static void CloseLibHandle(void **handle);
	
	static void *s_hServer;
	static void *s_hEngine;
};


class IAddr : public AutoNameMap<IAddr>
{
public:
	enum class State : int
	{
		INITIAL,
		OK,
		FAIL,
	};
	
	virtual ~IAddr() {}
	
	void Init();
	
	virtual const char *GetName() const = 0;
	virtual Library GetLibrary() const                { return Library::SERVER; }
	virtual bool FindAddrLinux(uintptr_t& addr) const { return false; }
	virtual bool FindAddrOSX(uintptr_t& addr) const   { return this->FindAddrLinux(addr); }
	virtual bool FindAddrWin(uintptr_t& addr) const   { return false; }
	
	State GetState() const { return this->m_State; }
	void *GetAddr() const  { return (void *)this->m_iAddr; }
	
protected:
	IAddr() {}
	
private:
	State m_State = State::INITIAL;
	uintptr_t m_iAddr = 0x00000000;
};


class CAddr_Sym : public IAddr
{
public:
	virtual bool FindAddrLinux(uintptr_t& addr) const override
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
	public: \
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


#endif
