#ifndef _INCLUDE_SIGSEGV_ADDR_STANDARD_H_
#define _INCLUDE_SIGSEGV_ADDR_STANDARD_H_


#include "addr/addr.h"


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


class IAddr_FixedAddr : public IAddr_Sym
{
public:
	virtual bool FindAddrWin(uintptr_t& addr) const override;
	
protected:
	virtual int GetAddress() const = 0;
	virtual int GetServerVersion() const = 0;
};

class CAddr_FixedAddr : public IAddr_FixedAddr
{
public:
	CAddr_FixedAddr(const std::string& name, const std::string& sym, int addr, int sv_version) :
		m_strName(name), m_strSymbol(sym), m_iAddr(addr), m_iServerVersion(sv_version) {}
	
	virtual const char *GetName() const override   { return this->m_strName.c_str(); }
	virtual const char *GetSymbol() const override { return this->m_strSymbol.c_str(); }
	virtual int GetAddress() const override        { return this->m_iAddr; }
	virtual int GetServerVersion() const override  { return this->m_iServerVersion; }
	
private:
	std::string m_strName;
	std::string m_strSymbol;
	int m_iAddr;
	int m_iServerVersion;
};


class IAddr_DataDescMap : public IAddr_Sym
{
public:
	virtual bool FindAddrWin(uintptr_t& addr) const override;
	
protected:
	virtual const char *GetClassName() const = 0;
};

class CAddr_DataDescMap : public IAddr_DataDescMap
{
public:
	CAddr_DataDescMap(const std::string& name, const std::string& sym, const std::string& class_name) :
		m_strName(name), m_strSymbol(sym), m_strClassName(class_name) {}
	
	virtual const char *GetName() const override      { return this->m_strName.c_str(); }
	virtual const char *GetSymbol() const override    { return this->m_strSymbol.c_str(); }
	virtual const char *GetClassName() const override { return this->m_strClassName.c_str(); }
	
private:
	std::string m_strName;
	std::string m_strSymbol;
	std::string m_strClassName;
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


class IAddr_Func_DataMap_VThunk : public IAddr_Sym
{
public:
	virtual bool FindAddrWin(uintptr_t& addr) const override;
	
protected:
	virtual const char *GetDataMapName() const = 0;
	virtual const char *GetFuncName() const = 0;
	virtual const char *GetVTableName() const = 0;
};

class CAddr_Func_DataMap_VThunk : public IAddr_Func_DataMap_VThunk
{
public:
	CAddr_Func_DataMap_VThunk(const std::string& name, const std::string& sym, const std::string& dm_name, const std::string& func_name, const std::string& vt_name) :
		m_strName(name), m_strSymbol(sym), m_strDataMapName(dm_name), m_strFuncName(func_name), m_strVTName(vt_name) {}
	
	virtual const char *GetName() const override        { return this->m_strName.c_str(); }
	virtual const char *GetSymbol() const override      { return this->m_strSymbol.c_str(); }
	virtual const char *GetDataMapName() const override { return this->m_strDataMapName.c_str(); }
	virtual const char *GetFuncName() const override    { return this->m_strFuncName.c_str(); }
	virtual const char *GetVTableName() const override  { return this->m_strVTName.c_str(); }
	
private:
	std::string m_strName;
	std::string m_strSymbol;
	std::string m_strDataMapName;
	std::string m_strFuncName;
	std::string m_strVTName;
};


/* address finder for functions with these traits:
 * 1. func body starts with "push ebp; mov ebp,esp"
 * 2. func body contains a unique absolute symbol reference
 */
class IAddr_Func_EBPPrologue_UniqueRef : public IAddr_Sym
{
public:
	virtual bool FindAddrWin(uintptr_t& addr) const override;
	
protected:
	virtual const char *GetUniqueSymbol() const = 0;
};

class CAddr_Func_EBPPrologue_UniqueRef : public IAddr_Func_EBPPrologue_UniqueRef
{
public:
	CAddr_Func_EBPPrologue_UniqueRef(const std::string& name, const std::string& sym, const std::string& uni_ref) :
		m_strName(name), m_strSymbol(sym), m_strUniqueSymbol(uni_ref) {}
	
	virtual const char *GetName() const override         { return this->m_strName.c_str(); }
	virtual const char *GetSymbol() const override       { return this->m_strSymbol.c_str(); }
	virtual const char *GetUniqueSymbol() const override { return this->m_strUniqueSymbol.c_str(); }
	
private:
	std::string m_strName;
	std::string m_strSymbol;
	std::string m_strUniqueSymbol;
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
	
	virtual const char *GetName() const override      { return this->m_strName.c_str(); }
	virtual const char *GetSymbol() const override    { return this->m_strSymbol.c_str(); }
	virtual const char *GetUniqueStr() const override { return this->m_strUniqueStr.c_str(); }
	
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


class IAddr_Func_EBPPrologue_VProf : public IAddr_Sym
{
public:
	virtual bool FindAddrWin(uintptr_t& addr) const override;
	
protected:
	virtual const char *GetVProfName() const = 0;
	virtual const char *GetVProfGroup() const = 0;
};

class CAddr_Func_EBPPrologue_VProf : public IAddr_Func_EBPPrologue_VProf
{
public:
	CAddr_Func_EBPPrologue_VProf(const std::string& name, const std::string& sym, const std::string& vprof_name, const std::string& vprof_group) :
		m_strName(name), m_strSymbol(sym), m_strVProfName(vprof_name), m_strVProfGroup(vprof_group) {}
	
	virtual const char *GetName() const override       { return this->m_strName.c_str(); }
	virtual const char *GetSymbol() const override     { return this->m_strSymbol.c_str(); }
	virtual const char *GetVProfName() const override  { return this->m_strVProfName.c_str(); }
	virtual const char *GetVProfGroup() const override { return this->m_strVProfGroup.c_str(); }
	
private:
	std::string m_strName;
	std::string m_strSymbol;
	std::string m_strVProfName;
	std::string m_strVProfGroup;
};


#if 0
/* address finder for functions with these traits:
 * 1. func body starts with "push ebp; mov ebp,esp"
 * 2. func body contains a unique reference to a particular convar
 */
class IAddr_Func_EBPPrologue_UniqueConVar : public IAddr_Sym
{
public:
	enum class DataType : int
	{
		INT,
		FLOAT,
		STRING,
	};
	
	virtual bool FindAddrWin(uintptr_t& addr) const override;
	
protected:
	virtual const char *GetConVarName() const = 0;
	virtual DataType GetConVarType() const = 0;
};

class CAddr_Func_EBPPrologue_UniqueConVar : public IAddr_Func_EBPPrologue_UniqueConVar
{
public:
	CAddr_Func_EBPPrologue_UniqueConVar(const std::string& name, DataType type) :
		m_strName(name), m_iType(type) {}
	
	virtual const char *GetConVarName() const override { return this->m_strName.c_str(); }
	virtual DataType GetConVarType() const override    { return this->m_iType; }
	
private:
	std::string m_strName;
	DataType m_iType;
};
#endif


#if 0
class IAddr_Func_EBPPrologue_UniqueCall : public IAddr_Sym
{
public:
	virtual bool FindAddrWin(uintptr_t& addr) const override;
	#error TODO: implement in standard.cpp
	
protected:
	virtual const char *GetUniqueFuncName() const = 0;
};

class CAddr_Func_EBPPrologue_UniqueCall : public IAddr_Func_EBPPrologue_UniqueCall
{
public:
	CAddr_Func_EBPPrologue_UniqueCall(const std::string& name, const std::string& sym, const std::string& f_name) :
		m_strName(name), m_strSymbol(sym), m_strFuncName(f_name) {}
	
	virtual const char *GetName() const override           { return this->m_strName.c_str(); }
	virtual const char *GetSymbol() const override         { return this->m_strSymbol.c_str(); }
	virtual const char *GetUniqueFuncName() const override { return this->m_strFuncName.c_str(); }
	
private:
	std::string m_strName;
	std::string m_strSymbol;
	std::string m_strFuncName;
};
#error TODO: add to gameconf.cpp


class IAddr_Func_EBPPrologue_Ctor : public IAddr_Sym
{
public:
	virtual bool FindAddrWin(uintptr_t& addr) const override;
	#error TODO: implement in standard.cpp
	
protected:
	virtual const char *GetVTableName() const = 0;
	virtual int GetDtorVTableIndex() const = 0;
};
#error TODO: add to gameconf.cpp

#error TODO: CAddr_Func_EBPPrologue_Ctor
#endif


class IAddr_Pattern : public IAddr_Sym
{
public:
	virtual bool FindAddrWin(uintptr_t& addr) const override;
	
protected:
	virtual const char *GetSegment() const = 0;
	virtual const char *GetPattern() const = 0;
	virtual const char *GetMask() const = 0;
};

class CAddr_Pattern : public IAddr_Pattern
{
public:
	CAddr_Pattern(const std::string& name, const std::string& sym, const std::string& segment, const std::string& pattern, const std::string& mask) :
		m_strName(name), m_strSymbol(sym), m_strSegment(segment), m_strPattern(pattern), m_strMask(mask) {}
	
	virtual const char *GetName() const override    { return this->m_strName.c_str(); }
	virtual const char *GetSymbol() const override  { return this->m_strSymbol.c_str(); }
	virtual const char *GetSegment() const override { return this->m_strSegment.c_str(); }
	virtual const char *GetPattern() const override { return this->m_strPattern.c_str(); }
	virtual const char *GetMask() const override    { return this->m_strMask.c_str(); }
	
private:
	std::string m_strName;
	std::string m_strSymbol;
	std::string m_strSegment;
	std::string m_strPattern;
	std::string m_strMask;
};


#endif
