#ifndef _INCLUDE_SIGSEGV_UTIL_SCAN_H_
#define _INCLUDE_SIGSEGV_UTIL_SCAN_H_


#include "library.h"
#include "util/buf.h"


enum class ScanDir : int
{
	FORWARD,
	REVERSE,
};

enum class ScanResults : int
{
	FIRST,
	ALL,
};


class IBounds
{
public:
	virtual const void *GetLowerBound() const = 0;
	virtual const void *GetUpperBound() const = 0;
	
protected:
	IBounds() {}
};

class CAddrAddrBounds : public IBounds
{
public:
	CAddrAddrBounds(const void *addr1, const void *addr2) :
		m_Addr1(addr1), m_Addr2(addr2) {}
	
	virtual const void *GetLowerBound() const override
	{
		return Min(this->m_Addr1, this->m_Addr2);
	}
	virtual const void *GetUpperBound() const override
	{
		return Max(this->m_Addr1, this->m_Addr2);
	}
	
private:
	const void *m_Addr1;
	const void *m_Addr2;
};

class CAddrOffBounds : public CAddrAddrBounds
{
public:
	CAddrOffBounds(const void *addr, ptrdiff_t off) :
		CAddrAddrBounds(addr, (const void *)((uintptr_t)addr + off)) {}
};

class CLibBounds : public IBounds
{
public:
	CLibBounds(Library lib);
	
	virtual const void *GetLowerBound() const override { return this->m_AddrLow; }
	virtual const void *GetUpperBound() const override { return this->m_AddrHigh; }
	
private:
	const void *m_AddrLow  = nullptr;
	const void *m_AddrHigh = nullptr;
};

class CLibSegBounds : public IBounds
{
public:
	CLibSegBounds(Library lib, const char *seg);
	
	virtual const void *GetLowerBound() const override { return this->m_AddrLow; }
	virtual const void *GetUpperBound() const override { return this->m_AddrHigh; }
	
private:
	const void *m_AddrLow  = nullptr;
	const void *m_AddrHigh = nullptr;
};


class IScan
{
public:
	const std::vector<void *>& Matches() const { return this->m_Matches; }
	
protected:
	IScan(ScanDir dir, ScanResults rtype, const IBounds& bounds, int align) :
		m_Dir(dir), m_RType(rtype), m_Bounds(bounds), m_Align(align) {}
	
	void DoScan();
	
	virtual int GetBufLen() const = 0;
	virtual bool CheckOne(const void *where) const = 0;
	
private:
	ScanDir m_Dir;
	ScanResults m_RType;
	const IBounds& m_Bounds;
	int m_Align;
	
	std::vector<void *> m_Matches;
};

class CBasicScan : public IScan
{
public:
	CBasicScan(ScanDir dir, ScanResults rtype, const IBounds& bounds, int align, const void *seek, int len) :
		IScan(dir, rtype, bounds, align), m_Seek(seek), m_Len(len)
	{
		this->DoScan();
	}
	
private:
	virtual int GetBufLen() const override { return this->m_Len; }
	virtual bool CheckOne(const void *where) const override;
	
	const void *m_Seek;
	int m_Len;
};

class CMaskedScan : public IScan
{
public:
	CMaskedScan(ScanDir dir, ScanResults rtype, const IBounds& bounds, int align, const ByteBuf& seek, const ByteBuf& mask) :
		IScan(dir, rtype, bounds, align), m_Seek(seek), m_Mask(mask)
	{
		assert(this->m_Seek.GetSize() == this->m_Mask.GetSize());
		this->DoScan();
	}
	
private:
	virtual int GetBufLen() const override { return this->m_Seek.GetSize(); }
	virtual bool CheckOne(const void *where) const override;
	
	const ByteBuf& m_Seek;
	const ByteBuf& m_Mask;
};

class CStringScan : public IScan
{
public:
	CStringScan(ScanDir dir, ScanResults rtype, const IBounds& bounds, int align, const char *str) :
		IScan(dir, rtype, bounds, align), m_Str(str)
	{
		this->DoScan();
	}
	
private:
	virtual int GetBufLen() const override { return strlen(this->m_Str) + 1; }
	virtual bool CheckOne(const void *where) const override;
	
	const char *m_Str;
};

class CStringPrefixScan : public IScan
{
public:
	CStringPrefixScan(ScanDir dir, ScanResults rtype, const IBounds& bounds, int align, const char *str) :
		IScan(dir, rtype, bounds, align), m_Str(str)
	{
		this->DoScan();
	}
	
private:
	virtual int GetBufLen() const override { return strlen(this->m_Str) + 1; }
	virtual bool CheckOne(const void *where) const override;
	
	const char *m_Str;
};


#endif
