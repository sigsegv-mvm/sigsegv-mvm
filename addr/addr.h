#ifndef _INCLUDE_SIGSEGV_ADDR_ADDR_H_
#define _INCLUDE_SIGSEGV_ADDR_ADDR_H_


#include "library.h"
#include "util/util.h"


class IAddr;

class AddrManager
{
public:
	static void Load();
	
	static void *GetAddr(const char *name);
	
private:
	AddrManager() {}
	
	static std::map<std::string, IAddr *> s_Addrs;
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
	virtual bool FindAddrLinux(uintptr_t& addr) const override;
	
protected:
	virtual const char *GetSymbol() const = 0;
};

class CAddr_Sym : public IAddr_Sym
{
public:
	CAddr_Sym(const std::string& name, const std::string& sym) :
		m_strName(name), m_strSymbol(sym) {}
	
	virtual const char *GetName() const override   { return this->m_strName.c_str(); }
	virtual const char *GetSymbol() const override { return this->m_strSymbol.c_str(); }
	
private:
	std::string m_strName;
	std::string m_strSymbol;
};


#if 0
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
#endif


class IAddr_VTable : public IAddr_Sym
{
public:
	virtual bool FindAddrLinux(uintptr_t& addr) const override;
	virtual bool FindAddrWin(uintptr_t& addr) const override;
	
protected:
	virtual const char *GetWinRTTIStr() const = 0;
};

class CAddr_VTable : public IAddr_VTable
{
public:
	CAddr_VTable(const std::string& name, const std::string& sym, const std::string& rtti) :
		m_strName(name), m_strSymbol(sym), m_strWinRTTIStr(rtti) {}
	
	virtual const char *GetName() const override       { return this->m_strName.c_str(); }
	virtual const char *GetSymbol() const override     { return this->m_strSymbol.c_str(); }
	virtual const char *GetWinRTTIStr() const override { return this->m_strWinRTTIStr.c_str(); }
	
private:
	std::string m_strName;
	std::string m_strSymbol;
	std::string m_strWinRTTIStr;
};


/* address finder for functions with these traits:
 * 1. func is virtual and has a confidently known vtable index
 */
class IAddr_Func_KnownVTIdx : public IAddr_Sym
{
public:
	virtual bool FindAddrWin(uintptr_t& addr) const override;
	
protected:
	virtual const char *GetVTableName() const = 0;
	virtual int GetVTableIndex() const = 0;
};

class CAddr_Func_KnownVTIdx : public IAddr_Func_KnownVTIdx
{
public:
	CAddr_Func_KnownVTIdx(const std::string& name, const std::string& sym, const std::string& vt_name, int vt_idx) :
		m_strName(name), m_strSymbol(sym), m_strVTName(vt_name), m_iVTIndex(vt_idx) {}
	
	virtual const char *GetName() const override       { return this->m_strName.c_str(); }
	virtual const char *GetSymbol() const override     { return this->m_strSymbol.c_str(); }
	virtual const char *GetVTableName() const override { return this->m_strVTName.c_str(); }
	virtual int GetVTableIndex() const override        { return this->m_iVTIndex; }
	
private:
	std::string m_strName;
	std::string m_strSymbol;
	std::string m_strVTName;
	int m_iVTIndex;
};


/* address finder for functions with these traits:
 * 1. func body starts with "push ebp; mov ebp,esp"
 * 2. func body contains a unique string reference
 */
class IAddr_Func_EBPPrologue_UniqueStr : public IAddr_Sym
{
public:
	virtual bool FindAddrWin(uintptr_t& addr) const override;
	
protected:
	virtual const char *GetUniqueStr() const = 0;
};

class CAddr_Func_EBPPrologue_UniqueStr : public IAddr_Func_EBPPrologue_UniqueStr
{
public:
	CAddr_Func_EBPPrologue_UniqueStr(const std::string& name, const std::string& sym, const std::string& uni_str) :
		m_strName(name), m_strSymbol(sym), m_strUniqueStr(uni_str) {}
	
	virtual const char *GetName() const override       { return this->m_strName.c_str(); }
	virtual const char *GetSymbol() const override     { return this->m_strSymbol.c_str(); }
	virtual const char *GetUniqueStr() const override  { return this->m_strUniqueStr.c_str(); }
	
private:
	std::string m_strName;
	std::string m_strSymbol;
	std::string m_strUniqueStr;
};


/* address finder for functions with these traits:
 * 1. func body starts with "push ebp; mov ebp,esp"
 * 2. func body contains a unique string reference
 * 3. func is virtual and has a confidently known vtable index
 */
class IAddr_Func_EBPPrologue_UniqueStr_KnownVTIdx : public IAddr_Sym
{
public:
	virtual bool FindAddrWin(uintptr_t& addr) const override;
	
protected:
	virtual const char *GetUniqueStr() const = 0;
	virtual const char *GetVTableName() const = 0;
	virtual int GetVTableIndex() const = 0;
};

class CAddr_Func_EBPPrologue_UniqueStr_KnownVTIdx : public IAddr_Func_EBPPrologue_UniqueStr_KnownVTIdx
{
public:
	CAddr_Func_EBPPrologue_UniqueStr_KnownVTIdx(const std::string& name, const std::string& sym, const std::string& uni_str, const std::string& vt_name, int vt_idx) :
		m_strName(name), m_strSymbol(sym), m_strUniqueStr(uni_str), m_strVTName(vt_name), m_iVTIndex(vt_idx) {}
	
	virtual const char *GetName() const override       { return this->m_strName.c_str(); }
	virtual const char *GetSymbol() const override     { return this->m_strSymbol.c_str(); }
	virtual const char *GetUniqueStr() const override  { return this->m_strUniqueStr.c_str(); }
	virtual const char *GetVTableName() const override { return this->m_strVTName.c_str(); }
	virtual int GetVTableIndex() const override        { return this->m_iVTIndex; }
	
private:
	std::string m_strName;
	std::string m_strSymbol;
	std::string m_strUniqueStr;
	std::string m_strVTName;
	int m_iVTIndex;
};


#endif
