#ifndef _INCLUDE_SIGSEGV_LINK_VCALL_H_
#define _INCLUDE_SIGSEGV_LINK_VCALL_H_


#include "link/link.h"
#include "abi.h"


// virtual void CFoo::FBar()
// T:        function type
// n_vtable: signature for "`vtable for'CFoo"
// n_func:   signature for "CFoo::FBar"

// will be a bit different for MSVC...

// linux: adjust vtable by adding 8
// windows: adjust vtable by subtracting 8


template<typename T>
class VFuncThunk : public ILinkage
{
public:
	VFuncThunk(const char *n_vtable, const char *n_func) :
		m_pszVTName(n_vtable), m_pszFuncName(n_func) {}
	
	virtual bool Link(char *error, size_t maxlen) override;
	
	//const T& operator*() const;
	const T& Get(const void *_this) const;
	
private:
	const char *m_pszVTName;
	const char *m_pszFuncName;
	
	int m_iVTIndex = -1;
};


template<typename T>
inline const T& VFuncThunk<T>::Get(const void *_this) const
{
	assert(this->m_iVTIndex != -1);
	
	T *const pVTable = *reinterpret_cast<T *const *>(_this);
	return pVTable[this->m_iVTIndex];
}


template<typename T>
inline bool VFuncThunk<T>::Link(char *error, size_t maxlen)
{
	T *pVT = nullptr;
	T pFunc = nullptr;
	
	if (this->m_iVTIndex == -1) {
		pVT = (T *)AddrManager::GetAddr(this->m_pszVTName);
		if (pVT == nullptr) {
			DevMsg("VFuncThunk::Link FAIL \"%s\"\n", this->m_pszFuncName);
			snprintf(error, maxlen, "VFuncThunk linkage error: signature lookup failed for \"%s\"", this->m_pszVTName);
			return false;
		}
		
		pFunc = (T)AddrManager::GetAddr(this->m_pszFuncName);
		if (pFunc == nullptr) {
			DevMsg("VFuncThunk::Link FAIL \"%s\"\n", this->m_pszFuncName);
			snprintf(error, maxlen, "VFuncThunk linkage error: signature lookup failed for \"%s\"", this->m_pszFuncName);
			return false;
		}
		
#if defined __GNUC__
		pVT = (T *)((uintptr_t)pVT + offsetof(vtable, vfptrs));
#endif
		
		bool found = false;
		for (int i = 0; i < 0x1000; ++i) {
			if (pVT[i] == pFunc) {
				this->m_iVTIndex = i;
				found = true;
				break;
			}
		}
		
		if (!found) {
			DevMsg("VFuncThunk::Link FAIL \"%s\"\n", this->m_pszFuncName);
			snprintf(error, maxlen, "VFuncThunk linkage error: vtable lookup failed for \"%s\"", this->m_pszFuncName);
			return false;
		}
	}
	
	DevMsg("VFuncThunk::Link OK +0x%x \"%s\"\n", this->m_iVTIndex * 4, this->m_pszFuncName);
	return true;
}


#endif
