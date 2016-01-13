#ifndef _INCLUDE_SIGSEGV_PROP_DATAMAP_H_
#define _INCLUDE_SIGSEGV_PROP_DATAMAP_H_


#include "common.h"
#include "extension.h"


template<typename T, const char *const *CLASS, const char *const *PROP>
class CProp_DataMap
{
public:
	const T& operator=(const T& val) { this->Set(val); return val; }
	operator const T&() const        { return this->Get(); }
	
private:
	const T& Get() const;
	void Set(const T& val) const;
	
	static int CalcOffset();
	
	static bool s_bInit;
	static int s_iOffset;
};

template<typename T, const char *const *CLASS, const char *const *PROP>
bool CProp_DataMap<T, CLASS, PROP>::s_bInit = false;

template<typename T, const char *const *CLASS, const char *const *PROP>
int CProp_DataMap<T, CLASS, PROP>::s_iOffset = 0;

template<typename T, const char *const *CLASS, const char *const *PROP>
inline const T& CProp_DataMap<T, CLASS, PROP>::Get() const
{
	if (!s_bInit) {
		s_iOffset = CalcOffset();
		s_bInit = true;
	}
	
	return *reinterpret_cast<const T *>((uintptr_t)this + s_iOffset);
}

template<typename T, const char *const *CLASS, const char *const *PROP>
inline void CProp_DataMap<T, CLASS, PROP>::Set(const T& val) const
{
	if (!s_bInit) {
		s_iOffset = CalcOffset();
		s_bInit = true;
	}
	
	*reinterpret_cast<T *>((uintptr_t)this + s_iOffset) = val;
}

template<typename T, const char *const *CLASS, const char *const *PROP>
inline int CProp_DataMap<T, CLASS, PROP>::CalcOffset()
{
	char str_GetDataDescMap[256];
	snprintf(str_GetDataDescMap, sizeof(str_GetDataDescMap), "%s::GetDataDescMap", *CLASS);
	
	datamap_t * (*p_GetDataDescMap)(void *) = nullptr;
	assert(g_pGameConf->GetMemSig(str_GetDataDescMap, (void **)&p_GetDataDescMap) && p_GetDataDescMap != nullptr);
	
	datamap_t *pMap = (*p_GetDataDescMap)(nullptr);
	
	sm_datatable_info_t info;
	assert(gamehelpers->FindDataMapInfo(pMap, *PROP, &info));
	
	int offset = info.actual_offset;
	
	DevMsg("[DATAMAP]  0x%04x %s::%s\n", offset, *CLASS, *PROP);
	return offset;
}


#endif
