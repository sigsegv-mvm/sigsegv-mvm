#ifndef _INCLUDE_SIGSEGV_ADDR_ADDR_H_
#define _INCLUDE_SIGSEGV_ADDR_ADDR_H_


#include "common.h"
#include "library.h"
#include "util/util.h"
#include "abi.h"


class IAddr;

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
	
	static std::map<std::string, IAddr *> s_Addrs;
	
	static void *s_hServer;
	static void *s_hEngine;
};


class IAddr : public AutoList<IAddr>
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
	virtual bool ShouldInitFirst() const              { return false; }
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


class IAddr_Sym : public IAddr
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

class CAddr_Sym : public IAddr_Sym
{
public:
	CAddr_Sym(const char *name, const char *sym) :
		m_pszName(name), m_pszSymbol(sym) {}
	
	virtual const char *GetName() const override       { return this->m_pszName; }
	virtual const char *GetSymbol() const override     { return this->m_pszSymbol; }
	
private:
	const char *m_pszName;
	const char *m_pszSymbol;
};


#define _ADDR_SYM(name, namestr, sym) \
	class Addr_Base__##name : public IAddr_Sym \
	{ \
	public: \
		virtual const char *GetName() const override final { return namestr; } \
	protected: \
		virtual const char *GetSymbol() const override final { return sym; } \
	}; \
	struct Addr__##name; \
	static Addr__##name addr_##name; \
	struct Addr__##name final : public CAddr_Base__##name
	// syntax highlighting is broken {};

#define ADDR_SYM_GLOBAL(name, sym) _ADDR_SYM(name, #name, sym)
#define ADDR_SYM_MEMBER(obj, member, sym) _ADDR_SYM(obj##_##member, #obj "::" #member, sym)


class IAddr_VTable : public IAddr_Sym
{
public:
	virtual bool ShouldInitFirst() const override { return true; }
	
	virtual bool FindAddrLinux(uintptr_t& addr) const override
	{
		bool result = IAddr_Sym::FindAddrLinux(addr);
		
		if (result) {
			addr += offsetof(vtable, vfptrs);
		}
		
		return result;
	}
	
	virtual bool FindAddrWin(uintptr_t& addr) const override
	{
		/* STEP 1: get ptr to _TypeDescriptor by finding typeinfo name string */
		
		const char *str = this->GetWinRTTIStr();
		
		CStringScan scan1(ScanDir::FORWARD, ScanResults::ALL, CLibBounds(this->GetLibrary()), 1, str);
		if (scan1.Matches().size() == 0) {
			DevMsg("CAddr_VTable::FindAddrWin: could not find RTTI string \"%s\"\n", str);
			return false;
		}
		if (scan1.Matches().size() > 1) {
			DevMsg("CAddr_VTable::FindAddrWin: found multiple string matches for \"%s\"\n", str);
		}
		
		auto *p_TD = (_TypeDescriptor *)((uintptr_t)scan1.Matches()[0] - offsetof(_TypeDescriptor, name));
		
		
		/* STEP 2: get ptr to __RTTI_CompleteObjectLocator by finding references to the _TypeDescriptor */
		
		__RTTI_CompleteObjectLocator seek_COL = {
			0x00000000,
			0x00000000,
			0x00000000,
			p_TD,
		};
		
		CBasicScan scan2(ScanDir::FORWARD, ScanResults::ALL, CLibBounds(this->GetLibrary()), 4, (const void *)&seek_COL, 0x10);
		if (scan2.Matches().size() == 0) {
			DevMsg("CAddr_VTable::FindAddrWin: could not find _TypeDescriptor refs for \"%s\"\n", this->GetName());
			return false;
		}
		if (scan2.Matches().size() > 1) {
			DevMsg("CAddr_VTable::FindAddrWin: found multiple _TypeDescriptor refs for \"%s\"\n", this->GetName());
		}
		
		auto *p_COL = (__RTTI_CompleteObjectLocator *)scan2.Matches()[0];
		
		
		/* STEP 3: get ptr to the vtable itself by finding references to the __RTTI_CompleteObjectLocator */
		
		CBasicScan scan3(ScanDir::FORWARD, ScanResults::ALL, CLibBounds(this->GetLibrary()), 4, (const void *)&p_COL, 0x4);
		if (scan3.Matches().size() == 0) {
			DevMsg("CAddr_VTable::FindAddrWin: could not find __RTTI_CompleteObjectLocator refs for \"%s\"\n", this->GetName());
			return false;
		}
		if (scan3.Matches().size() > 1) {
			DevMsg("CAddr_VTable::FindAddrWin: found multiple __RTTI_CompleteObjectLocator refs for \"%s\"\n", this->GetName());
		}
		
		auto p_VT = (void **)((uintptr_t)scan3.Matches()[0] + 0x4);
		
		
		addr = (uintptr_t)p_VT;
		return true;
	}
	
protected:
	virtual const char *GetWinRTTIStr() const = 0;
};

class CAddr_VTable : public IAddr_VTable
{
public:
	CAddr_VTable(const char *name, const char *sym, const char *rtti) :
		m_pszName(name), m_pszSymbol(sym), m_pszWinRTTIStr(rtti) {}
	
	virtual const char *GetName() const override       { return this->m_pszName; }
	virtual const char *GetSymbol() const override     { return this->m_pszSymbol; }
	virtual const char *GetWinRTTIStr() const override { return this->m_pszWinRTTIStr; }
	
private:
	const char *m_pszName;
	const char *m_pszSymbol;
	const char *m_pszWinRTTIStr;
};


/* address finder for functions with these traits:
 * 1. func body contains a unique string reference
 * 2. func body starts with "push ebp; mov ebp,esp"
 * 3. func is virtual and has a confidently known vtable offset
 */
class IAddr_Func_UniqueStr_EBPBacktrack_KnownVTOff : public IAddr_Sym
{
public:
	virtual bool FindAddrWin(uintptr_t& addr) const override
	{
		CStringScan scan1(ScanDir::FORWARD, ScanResults::ALL, CLibBounds(this->GetLibrary()), 1, this->GetUniqueStr());
		if (scan1.Matches().size() != 1) { Fail(1); return false; }
		auto p_str = (const char *)scan1.Matches()[0];
		
		CBasicScan scan2(ScanDir::FORWARD, ScanResults::ALL, CLibBounds(this->GetLibrary()), 1, (const void *)&p_str, 0x4);
		if (scan2.Matches().size() != 1) { Fail(2); return false; }
		auto p_in_func = (const char **)scan2.Matches()[0];
		
		constexpr uint8_t prologue[] = {
			0x55,       // +0000  push ebp
			0x8b, 0xec, // +0001  mov ebp,esp
		};
		CBasicScan scan3(ScanDir::REVERSE, ScanResults::FIRST, CAddrOffBounds(p_in_func, -0x1000), 0x10, (const void *)prologue, sizeof(prologue));
		if (scan3.Matches().size() != 1) { Fail(3); return false; }
		auto p_func = (uintptr_t)scan3.Matches()[0];
		
		auto p_VT = (const uintptr_t *)AddrManager::GetAddr(this->GetVTableName());
		if (p_VT == nullptr) { Fail(4); return false; }
		
		uintptr_t vfptr = p_VT[this->GetVTableIndex()];
		if (vfptr != p_func) { Fail(5); return false; }
		
		addr = p_func;
		return true;
	}
	
protected:
	virtual const char *GetUniqueStr() const = 0;
	virtual const char *GetVTableName() const = 0;
	virtual int GetVTableIndex() const = 0;
	
private:
	void Fail(int step) const
	{
		DevMsg("IAddr_Func_UniqueStr_EBPBacktrack_KnownVTOff: FAIL @ step %d\n", step);
	}
};


/* address finder for functions with these traits:
 * 1. func is virtual and has a confidently known vtable offset
 */
class IAddr_Func_KnownVTOff : public IAddr_Sym
{
public:
	virtual bool FindAddrWin(uintptr_t& addr) const override
	{
		auto p_VT = (const uintptr_t *)AddrManager::GetAddr(this->GetVTableName());
		if (p_VT == nullptr) return false;
		
		addr = p_VT[this->GetVTableIndex()];
		return true;
	}
	
protected:
	virtual const char *GetVTableName() const = 0;
	virtual int GetVTableIndex() const = 0;
};

class CAddr_Func_KnownVTOff : public IAddr_Func_KnownVTOff
{
public:
	CAddr_Func_KnownVTOff(const char *name, const char *sym, const char *vt_name, int vt_idx) :
		m_pszName(name), m_pszSymbol(sym), m_pszVTName(vt_name), m_iVTIndex(vt_idx) {}
	
	virtual const char *GetName() const override       { return this->m_pszName; }
	virtual const char *GetSymbol() const override     { return this->m_pszSymbol; }
	virtual const char *GetVTableName() const override { return this->m_pszVTName; }
	virtual int GetVTableIndex() const override        { return this->m_iVTIndex; }
	
private:
	const char *m_pszName;
	const char *m_pszSymbol;
	const char *m_pszVTName;
	int m_iVTIndex;
};


#endif
