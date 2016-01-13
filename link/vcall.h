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


#if defined __GNUC__

template<typename T>
inline bool VFuncThunk<T>::Link(char *error, size_t maxlen)
{
	T *pVT = nullptr;
	T pFunc = nullptr;
	
	if (this->m_iVTIndex == -1) {
		if (!g_pGameConf->GetMemSig(this->m_pszVTName, (void **)&pVT) ||
			pVT == nullptr) {
			DevMsg("VFuncThunk::Link FAIL \"%s\"\n", this->m_pszFuncName);
			snprintf(error, maxlen, "VFuncThunk linkage error: signature lookup failed for \"%s\"", this->m_pszVTName);
			return false;
		}
		if (!g_pGameConf->GetMemSig(this->m_pszFuncName, (void **)&pFunc) ||
			pFunc == nullptr) {
			DevMsg("VFuncThunk::Link FAIL \"%s\"\n", this->m_pszFuncName);
			snprintf(error, maxlen, "VFuncThunk linkage error: signature lookup failed for \"%s\"", this->m_pszFuncName);
			return false;
		}
		
		pVT = (T *)((uintptr_t)pVT - offsetof(vtable, vfptrs));
		
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

#elif defined _MSC_VER

// for MSVC:
// 1. do a string search for the ".?AV" string
// 2. ensure exactly one match; subtract 8; now we have the _TypeDescriptor
// 3. do a mem search for references to the _TypeDescriptor:
//      0x00000000 (signature)
//      0x00000000 (offset)
//      0x00000000 (cdOffset)
//      &_TypeDescriptor
// 4. ensure exactly one match; now we have the __RTTI_CompleteObjectLocator
// 5. do a mem search for references to the __RTTI_CompleteObjectLocator
// 6. ensure exactly one match; add 4; now we have the VFTable

template<typename T>
inline bool VFuncThunk<T>::Link(char *error, size_t maxlen)
{
	_TypeDescriptor *pTD = nullptr;
	__RTTI_CompleteObjectLocator *pCOL = nullptr;
	T *pVT = nullptr;
	T pFunc = nullptr;
	
	if (this->m_iVTIndex == -1) {
		if (!g_pGameConf->GetMemSig(this->m_pszVTName, (void **)&pTD) ||
			pTD == nullptr) {
			DevMsg("VFuncThunk::Link FAIL \"%s\"\n", this->m_pszFuncName);
			snprintf(error, maxlen, "VFuncThunk linkage error: signature lookup failed for \"%s\"", this->m_pszVTName);
			return false;
		}
		if (!g_pGameConf->GetMemSig(this->m_pszFuncName, (void **)&pFunc) ||
			pFunc == nullptr) {
			DevMsg("VFuncThunk::Link FAIL \"%s\"\n", this->m_pszFuncName);
			snprintf(error, maxlen, "VFuncThunk linkage error: signature lookup failed for \"%s\"", this->m_pszFuncName);
			return false;
		}
		
		pTD = (_TypeDescriptor *)((uintptr_t)pTD - offsetof(_TypeDescriptor, name));
		
		__RTTI_CompleteObjectLocator pattern_COL = {
			0x00000000,
			0x00000000,
			0x00000000,
			pTD,
		};
		
		std::vector<void *> refs_TD;
		assert(MemFindPattern(gamedll, (const char *)&pattern_COL, 0x10, refs_TD));
		
		if (refs_TD.size() != 1) {
			DevMsg("VFuncThunk::Link FAIL \"%s\"\n", this->m_pszFuncName);
			snprintf(error, maxlen, "VFuncThunk linkage error: %d TD refs for \"%s\"", refs_TD.size(), this->m_pszFuncName);
			return false;
		}
		
		pCOL = (__RTTI_CompleteObjectLocator *)refs_TD[0];
		
		std::vector<void *>refs_COL;
		assert(MemFindPattern(gamedll, (const char *)pCOL, 0x4, refs_COL));
		
		if (refs_COL.size() != 1) {
			DevMsg("VFuncThunk::Link FAIL \"%s\"\n", this->m_pszFuncName);
			snprintf(error, maxlen, "VFuncThunk linkage error: %d COL refs for \"%s\"", refs_COL.size(), this->m_pszFuncName);
			return false;
		}
		
		pVT = (T *)((uintptr_t)refs_COL[0] + 0x4);
		
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


#endif
