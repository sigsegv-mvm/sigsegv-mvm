#ifndef _INCLUDE_SIGSEGV_LINK_LINK_H_
#define _INCLUDE_SIGSEGV_LINK_LINK_H_


#include "extension.h"
#include "common.h"
#include "util/util.h"
#include "addr/addr.h"


class ILinkage : public AutoList<ILinkage>
{
public:
	virtual ~ILinkage() {}
	
	virtual bool Link(char *error, size_t maxlen) = 0;
	
protected:
	ILinkage() {}
};


template<typename T>
class FuncThunk : public ILinkage
{
public:
	FuncThunk(const char *n_func) :
		m_pszFuncName(n_func) {}
	
	virtual bool Link(char *error, size_t maxlen) override
	{
		if (this->m_pFuncPtr == nullptr) {
			this->m_pFuncPtr = (T)AddrManager::GetAddr(this->m_pszFuncName);
			if (this->m_pFuncPtr == nullptr) {
				DevMsg("FuncThunk::Link FAIL \"%s\"\n", this->m_pszFuncName);
				snprintf(error, maxlen, "FuncThunk linkage error: signature lookup failed for \"%s\"", this->m_pszFuncName);
				return false;
			}
		}
		
		DevMsg("FuncThunk::Link OK 0x%08x \"%s\"\n", (uintptr_t)this->m_pFuncPtr, this->m_pszFuncName);
		return true;
	}
	
	const T& operator*() const
	{
		assert(this->m_pFuncPtr != nullptr);
		return this->m_pFuncPtr;
	}
	
private:
	const char *m_pszFuncName;
	
	T m_pFuncPtr = nullptr;
};


template<typename T>
class GlobalThunk : public ILinkage
{
public:
	GlobalThunk(const char *n_obj) :
		m_pszObjName(n_obj) {}
	
	virtual bool Link(char *error, size_t maxlen) override
	{
		if (this->m_pObjPtr == nullptr) {
			this->m_pObjPtr = (T *)AddrManager::GetAddr(this->m_pszObjName);
			if (this->m_pObjPtr == nullptr) {
				DevMsg("GlobalThunk::Link FAIL \"%s\"\n", this->m_pszObjName);
				snprintf(error, maxlen, "GlobalThunk linkage error: signature lookup failed for \"%s\"", this->m_pszObjName);
				return false;
			}
		}
		
		DevMsg("GlobalThunk::Link OK 0x%08x \"%s\"\n", (uintptr_t)this->m_pObjPtr, this->m_pszObjName);
		return true;
	}
	
	operator T&() const
	{
		assert(this->m_pObjPtr != nullptr);
		return *m_pObjPtr;
	}
	
	T& operator->() const
	{
		return *m_pObjPtr;
	}
	
private:
	const char *m_pszObjName;
	
	T *m_pObjPtr = nullptr;
};


#include "link/vcall.h"


namespace Link
{
	bool InitAll(char *error, size_t maxlen);
}


#endif
