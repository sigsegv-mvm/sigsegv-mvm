#ifndef _INCLUDE_SIGSEGV_PATCH_H_
#define _INCLUDE_SIGSEGV_PATCH_H_


#include "common.h"


// CPatch:
// - func signature
// - base address
// - "before" buffer
// - "after" buffer
// - automatic verification
// - apply AND un-apply
// - optional fuzzy base offset


class ByteBuf
{
public:
	ByteBuf(int size);
	virtual ~ByteBuf();
	
	uint8_t& operator[](int idx);
	const uint8_t& operator[](int idx) const;
	
	void SetAll(uint8_t val);
	void SetRange(int idx, int len, uint8_t val);
	
	void CopyFrom(const ByteBuf& that);
	void CopyFrom(const uint8_t *arr);
	
private:
	const int m_iSize;
	uint8_t *m_Buf = nullptr;
};


inline uint8_t& ByteBuf::operator[](int idx)
{
	assert(idx >= 0 && idx < this->m_iSize);
	return this->m_Buf[idx];
}

inline const uint8_t& ByteBuf::operator[](int idx) const
{
	assert(idx >= 0 && idx < this->m_iSize);
	return this->m_Buf[idx];
}


inline void ByteBuf::SetAll(uint8_t val)
{
	memset(this->m_Buf, val, this->m_iSize);
}

inline void ByteBuf::SetRange(int idx, int len, uint8_t val)
{
	assert(idx       >= 0 && idx       < this->m_iSize);
	assert(idx + len >= 0 && idx + len < this->m_iSize);
	
	for (int i = 0; i < len; ++i) {
		this->m_Buf[idx + i] = val;
	}
}


inline void ByteBuf::CopyFrom(const ByteBuf& that)
{
	assert(this->m_iSize == that.m_iSize);
	memcpy(this->m_Buf, that.m_Buf, this->m_iSize);
}

inline void ByteBuf::CopyFrom(const uint8_t *arr)
{
	memcpy(this->m_Buf, arr, this->m_iSize);
}


class IPatch
{
public:
	virtual ~IPatch() {}
	
	virtual bool VerifyOnly() const { return false; }
	
	bool Init(char *error, size_t maxlen);
	bool Check(char *error, size_t maxlen);
	
	virtual void Apply();
	virtual void UnApply();
	
	virtual int GetLength() const final { return this->m_iLength; }
	virtual const char *GetFuncName() const = 0;
	virtual uint32_t GetFuncOffset() const = 0;
	
protected:
	IPatch(int len) :
		m_iLength(len),
		m_BufVerify(len), m_BufPatch(len),
		m_MaskVerify(len), m_MaskPatch(len),
		m_BufRestore(len) {}
	
	virtual void GetVerifyInfo(ByteBuf& buf, ByteBuf& mask) const = 0;
	virtual void GetPatchInfo(ByteBuf& buf, ByteBuf& mask) const = 0;
	
private:
	const int m_iLength;
	
	const char *m_pszFuncName = nullptr;
	uint32_t m_iFuncOffset = 0;
	
	ByteBuf m_BufVerify;
	ByteBuf m_BufPatch;
	ByteBuf m_MaskVerify;
	ByteBuf m_MaskPatch;
	
	void *m_pFuncAddr = nullptr;
	
	bool m_bApplied = false;
	ByteBuf m_BufRestore;
};


class IVerify : public IPatch
{
public:
	virtual ~IVerify() {}
	
	virtual bool VerifyOnly() const override final { return true; }
	
	virtual void Apply() override final {}
	virtual void UnApply() override final {}
	
protected:
	IVerify(int len) : IPatch(len) {}
	
	virtual void GetPatchInfo(ByteBuf& buf, ByteBuf& mask) const override final {}
};


#endif
