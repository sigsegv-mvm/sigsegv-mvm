#ifndef _INCLUDE_SIGSEGV_MEM_SCAN_H_
#define _INCLUDE_SIGSEGV_MEM_SCAN_H_


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
	virtual ~IScanner() {}
	
	const std::vector<const void *>& Matches() const { return this->m_Matches; }
	
	void Reset() { this->m_Matches.clear(); }
	bool IsDone() const { return this->m_bDone; }
	
	ScanResults GetResultsType() const { return this->m_RType; }
	int GetBufLen() const { return this->m_Len; }
	
	virtual void CheckOne(const void *where) = 0;
	
protected:
	IScanner(ScanResults rtype, int len) :
		m_RType(rtype), m_Len(len) {}
	
	void AddMatch(const void *match);
	
private:
	ScanResults m_RType;
	int m_Len;
	
	std::vector<const void *> m_Matches;
	bool m_bDone = false;
};

class CBasicScanner : public IScanner
{
public:
	CBasicScanner(ScanResults rtype, const void *seek, int len) :
		IScanner(rtype, len)
	{
		this->m_Seek = new uint8_t[len];
		memcpy(this->m_Seek, seek, len);
	}
	virtual ~CBasicScanner()
	{
		delete[] this->m_Seek;
	}
	
	virtual void CheckOne(const void *where) override;
	
private:
	uint8_t *m_Seek;
};

class CMaskedScanner : public IScanner
{
public:
	CMaskedScanner(ScanResults rtype, const ByteBuf& seek, const ByteBuf& mask) :
		IScanner(rtype, seek.GetSize()), m_Seek(seek), m_Mask(mask)
	{
		assert(this->m_Seek.GetSize() == this->m_Mask.GetSize());
	}
	
	virtual void CheckOne(const void *where) override;
	
private:
	const ByteBuf& m_Seek;
	const ByteBuf& m_Mask;
};

class CStringScanner : public IScanner
{
public:
	CStringScanner(ScanResults rtype, const char *str) :
		IScanner(rtype, strlen(str) + 1), m_Str(str) {}
	
	virtual void CheckOne(const void *where) override;
	
private:
	const char *m_Str;
};

class CStringPrefixScanner : public IScanner
{
public:
	CStringPrefixScanner(ScanResults rtype, const char *str) :
		IScanner(rtype, strlen(str) + 1), m_Str(str) {}
	
	virtual void CheckOne(const void *where) override;
	
private:
	const char *m_Str;
};


template<ScanDir DIR, int ALIGN>
class IScan
{
protected:
	IScan(const IBounds& bounds, const std::vector<IScanner *>& scanners) :
		m_Bounds(bounds), m_Scanners(scanners) {}
	virtual ~IScan()
	{
		for (auto scanner : this->m_Scanners) {
			delete scanner;
		}
	}
	
protected:
	void DoScan();
	
private:
	template<bool NEVER_DONE>
	void InternalScan();
	
	const IBounds& m_Bounds;
	std::vector<IScanner *> m_Scanners;
};

template<ScanDir DIR, int ALIGN>
class CSingleScan : public IScan<DIR, ALIGN>
{
public:
	CSingleScan(const IBounds& bounds, IScanner *scanner) :
		IScan<DIR, ALIGN>(bounds, { scanner }), m_Scanner(scanner)
	{
		this->DoScan();
	}
	
	const std::vector<const void *>& Matches() const { return this->m_Scanner->Matches(); }
	
private:
	IScanner *m_Scanner;
};

template<ScanDir DIR, int ALIGN>
class CMultiScan : public IScan<DIR, ALIGN>
{
public:
	CMultiScan(const IBounds& bounds, const std::vector<IScanner *>& scanners) :
		IScan<DIR, ALIGN>(bounds, scanners)
	{
		this->DoScan();
	}
};

template<ScanDir DIR, int ALIGN, int THREADS>
class CThreadedScan
{
public:
	CThreadedScan(const IBounds& bounds, const std::vector<IScanner *>& scanners) :
		m_Bounds(bounds)
	{
		for (auto scanner : scanners) {
			this->m_WorkToDo.push_back(scanner);
		}
		
		int n_threads = Min(THREADS, (int)scanners.size());
		
		std::vector<std::thread> threads;
		for (int i = 0; i < n_threads; ++i) {
			DevMsg("CThreadedScan: creating thread #%d\n", i);
			threads.emplace_back(&CThreadedScan<DIR, ALIGN, THREADS>::Worker, this, i);
		}
		
		for (auto& thread : threads) {
			DevMsg("CThreadedScan: join\n");
			thread.join();
		}
		DevMsg("CThreadedScan: done\n");
	}
	
private:
	void Worker(int id)
	{
//		DevMsg("CThreadedScan::Worker #%d BEGIN\n", id);
		
		IScanner *scanner;
		
		while ((scanner = this->GetWork()) != nullptr) {
//			DevMsg("CThreadedScan::Worker #%d SCAN BEGIN %08x\n", id, (uintptr_t)scanner);
			CSingleScan<DIR, ALIGN> *scan = new CSingleScan<DIR, ALIGN>(this->m_Bounds, scanner);
//			DevMsg("CThreadedScan::Worker #%d SCAN END   %08x\n", id, (uintptr_t)scanner);
			this->SubmitResult(scan);
		}
		
//		DevMsg("CThreadedScan::Worker #%d END\n", id);
	}
	
	IScanner *GetWork()
	{
		std::lock_guard<std::mutex> lock(this->m_MutexToDo);
		
		if (this->m_WorkToDo.empty()) {
			return nullptr;
		}
		
		IScanner *work = this->m_WorkToDo.front();
		this->m_WorkToDo.pop_front();
		return work;
	}
	
	void SubmitResult(CSingleScan<DIR, ALIGN> *result)
	{
		std::lock_guard<std::mutex> lock(this->m_MutexDone);
		
		this->m_WorkDone.emplace_back(std::unique_ptr<CSingleScan<DIR, ALIGN>>(result));
	}
	
	const IBounds& m_Bounds;
	
	std::mutex m_MutexToDo;
	std::list<IScanner *> m_WorkToDo;
	
	std::mutex m_MutexDone;
	std::vector<std::unique_ptr<CSingleScan<DIR, ALIGN>>> m_WorkDone;
};


namespace Scan
{
	const char *FindUniqueConstStr(const char *str);
}


template<ScanDir DIR, int ALIGN>
inline void IScan<DIR, ALIGN>::DoScan()
{
	bool never_done = true;
	for (auto scanner : this->m_Scanners) {
		if (scanner->GetResultsType() != ScanResults::ALL) {
			never_done = false;
			break;
		}
	}
	
	if (never_done) {
		this->InternalScan<true>();
	} else {
		this->InternalScan<false>();
	}
}

template<ScanDir DIR, int ALIGN> template<bool NEVER_DONE>
inline void IScan<DIR, ALIGN>::InternalScan()
{
	constexpr bool FWD = (DIR == ScanDir::FORWARD);
	
	for (auto scanner : this->m_Scanners) {
		scanner->Reset();
	}
	
	const uint8_t *p_low  = (const uint8_t *)this->m_Bounds.GetLowerBound();
	const uint8_t *p_high = (const uint8_t *)this->m_Bounds.GetUpperBound();
	
	const uint8_t *ptr = (FWD ? p_low : p_high - ALIGN);
	const uint8_t *end = (FWD ? p_high : p_low - ALIGN);
	
	uintptr_t rem = (uintptr_t)ptr % ALIGN;
	if (rem != 0) {
		if (FWD) {
			ptr += (ALIGN - rem);
		} else {
			ptr -= rem;
		}
	}
	
	constexpr int INCR = (FWD ? ALIGN : -ALIGN);
	
//	DevMsg("IScan::DoScan: ptr 0x%08x, end 0x%08x, incr %c0x%08x\n", (uintptr_t)ptr, (uintptr_t)end, (FWD ? '+' : '-'), ALIGN);
	
	/* use an array because std::vector is horrendously slow here */
	int num_scanners = this->m_Scanners.size();
	IScanner **scanners = new IScanner *[num_scanners];
	for (int i = 0; i < num_scanners; ++i) {
		scanners[i] = this->m_Scanners[i];
	}
	
	while (FWD ? (ptr < end) : (ptr > end)) {
//		bool all_done = false;
		
		for (int i = 0; i < num_scanners; ++i) {
			IScanner *scanner = scanners[i];
			
			if (!NEVER_DONE && scanner->IsDone()) {
				continue;
			}
			
			if (ptr + scanner->GetBufLen() <= p_high) {
				scanner->CheckOne(ptr);
#if 0
				
				if (scanner->IsDone()) {
					scanners.erase(scanner);
					if (scanners.empty()) {
						all_done = true;
					}
				}
#endif
			}
		}
		
//		if (all_done) {
//			break;
//		}
		
		ptr += INCR;
	}
	
	delete[] scanners;
}


#endif
