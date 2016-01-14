#ifndef _INCLUDE_SIGSEGV_ADDR_ADDR_H_
#define _INCLUDE_SIGSEGV_ADDR_ADDR_H_


#include "common.h"
#include "extension.h"
#include "util/util.h"


class AddrManager
{
public:
	static void Load();
	static void UnLoad();
	
	static void *GetAddr(const char *name);
	
	static void *FindSymbol(const char *lib, const char *sym);
	
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
	virtual const char *GetLibrary() const            { return "server"; }
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


#endif
