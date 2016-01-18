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


class IScanner
{
public:
	const std::vector<const void *>& Matches() const { return this->m_Matches; }
	
	void Reset() { this->m_Matches.clear(); }
	bool IsDone() const { return this->m_bDone; }
	
	virtual void CheckOne(const void *where) = 0;
	virtual int GetBufLen() const = 0;
	
protected:
	IScanner(ScanResults rtype) :
		m_RType(rtype) {}
	
	void AddMatch(const void *match);
	
private:
	ScanResults m_RType;
	
	std::vector<const void *> m_Matches;
	bool m_bDone = false;
};

class CBasicScanner : public IScanner
{
public:
	CBasicScanner(ScanResults rtype, const void *seek, int len) :
		IScanner(rtype), m_Seek(seek), m_Len(len) {}
	
	virtual int GetBufLen() const override { return this->m_Len; }
	virtual void CheckOne(const void *where) override;
	
private:
	const void *m_Seek;
	int m_Len;
};

class CMaskedScanner : public IScanner
{
public:
	CMaskedScanner(ScanResults rtype, const ByteBuf& seek, const ByteBuf& mask) :
		IScanner(rtype), m_Seek(seek), m_Mask(mask)
	{
		assert(this->m_Seek.GetSize() == this->m_Mask.GetSize());
	}
	
	virtual int GetBufLen() const override { return this->m_Seek.GetSize(); }
	virtual void CheckOne(const void *where) override;
	
private:
	const ByteBuf& m_Seek;
	const ByteBuf& m_Mask;
};

class CStringScanner : public IScanner
{
public:
	CStringScanner(ScanResults rtype, const char *str) :
		IScanner(rtype), m_Str(str) {}
	
	virtual int GetBufLen() const override { return strlen(this->m_Str) + 1; }
	virtual void CheckOne(const void *where) override;
	
private:
	const char *m_Str;
};

class CStringPrefixScanner : public IScanner
{
public:
	CStringPrefixScanner(ScanResults rtype, const char *str) :
		IScanner(rtype), m_Str(str) {}
	
	virtual int GetBufLen() const override { return strlen(this->m_Str) + 1; }
	virtual void CheckOne(const void *where) override;
	
private:
	const char *m_Str;
};


class IScan
{
protected:
	IScan(ScanDir dir, const IBounds& bounds, int align, const std::vector<IScanner *>& scanners) :
		m_Dir(dir), m_Bounds(bounds), m_Align(align), m_Scanners(scanners) {}
	virtual ~IScan()
	{
		for (auto scanner : this->m_Scanners) {
			delete scanner;
		}
	}
	
protected:
	void DoScan();
	
private:
	ScanDir m_Dir;
	const IBounds& m_Bounds;
	int m_Align;
	std::vector<IScanner *> m_Scanners;
};

class CSingleScan : public IScan
{
public:
	CSingleScan(ScanDir dir, const IBounds& bounds, int align, IScanner *scanner) :
		IScan(dir, bounds, align, { scanner }), m_Scanner(scanner)
	{
		this->DoScan();
	}
	
	const std::vector<const void *>& Matches() const { return this->m_Scanner->Matches(); }
	
private:
	IScanner *m_Scanner;
};

class CMultiScan : public IScan
{
public:
	CMultiScan(ScanDir dir, const IBounds& bounds, int align, const std::vector<IScanner *>& scanners) :
		IScan(dir, bounds, align, scanners)
	{
		this->DoScan();
	}
};


#endif
