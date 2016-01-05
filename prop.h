#ifndef _INCLUDE_SIGSEGV_PROP_H_
#define _INCLUDE_SIGSEGV_PROP_H_


#include "extension.h"


template<typename C, typename T>
class IProp
{
public:
	virtual const T& Get(const C *obj) = 0;
	virtual void Set(C *obj, const T& val) = 0;
	
protected:
	IProp() {}
};


template<typename C, typename T>
class CProp_SendProp : IProp<C, T>
{
public:
	CProp_SendProp(const char *gamedata) :
		m_pszGameData(gamedata) {}
	
	const T& Get(const C *obj);
	void Set(C *obj, const T& val);
	
private:
	void GetOffset();
	
	const char *m_pszGameData;
	bool m_bInit = false;
	int m_iOffset = 0;
};


template<typename C, typename T>
const T& CProp_SendProp<C, T>::Get(const C *obj)
{
	this->GetOffset();
	
	return *(const T *)((uintptr_t)obj + this->m_iOffset);
}

template<typename C, typename T>
void CProp_SendProp<C, T>::Set(C *obj, const T& val)
{
	this->GetOffset();
	
	/* TODO: update network state */
	assert(0);
}


template<typename C, typename T>
void CProp_SendProp<C, T>::GetOffset()
{
	if (!this->m_bInit) {
		assert(g_pGameConf->GetOffset(this->m_pszGameData, &this->m_iOffset));
		this->m_bInit = true;
	}
}


#endif
