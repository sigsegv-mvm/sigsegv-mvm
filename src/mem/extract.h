#ifndef _INCLUDE_SIGSEGV_MEM_EXTRACT_H_
#define _INCLUDE_SIGSEGV_MEM_EXTRACT_H_


#include "util/buf.h"
#include "mem/scan.h"
#include "addr/addr.h"


template<typename T>
class IExtract
{
public:
	virtual ~IExtract() {}
	
	bool Init();
	bool Check();
	
	T Extract();
	
	int GetLength() const { return this->m_iLength; }
	virtual const char *GetFuncName() const = 0;
	virtual void *GetFuncAddr() const { return nullptr; }
	virtual uint32_t GetFuncOffMin() const = 0;
	virtual uint32_t GetFuncOffMax() const = 0;
	virtual uint32_t GetExtractOffset() const = 0;
	
	virtual T AdjustValue(T val) const { return val; }
	
protected:
	IExtract(int len) :
		m_iLength(len),
		m_BufExtract(len), m_MaskExtract(len) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const = 0;
	
private:
	const int m_iLength;
	
	const char *m_pszFuncName = nullptr;
	uint32_t m_iFuncOffMin = 0;
	uint32_t m_iFuncOffMax = 0;
	uint32_t m_iExtractOffset = 0;
	
	bool m_bFoundOffset = false;
	uint32_t m_iFuncOffActual = 0;
	
	ByteBuf m_BufExtract;
	ByteBuf m_MaskExtract;
	
	void *m_pFuncAddr = nullptr;
};


template<typename T>
T IExtract<T>::Extract()
{
	assert(this->m_bFoundOffset);
	T val = *reinterpret_cast<T *>((uintptr_t)this->m_pFuncAddr +
		this->m_iFuncOffActual + this->m_iExtractOffset);
	
	return this->AdjustValue(val);
}

template<typename T>
bool IExtract<T>::Init()
{
	this->m_pszFuncName = this->GetFuncName();
	if (this->m_pszFuncName != nullptr) {
		this->m_pFuncAddr = AddrManager::GetAddr(this->m_pszFuncName);
	} else {
		this->m_pFuncAddr = this->GetFuncAddr();
	}
	
	if (this->m_pFuncAddr == nullptr) {
		return false;
	}
	
	this->m_iFuncOffMin = this->GetFuncOffMin();
	this->m_iFuncOffMax = this->GetFuncOffMax();
	this->m_iExtractOffset = this->GetExtractOffset();
	
	assert(this->m_iExtractOffset + sizeof(T) <= (unsigned int)this->m_iLength);
	
	this->m_MaskExtract.SetAll(0xff);
	
	if (!this->GetExtractInfo(this->m_BufExtract, this->m_MaskExtract)) {
		return false;
	}
	
	return true;
}

template<typename T>
bool IExtract<T>::Check()
{
	using ExtractScanner = CMaskedScanner<ScanDir::FORWARD, ScanResults::ALL, 1>;
	
	uintptr_t addr_min = (uintptr_t)this->m_pFuncAddr + this->m_iFuncOffMin;
	uintptr_t addr_max = (uintptr_t)this->m_pFuncAddr + this->m_iFuncOffMax + this->m_iLength;
	
//	DevMsg("addr_min: %08x\n", addr_min);
//	DevMsg("addr_max: %08x\n", addr_max);
//	DevMsg("length:   %08x\n", this->m_iLength);
//	
//	DevMsg("m_BufExtract:\n");
//	for (int i = 0; i < this->m_iLength; ++i) {
//		DevMsg(" %02x", this->m_BufExtract[i]);
//	}
//	DevMsg("\nm_MaskExtract:\n");
//	for (int i = 0; i < this->m_iLength; ++i) {
//		DevMsg(" %02x", this->m_MaskExtract[i]);
//	}
//	DevMsg("\n");
	
	CScan<ExtractScanner> scan(CAddrAddrBounds((void *)addr_min, (void *)addr_max), this->m_BufExtract, this->m_MaskExtract);
	if (!scan.ExactlyOneMatch()) {
		DevMsg("IExtract::Check: FAIL: \"%s\": found %u matching regions\n", this->m_pszFuncName, scan.Matches().size());
		return false;
	}
	
	this->m_bFoundOffset = true;
	this->m_iFuncOffActual = (uintptr_t)scan.FirstMatch() - (uintptr_t)this->m_pFuncAddr;
	
	DevMsg("IExtract::Check: OK: \"%s\": actual offset +0x%04x\n", this->m_pszFuncName, this->m_iFuncOffActual);
	
	return true;
}


class IExtractStub final
{
public:
	template<typename... ARGS> IExtractStub(ARGS...) {}
};


#endif
