#include "mem/scan.h"


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
	if (memcmp(where, this->m_Seek, this->GetBufLen()) == 0) {
		this->AddMatch(where);
	}
}

void CMaskedScanner::CheckOne(const void *where)
{
	int len = this->GetBufLen();
	
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
		this->AddMatch(where);
	}
}

void CStringPrefixScanner::CheckOne(const void *where)
{
	if (strncmp((const char *)where, this->m_Str, this->GetBufLen() - 1) == 0) {
		this->AddMatch(where);
	}
}


namespace Scan
{
	const char *FindUniqueConstStr(const char *str)
	{
		CSingleScan<ScanDir::FORWARD, 1> scan(CLibSegBounds(Library::SERVER, ".rdata"), new CStringScanner(ScanResults::ALL, str));
		
		if (scan.Matches().size() == 1) {
			return (const char *)scan.Matches()[0];
		}
		
		/* get aggressive: try to exclude matches that are probably a suffix */
		if (scan.Matches().size() > 1) {
			std::vector<const char *> matches;
			for (auto match : scan.Matches()) {
				const char *m_str = (const char *)match;
				if (m_str[-1] == '\0') {
					matches.push_back(m_str);
				}
			}
			
			if (matches.size() == 1) {
				return matches[0];
			}
		}
		
		return nullptr;
	}
}
