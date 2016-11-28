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


// BOUNDS ======================================================================

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
	CLibSegBounds(Library lib, Segment seg);
	
	virtual const void *GetLowerBound() const override { return this->m_AddrLow; }
	virtual const void *GetUpperBound() const override { return this->m_AddrHigh; }
	
private:
	const void *m_AddrLow  = nullptr;
	const void *m_AddrHigh = nullptr;
};


// SCANNERS ====================================================================

template<ScanDir DIR, ScanResults RTYPE, int ALIGN>
class IScanner
{
public:
	virtual ~IScanner() {}
	
	static constexpr ScanDir GetDir()             { return DIR; }
	static constexpr ScanResults GetResultsType() { return RTYPE; }
	static constexpr int GetAlign()               { return ALIGN; }
	
	const IBounds& GetBounds() const { return this->m_Bounds; }
	int GetLen() const               { return this->m_Len; }
	
	const std::vector<const void *>& Matches() const { return this->m_Matches; }
	const void *FirstMatch() const                   { return this->m_Matches[0]; }
	bool ExactlyOneMatch() const                     { return (this->m_Matches.size() == 1); }
	
protected:
	IScanner(const IBounds& bounds, int len) :
		m_Bounds(bounds), m_Len(len) {}
	
	void AddMatch(const void *match) { this->m_Matches.push_back(match); }
	
private:
	const IBounds& m_Bounds;
	int m_Len;
	
	std::vector<const void *> m_Matches;
};


template<ScanDir DIR, ScanResults RTYPE, int ALIGN>
class CBasicScanner : public IScanner<DIR, RTYPE, ALIGN>
{
public:
	CBasicScanner(const IBounds& bounds, const void *seek, int len) :
		IScanner<DIR, RTYPE, ALIGN>(bounds, len)
	{
		this->m_Seek = new uint8_t[len];
		memcpy(this->m_Seek, seek, len);
	}
	virtual ~CBasicScanner()
	{
		delete[] this->m_Seek;
	}
	
	bool CheckOne(const void *where);
	
private:
	uint8_t *m_Seek;
};

template<ScanDir DIR, ScanResults RTYPE, int ALIGN>
inline bool CBasicScanner<DIR, RTYPE, ALIGN>::CheckOne(const void *where)
{
	if (memcmp(where, this->m_Seek, this->GetLen()) == 0) {
		this->AddMatch(where);
		return true;
	} else {
		return false;
	}
}


template<ScanDir DIR, ScanResults RTYPE, typename T, int ALIGN = 1>
class CTypeScanner : public CBasicScanner<DIR, RTYPE, ALIGN>
{
public:
	CTypeScanner(const IBounds& bounds, const T& seek) :
		CBasicScanner<DIR, RTYPE, ALIGN>(bounds, reinterpret_cast<const void *>(&seek), sizeof(T)) {}
	virtual ~CTypeScanner() {}
};


template<ScanDir DIR, ScanResults RTYPE, typename T>
class CAlignedTypeScanner : public CTypeScanner<DIR, RTYPE, T, sizeof(T)>
{
public:
	CAlignedTypeScanner(const IBounds& bounds, const T& seek) :
		CTypeScanner<DIR, RTYPE, T, sizeof(T)>(bounds, seek) {}
	virtual ~CAlignedTypeScanner() {}
};


template<ScanDir DIR, ScanResults RTYPE, int ALIGN>
class CStringScanner : public IScanner<DIR, RTYPE, ALIGN>
{
public:
	CStringScanner(const IBounds& bounds, const char *str) :
		IScanner<DIR, RTYPE, ALIGN>(bounds, strlen(str) + 1), m_Str(str) {}
	
	bool CheckOne(const void *where);
	
private:
	const char *m_Str;
};

template<ScanDir DIR, ScanResults RTYPE, int ALIGN>
inline bool CStringScanner<DIR, RTYPE, ALIGN>::CheckOne(const void *where)
{
	if (strcmp((const char *)where, this->m_Str) == 0) {
		this->AddMatch(where);
		return true;
	} else {
		return false;
	}
}


template<ScanDir DIR, ScanResults RTYPE, int ALIGN>
class CStringPrefixScanner : public IScanner<DIR, RTYPE, ALIGN>
{
public:
	CStringPrefixScanner(const IBounds& bounds, const char *str) :
		IScanner<DIR, RTYPE, ALIGN>(bounds, strlen(str) + 1), m_Str(str) {}
	
	bool CheckOne(const void *where);
	
private:
	const char *m_Str;
};

template<ScanDir DIR, ScanResults RTYPE, int ALIGN>
inline bool CStringPrefixScanner<DIR, RTYPE, ALIGN>::CheckOne(const void *where)
{
	if (strncmp((const char *)where, this->m_Str, this->GetLen() - 1) == 0) {
		this->AddMatch(where);
		return true;
	} else {
		return false;
	}
}


template<ScanDir DIR, ScanResults RTYPE, int ALIGN>
class CMaskedScanner : public IScanner<DIR, RTYPE, ALIGN>
{
public:
	CMaskedScanner(const IBounds& bounds, const ByteBuf& seek, const ByteBuf& mask) :
		IScanner<DIR, RTYPE, ALIGN>(bounds, seek.GetSize()), m_Seek(seek), m_Mask(mask)
	{
		assert(this->m_Seek.GetSize() == this->m_Mask.GetSize());
	}
	
	bool CheckOne(const void *where);
	
private:
	const ByteBuf& m_Seek;
	const ByteBuf& m_Mask;
};

template<ScanDir DIR, ScanResults RTYPE, int ALIGN>
inline bool CMaskedScanner<DIR, RTYPE, ALIGN>::CheckOne(const void *where)
{
	int len = this->GetLen();
	
	for (int i = 0; i < len; ++i) {
		uint8_t b_mem  = *((uint8_t *)where + i);
		uint8_t b_seek = this->m_Seek[i];
		uint8_t b_mask = this->m_Mask[i];
		
		if ((b_mem & b_mask) != (b_seek & b_mask)) {
			return false;
		}
	}
	
	this->AddMatch(where);
	return true;
}


template<ScanDir DIR, ScanResults RTYPE, int ALIGN>
class CCallScanner : public IScanner<DIR, RTYPE, ALIGN>
{
public:
	CCallScanner(const IBounds& bounds, uint32_t target) :
		IScanner<DIR, RTYPE, ALIGN>(bounds, sizeof(target)), m_Target(target) {}
	
	bool CheckOne(const void *where);
	
private:
	const uint32_t m_Target;
};

template<ScanDir DIR, ScanResults RTYPE, int ALIGN>
inline bool CCallScanner<DIR, RTYPE, ALIGN>::CheckOne(const void *where)
{
	auto p_opcode = reinterpret_cast<const uint8_t *>(where);
	auto p_offset = reinterpret_cast<const uint32_t *>((uintptr_t)where + 1);
	
	if (*p_opcode == 0xe8) {
		uint32_t rel = this->m_Target - ((uintptr_t)where + 5);
		if (*p_offset == rel) {
			this->AddMatch(where);
			return true;
		}
	}
	
	return false;
}


// SCANS =======================================================================

template<class SCANNER>
class CScan
{
public:
	CScan(SCANNER *scanner) :
		m_Scanner(scanner)
	{
		this->DoScan();
	}
	template<typename... ARGS>
	CScan(ARGS&&... args)
	{
		this->m_Scanner = new SCANNER(std::forward<ARGS>(args)...);
		this->DoScan();
	}
	~CScan()
	{
		delete this->m_Scanner;
	}
	
	void DoScan();
	
	const std::vector<const void *>& Matches() const { return this->m_Scanner->Matches(); }
	const void *FirstMatch() const                   { return this->m_Scanner->FirstMatch(); }
	bool ExactlyOneMatch() const                     { return this->m_Scanner->ExactlyOneMatch(); }
	
private:
	SCANNER *m_Scanner = nullptr;
};

template<class SCANNER>
inline void CScan<SCANNER>::DoScan()
{
	SCANNER *scanner = this->m_Scanner;
	
	constexpr ScanDir DIR       = SCANNER::GetDir();
	constexpr ScanResults RTYPE = SCANNER::GetResultsType();
	constexpr int ALIGN         = SCANNER::GetAlign();
	
	constexpr bool FWD = (DIR == ScanDir::FORWARD);
	
	const uint8_t *p_low  = (const uint8_t *)scanner->GetBounds().GetLowerBound();
	const uint8_t *p_high = (const uint8_t *)scanner->GetBounds().GetUpperBound();
	
	p_high -= scanner->GetLen();
	
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
	
	while (FWD ? (ptr <= end) : (ptr >= end)) {
		bool matched = scanner->CheckOne((const void *)ptr);
		
		if (RTYPE == ScanResults::FIRST && matched) {
			break;
		}
		
		ptr += INCR;
	}
}


// TODO: make CMultiScan threads globally shared/persistent instead of creating/deleting
// TODO: make heterogeneous CMultiScans possible by using RTTI

template<class SCANNER>
class CMultiScan
{
public:
	CMultiScan(const std::vector<SCANNER *>& scanners)
	{
		for (auto scanner : scanners) {
			this->m_Scanners.push_back(scanner);
		}
		
		this->DoScans();
	}
	
private:
	void DoScans();
	
	void Worker(int id);
	SCANNER *GetTask();
	void SubmitTask(CScan<SCANNER> *result);
	
	/* tasks in */
	std::mutex m_MutexIn;
	std::list<SCANNER *> m_Scanners;
	
	/* tasks out */
	std::mutex m_MutexOut;
	std::vector<std::unique_ptr<CScan<SCANNER>>> m_Scans;
};

template<class SCANNER>
inline void CMultiScan<SCANNER>::DoScans()
{
//	DevMsg("CMultiScan: BEGIN\n");
	
	unsigned int n_threads = Max(1U, std::thread::hardware_concurrency());
	n_threads = Min(n_threads, this->m_Scanners.size());
	
	std::vector<std::thread> threads;
	for (unsigned int i = 0; i < n_threads; ++i) {
//		DevMsg("CMultiScan: SPAWN T#%d\n", i);
		threads.emplace_back(&CMultiScan<SCANNER>::Worker, this, i);
	}
	
	for (unsigned int i = 0; i < n_threads; ++i) {
		threads[i].join();
//		DevMsg("CMultiScan: JOIN  T#%d\n", i);
	}
	
//	DevMsg("CMultiScan: END\n");
}

template<class SCANNER>
inline void CMultiScan<SCANNER>::Worker(int id)
{
//	DevMsg("CMultiScan: W#%d BEGIN\n", id);
	
	SCANNER *scanner;
	while ((scanner = this->GetTask()) != nullptr) {
//		DevMsg("CMultiScan: W#%d SCAN BEGIN %08x\n", id, (uintptr_t)scanner);
		auto scan = new CScan<SCANNER>(scanner);
//		DevMsg("CMultiScan: W#%d SCAN END   %08x\n", id, (uintptr_t)scanner);
		this->SubmitTask(scan);
	}
	
//	DevMsg("CMultiScan: W#%d END\n", id);
}

template<class SCANNER>
inline SCANNER *CMultiScan<SCANNER>::GetTask()
{
	std::lock_guard<std::mutex> lock(this->m_MutexIn);
	
	if (this->m_Scanners.empty()) {
		return nullptr;
	}
	
	SCANNER *scanner = this->m_Scanners.front();
	this->m_Scanners.pop_front();
	return scanner;
}

template<class SCANNER>
inline void CMultiScan<SCANNER>::SubmitTask(CScan<SCANNER> *result)
{
	std::lock_guard<std::mutex> lock(this->m_MutexOut);
	
	this->m_Scans.emplace_back(result);
}


namespace Scan
{
	const char *FindUniqueConstStr(Library lib, const char *str);
	const void *FindFuncPrologue(const void *p_in_func);
}


#endif
