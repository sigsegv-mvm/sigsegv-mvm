#ifndef _INCLUDE_SIGSEGV_PROP_SENDPROP_H_
#define _INCLUDE_SIGSEGV_PROP_SENDPROP_H_


#include "extension.h"


template<typename T, const char *const *CLASS, const char *const *PROP>
class CProp_SendProp
{
public:
	const T& operator=(const T& val) { this->Set(val); return val; }
	operator const T&() const        { return this->Get(); }
	operator T*() const              { return this->GetPtr(); }
	
	T *GetPtr() const;
	
private:
	const T& Get() const;
	void Set(const T& val) const;
	
	static int CalcOffset();
	
	static bool s_bInit;
	static int s_iOffset;
};

template<typename T, const char *const *CLASS, const char *const *PROP>
bool CProp_SendProp<T, CLASS, PROP>::s_bInit = false;

template<typename T, const char *const *CLASS, const char *const *PROP>
int CProp_SendProp<T, CLASS, PROP>::s_iOffset = 0;

template<typename T, const char *const *CLASS, const char *const *PROP>
inline T *CProp_SendProp<T, CLASS, PROP>::GetPtr() const
{
	if (!s_bInit) {
		s_iOffset = CalcOffset();
		s_bInit = true;
	}
	
	return reinterpret_cast<T *>((uintptr_t)this + s_iOffset);
}

template<typename T, const char *const *CLASS, const char *const *PROP>
inline const T& CProp_SendProp<T, CLASS, PROP>::Get() const
{
	return *this->GetPtr();
}

template<typename T, const char *const *CLASS, const char *const *PROP>
inline void CProp_SendProp<T, CLASS, PROP>::Set(const T& val) const
{
	if (!s_bInit) {
		s_iOffset = CalcOffset();
		s_bInit = true;
	}
	
	/* TODO: update network state */
	assert(false);
	
	*reinterpret_cast<T *>((uintptr_t)this + s_iOffset) = val;
}

template<typename T, const char *const *CLASS, const char *const *PROP>
inline int CProp_SendProp<T, CLASS, PROP>::CalcOffset()
{
#if 0
	SendProp *pProp = gamehelpers->FindInSendTable(this->m_pszClassName, this->m_pszPropName);
	assert(pProp != nullptr);
	
	this->m_iOffset = gamehelpers->GetSendPropOffset(pProp);
	this->m_bInit = true;
#endif
	
	sm_sendprop_info_t info;
	assert(gamehelpers->FindSendPropInfo(*CLASS, *PROP, &info));
	
	int offset = info.actual_offset;
	
	DevMsg("[SENDPROP] 0x%04x %s::%s\n", offset, *CLASS, *PROP);
	return offset;
}


#endif
