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


void IScan::DoScan()
{
	this->m_Matches.clear();
	
	switch (this->m_Dir) {
	case ScanDir::FORWARD:
		this->DoScanForward();
		break;
	case ScanDir::REVERSE:
		this->DoScanReverse();
		break;
	default:
		assert(false);
	}
}


void IScan::DoScanForward()
{
	int align = this->m_Align;
	
	const uint8_t *ptr = (const uint8_t *)this->m_Bounds.GetLowerBound();
	const uint8_t *end = (const uint8_t *)this->m_Bounds.GetUpperBound() - this->GetBufLen();
	
	DevMsg("IScan::DoScanForward: ptr 0x%08x\n", (uintptr_t)ptr);
	DevMsg("IScan::DoScanForward: end 0x%08x\n", (uintptr_t)end);
	
	uintptr_t rem = (uintptr_t)ptr % align;
	if (rem != 0) {
		ptr += (align - rem);
	}
	
	while (ptr < end) {
		if (this->CheckOne(ptr)) {
			this->m_Matches.push_back((void *)ptr);
			
			if (this->m_RType == ScanResults::FIRST) {
				break;
			}
		}
		
		ptr += align;
	}
}

void IScan::DoScanReverse()
{
	int align = this->m_Align;
	
	const uint8_t *ptr = -1 + (const uint8_t *)this->m_Bounds.GetUpperBound() - this->GetBufLen();
	const uint8_t *end = -1 + (const uint8_t *)this->m_Bounds.GetLowerBound();
	
	DevMsg("IScan::DoScanReverse: ptr 0x%08x\n", (uintptr_t)ptr);
	DevMsg("IScan::DoScanReverse: end 0x%08x\n", (uintptr_t)end);
	
	uintptr_t rem = (uintptr_t)ptr % align;
	if (rem != 0) {
		ptr -= rem;
	}
	
	while (ptr > end) {
		if (this->CheckOne(ptr)) {
//			DevMsg("  %08x   MATCH\n", (uintptr_t)ptr);
			this->m_Matches.push_back((void *)ptr);
			
			if (this->m_RType == ScanResults::FIRST) {
				break;
			}
		} else {
//			DevMsg("  %08x   NOPE\n", (uintptr_t)ptr);
		}
		
		ptr -= align;
	}
}


bool CBasicScan::CheckOne(const void *where) const
{
	return (memcmp(where, this->m_Seek, this->m_Len) == 0);
}

bool CMaskedScan::CheckOne(const void *where) const
{
	int len = this->m_Seek.GetSize();
	
	for (int i = 0; i < len; ++i) {
		uint8_t b_mem  = *((uint8_t *)where + i);
		uint8_t b_seek = this->m_Seek[i];
		uint8_t b_mask = this->m_Mask[i];
		
		if ((b_mem & b_mask) != (b_seek & b_mask)) {
			return false;
		}
	}
	
	return true;
}

bool CStringScan::CheckOne(const void *where) const
{
	return (strcmp((const char *)where, this->m_Str) == 0);
}

bool CStringPrefixScan::CheckOne(const void *where) const
{
	return (strncmp((const char *)where, this->m_Str, strlen(this->m_Str)) == 0);
}
