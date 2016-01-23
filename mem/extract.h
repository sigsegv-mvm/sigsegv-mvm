#ifndef _INCLUDE_SIGSEGV_MEM_EXTRACT_H_
#define _INCLUDE_SIGSEGV_MEM_EXTRACT_H_


#include "util/buf.h"
#include "addr/addr.h"


#warning add fuzzy functionality from IPatch (if it works)


template<typename T>
class IExtract
{
public:
	virtual ~IExtract() {}
	
	bool Init();
	bool Check();
	
	T Extract();
	
	virtual int GetLength() const final { return this->m_iLength; }
	virtual const char *GetFuncName() const = 0;
	virtual uint32_t GetFuncOffset() const = 0;
	virtual uint32_t GetExtractOffset() const = 0;
	
	virtual T AdjustValue(T val) const { return val; }
	
protected:
	IExtract(int len) :
		m_iLength(len),
		m_BufExtract(len), m_MaskExtract(len) {}
	
	virtual void GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const = 0;
	
private:
	const int m_iLength;
	
	const char *m_pszFuncName = nullptr;
	uint32_t m_iFuncOffset = 0;
	uint32_t m_iExtractOffset = 0;
	
	ByteBuf m_BufExtract;
	ByteBuf m_MaskExtract;
	
	void *m_pFuncAddr = nullptr;
};


template<typename T>
T IExtract<T>::Extract()
{
	T val = *reinterpret_cast<T *>((uintptr_t)this->m_pFuncAddr +
		this->m_iFuncOffset + this->m_iExtractOffset);
	
	return this->AdjustValue(val);
}

template<typename T>
bool IExtract<T>::Init()
{
	this->m_pszFuncName = this->GetFuncName();
	this->m_iFuncOffset = this->GetFuncOffset();
	this->m_iExtractOffset = this->GetExtractOffset();
	
	assert(this->m_iExtractOffset + sizeof(T) <= (unsigned int)this->m_iLength);
	
	this->m_pFuncAddr = AddrManager::GetAddr(this->m_pszFuncName);
	if (this->m_pFuncAddr == nullptr) {
		return false;
	}
	
	this->m_MaskExtract.SetAll(0xff);
	
	this->GetExtractInfo(this->m_BufExtract, this->m_MaskExtract);
	
	return true;
}

template<typename T>
bool IExtract<T>::Check()
{
	uint8_t *ptr = (uint8_t *)((uintptr_t)this->m_pFuncAddr + this->m_iFuncOffset);
	for (int i = 0; i < this->m_iLength; ++i) {
		uint8_t *mem = ptr + i;
		
		uint8_t x_byte = this->m_BufExtract[i];
		uint8_t x_mask = this->m_MaskExtract[i];
		
		if ((*mem & x_mask) != (x_byte & x_mask)) {
			return false;
		}
	}
	
	return true;
}


#endif
