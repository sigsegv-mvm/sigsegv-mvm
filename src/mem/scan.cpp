#include "mem/scan.h"


CLibBounds::CLibBounds(Library lib)
{
	if (LibMgr::HaveLib(lib)) {
		const LibInfo& lib_info = LibMgr::GetInfo(lib);
		
		this->m_AddrLow  = reinterpret_cast<const void *>(lib_info.AddrBegin());
		this->m_AddrHigh = reinterpret_cast<const void *>(lib_info.AddrEnd());
	}
}

CLibSegBounds::CLibSegBounds(Library lib, Segment seg)
{
	const SegInfo& seg_info = LibMgr::GetInfo(lib).GetSeg(seg);
	
	this->m_AddrLow  = reinterpret_cast<const void *>(seg_info.AddrBegin());
	this->m_AddrHigh = reinterpret_cast<const void *>(seg_info.AddrEnd());
}


namespace Scan
{
	const char *FindUniqueConstStr(Library lib, const char *str)
	{
		using StrScanner = CStringScanner<ScanDir::FORWARD, ScanResults::ALL, 1>;
		
		CScan<StrScanner> scan(CLibSegBounds(lib, Segment::RODATA), str);
		if (scan.ExactlyOneMatch()) {
			return (const char *)scan.FirstMatch();
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
	
	// TODO: use a CMultiScan in FindFuncPrologue
	const void *FindFuncPrologue(const void *p_in_func)
	{
		using PrologueScanner = CBasicScanner<ScanDir::REVERSE, ScanResults::FIRST, 0x10>;
		
		/* normal EBP frame */
		constexpr uint8_t ebp_prologue[] = {
			0x55,       // +0000  push ebp
			0x8b, 0xec, // +0001  mov ebp,esp
		};
		CScan<PrologueScanner> scan_ebp(CAddrOffBounds(p_in_func, -0x10000), (const void *)ebp_prologue, sizeof(ebp_prologue));
		
		/* uncommon EBX/EBP frame (for 16-byte alignment) */
		constexpr uint8_t ebx_prologue[] = {
			0x53,                   // +0000  push ebx
			0x8b, 0xdc,             // +0001  mov ebx,esp
			0x83, 0xec, 0x08,       // +0003  sub esp,8
			0x83, 0xe4, 0xf0,       // +0006  and esp,0xfffffff0
			0x83, 0xc4, 0x04,       // +0009  add esp,4
			0x55,                   // +000C  push ebp
			0x8b, 0x6b, 0x04,       // +000D  mov ebp,[ebx+4]
			0x89, 0x6c, 0x24, 0x04, // +0010  mov [esp+4],ebp
			0x8b, 0xec,             // +0014  mov ebp,esp
		};
		CScan<PrologueScanner> scan_ebx(CAddrOffBounds(p_in_func, -0x10000), (const void *)ebx_prologue, sizeof(ebx_prologue));
		
		bool found_ebp = (scan_ebp.ExactlyOneMatch());
		bool found_ebx = (scan_ebx.ExactlyOneMatch());
		
		if (found_ebp && found_ebx) {
			return Max(scan_ebp.FirstMatch(), scan_ebx.FirstMatch());
		} else if (found_ebp) {
			return scan_ebp.FirstMatch();
		} else if (found_ebx) {
			return scan_ebx.FirstMatch();
		} else {
			return nullptr;
		}
	}
	
	// TODO: make a convenience class which takes a std::vector<const char *>
	// and exposes a std::map<std::string, const void *>, which internally does
	// a CMultiScan for string constants and sets the map appropriately
	// (use this in CAddr_pszWpnEntTranslationList)
}
