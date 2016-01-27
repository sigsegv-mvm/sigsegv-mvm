#ifndef _INCLUDE_SIGSEGV_LINK_VCALL_H_
#define _INCLUDE_SIGSEGV_LINK_VCALL_H_


#include "link/link.h"
#include "abi.h"
#include "util/rtti.h"


// virtual void CFoo::FBar()
// FUNC:     function type
// n_vtable: signature for "`vtable for'CFoo"
// n_func:   signature for "CFoo::FBar"

// will be a bit different for MSVC...

// linux: adjust vtable by adding 8
// windows: adjust vtable by subtracting 8


template<class T, typename FUNC>
class VFuncThunk : public ILinkage
{
public:
	VFuncThunk(const char *n_func) :
		m_pszFuncName(n_func) {}
	
	virtual bool Link() override;
	
	//const FUNC& operator*() const;
	const FUNC& Get(const void *_this) const;
	
private:
	const char *m_pszFuncName;
	
	int m_iVTIndex = -1;
};


template<class T, typename FUNC>
inline const FUNC& VFuncThunk<T, FUNC>::Get(const void *_this) const
{
	assert(this->m_iVTIndex != -1);
	
	FUNC *const pVTable = *reinterpret_cast<FUNC *const *>(_this);
	return pVTable[this->m_iVTIndex];
}


template<class T, typename FUNC>
inline bool VFuncThunk<T, FUNC>::Link()
{
	FUNC *pVT = nullptr;
	FUNC pFunc = nullptr;
	
	if (this->m_iVTIndex == -1) {
		pVT = (FUNC *)RTTI::GetVTable<T>();
		if (pVT == nullptr) {
			DevMsg("VFuncThunk::Link FAIL \"%s\": can't find vtable\n", this->m_pszFuncName);
			return false;
		}
		
		pFunc = (FUNC)AddrManager::GetAddr(this->m_pszFuncName);
		if (pFunc == nullptr) {
			DevMsg("VFuncThunk::Link FAIL \"%s\": can't find func addr\n", this->m_pszFuncName);
			return false;
		}
		
#if defined __GNUC__
		pVT = (FUNC *)((uintptr_t)pVT + offsetof(vtable, vfptrs));
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
			DevMsg("VFuncThunk::Link FAIL \"%s\": can't find func ptr in vtable\n", this->m_pszFuncName);
			return false;
		}
	}
	
	DevMsg("VFuncThunk::Link OK +0x%x \"%s\"\n", this->m_iVTIndex * 4, this->m_pszFuncName);
	return true;
}


#endif
