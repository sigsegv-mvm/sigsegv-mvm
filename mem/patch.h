#ifndef _INCLUDE_SIGSEGV_MEM_PATCH_H_
#define _INCLUDE_SIGSEGV_MEM_PATCH_H_


#include "util/util.h"
#include "addr/addr.h"


// CPatch:
// - func signature
// - base address
// - "before" buffer
// - "after" buffer
// - automatic verification
// - apply AND un-apply
// - optional fuzzy base offset


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
