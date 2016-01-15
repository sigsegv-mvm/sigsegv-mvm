#include "util/scan.h"


CLibBounds::CLibBounds(Library lib)
{
	DynLibInfo info;
	memset(&info, 0x00, sizeof(info));
	assert(g_MemUtils.GetLibraryInfo(LibMgr::GetPtr(lib), info));
	
	this->m_AddrLow  = (const void *)info.baseAddress;
	this->m_AddrHigh = (const void *)((uintptr_t)info.baseAddress + info.memorySize);
}

CLibSegBounds::CLibSegBounds(Library lib, Segment seg)
{
	// TODO
	assert(false);
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
	
	while (ptr < end) {
		if ((uintptr_t)ptr % align == 0) {
			if (this->CheckOne(ptr)) {
				this->m_Matches.push_back((void *)ptr);
				
				if (this->m_RType == ScanResults::FIRST) {
					break;
				}
			}
		}
		
		++ptr;
	}
}

void IScan::DoScanReverse()
{
	int align = this->m_Align;
	
	const uint8_t *ptr = -1 + (const uint8_t *)this->m_Bounds.GetUpperBound() - this->GetBufLen();
	const uint8_t *end = -1 + (const uint8_t *)this->m_Bounds.GetLowerBound();
	
	DevMsg("IScan::DoScanReverse: ptr 0x%08x\n", (uintptr_t)ptr);
	DevMsg("IScan::DoScanReverse: end 0x%08x\n", (uintptr_t)end);
	
	while (ptr > end) {
		if ((uintptr_t)ptr % align == 0) {
			if (this->CheckOne(ptr)) {
//				DevMsg("  %08x   aligned MATCH\n", (uintptr_t)ptr);
				this->m_Matches.push_back((void *)ptr);
				
				if (this->m_RType == ScanResults::FIRST) {
					break;
				}
			} else {
//				DevMsg("  %08x   aligned NOPE\n", (uintptr_t)ptr);
			}
		} else {
//			DevMsg("  %08x unaligned\n", (uintptr_t)ptr);
		}
		
		--ptr;
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
