#ifndef _INCLUDE_SIGSEGV_LINK_LINK_H_
#define _INCLUDE_SIGSEGV_LINK_LINK_H_


#include "extension.h"
#include "common.h"
#include "util/util.h"


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
	FuncThunk(const char *name) :
		m_pszFuncName(name) {}
	
	virtual bool Link(char *error, size_t maxlen) override
	{
		if (this->m_pFuncPtr == nullptr) {
			if (!g_pGameConf->GetMemSig(this->m_pszFuncName, (void **)&this->m_pFuncPtr) ||
				this->m_pFuncPtr == nullptr) {
				DevMsg("FuncThunk::Link FAIL \"%s\"\n", this->m_pszFuncName);
				snprintf(error, maxlen, "FuncThunk linkage error: signature lookup failed for \"%s\"", this->m_pszFuncName);
				return false;
			}
		}
		
		DevMsg("FuncThunk::Link OK \"%s\"\n", this->m_pszFuncName);
		return true;
	}
	
	const T& operator*()
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
	GlobalThunk(const char *name) :
		m_pszObjName(name) {}
	
	virtual bool Link(char *error, size_t maxlen) override
	{
		if (this->m_pObjPtr == nullptr) {
			if (!g_pGameConf->GetMemSig(this->m_pszObjName, (void **)&this->m_pObjPtr) ||
				this->m_pObjPtr == nullptr) {
				DevMsg("GlobalThunk::Link FAIL \"%s\"\n", this->m_pszObjName);
				snprintf(error, maxlen, "GlobalThunk linkage error: signature lookup failed for \"%s\"", this->m_pszObjName);
				return false;
			}
		}
		
		DevMsg("GlobalThunk::Link OK \"%s\"\n", this->m_pszObjName);
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


namespace Link
{
	bool InitAll(char *error, size_t maxlen);
}


#endif
