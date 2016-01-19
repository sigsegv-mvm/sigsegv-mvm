#include "util/scan.h"


CLibBounds::CLibBounds(Library lib)
{
	const LibInfo& info = LibMgr::GetInfo(lib);
	
	this->m_AddrLow  = (const void *)info.baseaddr;
	this->m_AddrHigh = (const void *)(info.baseaddr + info.len);
}

CLibSegBounds::CLibSegBounds(Library lib, const char *seg)
{
	const LibInfo& l_info = LibMgr::GetInfo(lib);
	const SegInfo& s_info = l_info.segs.at(seg);
	
	this->m_AddrLow  = (const void *)(l_info.baseaddr + s_info.off);
	this->m_AddrHigh = (const void *)(l_info.baseaddr + s_info.off + s_info.len);
}


void IScanner::AddMatch(const void *match)
{
	this->m_Matches.push_back(match);
	
	if (this->m_RType == ScanResults::FIRST) {
		this->m_bDone = true;
	}
}

void CBasicScanner::CheckOne(const void *where)
{
	if (memcmp(where, this->m_Seek, this->m_Len) == 0) {
		this->AddMatch(where);
	}
}

void CMaskedScanner::CheckOne(const void *where)
{
	int len = this->m_Seek.GetSize();
	
	for (int i = 0; i < len; ++i) {
		uint8_t b_mem  = *((uint8_t *)where + i);
		uint8_t b_seek = this->m_Seek[i];
		uint8_t b_mask = this->m_Mask[i];
		
		if ((b_mem & b_mask) != (b_seek & b_mask)) {
			return;
		}
	}
	
	this->AddMatch(where);
}

void CStringScanner::CheckOne(const void *where)
{
	if (strcmp((const char *)where, this->m_Str) == 0) {
		if (((const char *)where)[-1] == '\0') {
			this->AddMatch(where);
		}
	}
}

void CStringPrefixScanner::CheckOne(const void *where)
{
	if (strncmp((const char *)where, this->m_Str, strlen(this->m_Str)) == 0) {
		if (((const char *)where)[-1] == '\0') {
			this->AddMatch(where);
		}
	}
}


void IScan::DoScan()
{
	bool fwd;
	switch (this->m_Dir) {
	case ScanDir::FORWARD:
		fwd = true;
		break;
	case ScanDir::REVERSE:
		fwd = false;
		break;
	default:
		assert(false);
	}
	
	for (auto scanner : this->m_Scanners) {
		scanner->Reset();
	}
	
	int align = this->m_Align;
	
	const uint8_t *p_low  = (const uint8_t *)this->m_Bounds.GetLowerBound();
	const uint8_t *p_high = (const uint8_t *)this->m_Bounds.GetUpperBound();
	
	const uint8_t *ptr = (fwd ? p_low : p_high - align);
	const uint8_t *end = (fwd ? p_high : p_low - align);
	
	uintptr_t rem = (uintptr_t)ptr % align;
	if (rem != 0) {
		if (fwd) {
			ptr += (align - rem);
		} else {
			ptr -= rem;
		}
	}
	
	int incr = (fwd ? align : -align);
	
	DevMsg("IScan::DoScan: ptr 0x%08x, end 0x%08x, incr %c0x%08x\n", (uintptr_t)ptr, (uintptr_t)end, (fwd ? '+' : '-'), align);
	
	/* use an array because std::vector is horrendously slow here */
	int num_scanners = this->m_Scanners.size();
	IScanner **scanners = new IScanner *[num_scanners];
	for (int i = 0; i < num_scanners; ++i) {
		scanners[i] = this->m_Scanners[i];
	}
	
	while (fwd ? (ptr < end) : (ptr > end)) {
//		bool all_done = false;
		
		for (int i = 0; i < num_scanners; ++i) {
			IScanner *scanner = scanners[i];
			
			if (scanner->IsDone()) {
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
		
		ptr += incr;
	}
	
	delete[] scanners;
}
