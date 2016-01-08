#ifndef _INCLUDE_SIGSEGV_PROP_H_
#define _INCLUDE_SIGSEGV_PROP_H_


#include "extension.h"
#include "extract.h"


template<typename C, typename T>
class IProp
{
public:
	virtual const T& Get(const C *obj)
	{
		if (!this->m_bInit) {
			this->m_iOffset = this->CalcOffset();
			this->m_bInit = true;
		}
		
		return *(const T *)((uintptr_t)obj + this->m_iOffset);
	}
	
	virtual void Set(C *obj, const T& val)
	{
		if (!this->m_bInit) {
			this->m_iOffset = this->CalcOffset();
			this->m_bInit = true;
		}
		
		*(T *)((uintptr_t)obj + this->m_iOffset) = val;
	}
	
protected:
	IProp() {}
	
	virtual int CalcOffset() = 0;
	
private:
	bool m_bInit = false;
	int m_iOffset = 0;
};


template<typename C, typename T>
class CProp_SendProp : public IProp<C, T>
{
public:
	CProp_SendProp(const char *n_class, const char *n_prop) :
		m_pszClassName(n_class), m_pszPropName(n_prop) {}
	
	virtual void Set(C *obj, const T& val) override;
	
private:
	virtual int CalcOffset() override;
	
	const char *m_pszClassName;
	const char *m_pszPropName;
};

template<typename C, typename T>
void CProp_SendProp<C, T>::Set(C *obj, const T& val)
{
	/* TODO: update network state */
	assert(0);
	
	IProp<C, T>::Set(obj, val);
}

template<typename C, typename T>
int CProp_SendProp<C, T>::CalcOffset()
{
#if 0
	SendProp *pProp = gamehelpers->FindInSendTable(this->m_pszClassName, this->m_pszPropName);
	assert(pProp != nullptr);
	
	this->m_iOffset = gamehelpers->GetSendPropOffset(pProp);
	this->m_bInit = true;
#endif
	
	sm_sendprop_info_t info;
	assert(gamehelpers->FindSendPropInfo(this->m_pszClassName, this->m_pszPropName, &info));
	
	int offset = info.actual_offset;
	
	DevMsg("[SENDPROP] 0x%04x %s::%s\n", offset, this->m_pszClassName, this->m_pszPropName);
	return offset;
}


template<typename C, typename T>
class CProp_DataMap : public IProp<C, T>
{
public:
	CProp_DataMap(const char *n_class, const char *n_prop) :
		m_pszClassName(n_class), m_pszPropName(n_prop)
	{
		snprintf(this->m_szGetDataDescMap, sizeof(this->m_szGetDataDescMap), "%s::GetDataDescMap", n_class);
	}
	
private:
	virtual int CalcOffset() override;
	
	const char *m_pszClassName;
	const char *m_pszPropName;
	
	char m_szGetDataDescMap[1024];
};

template<typename C, typename T>
int CProp_DataMap<C, T>::CalcOffset()
{
	datamap_t * (*p_GetDataDescMap)(C *) = nullptr;
	assert(g_pGameConf->GetMemSig(this->m_szGetDataDescMap, (void **)&p_GetDataDescMap) && p_GetDataDescMap != nullptr);
	
	datamap_t *pMap = (*p_GetDataDescMap)(nullptr);
	
	sm_datatable_info_t info;
	assert(gamehelpers->FindDataMapInfo(pMap, this->m_pszPropName, &info));
	
	int offset = info.actual_offset;
	
	DevMsg("[DATAMAP]  0x%04x %s::%s\n", offset, this->m_pszClassName, this->m_pszPropName);
	return offset;
}


template<typename C, typename T>
class CProp_Extract : public IProp<C, T>
{
public:
	CProp_Extract(const char *n_class, const char *n_prop, IExtract<T> *extractor) :
		m_pszClassName(n_class), m_pszPropName(n_prop), m_pExtract(extractor) {}
	
	virtual ~CProp_Extract()
	{
		if (this->m_pExtract != nullptr) {
			delete this->m_pExtract;
		}
	}
	
private:
	virtual int CalcOffset() override;
	
	const char *m_pszClassName;
	const char *m_pszPropName;
	
	IExtract<T> *m_pExtract;
};

template<typename C, typename T>
int CProp_Extract<C, T>::CalcOffset()
{
	assert(this->m_pExtract->Init());
	assert(this->m_pExtract->Check());
	
	int offset = this->m_pExtract->Extract();
	
	DevMsg("[EXTRACT]  0x%04x %s::%s\n", offset, this->m_pszClassName, this->m_pszPropName);
	return offset;
}


#endif
