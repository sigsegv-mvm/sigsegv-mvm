#include "util/rtti.h"
#include "library.h"
#include "mem/scan.h"


#if defined __GNUC__
#warning TODO: find/replace all cases of "[VT]" and "[RTTI]"
#warning TODO: remove all cases of GetRTTIAddrName
#endif


#if defined _MSC_VER
class Prof
{
public:
	static void Begin()
	{
		s_bFail = false;
		
		if (!QueryPerformanceFrequency(&s_Freq)) s_bFail = true;
		if (!QueryPerformanceCounter(&s_Count1)) s_bFail = true;
	}
	static void End(const char *msg)
	{
		if (!QueryPerformanceCounter(&s_Count2)) s_bFail = true;
		
		if (s_bFail) {
			DevMsg("PROF %s: failed\n", msg);
		} else {
			int64_t freq  = s_Freq.QuadPart;
			int64_t delta = s_Count2.QuadPart - s_Count1.QuadPart;
			
			double sec = (double)delta / (double)freq;
			double msec = sec * 1000.0;
			
			DevMsg("PROF %s: %.3f ms\n", msg, msec);
		}
	}
	
private:
	static bool s_bFail;
	static LARGE_INTEGER s_Freq;
	static LARGE_INTEGER s_Count1;
	static LARGE_INTEGER s_Count2;
};
bool Prof::s_bFail;
LARGE_INTEGER Prof::s_Freq;
LARGE_INTEGER Prof::s_Count1;
LARGE_INTEGER Prof::s_Count2;
#endif


namespace RTTI
{
	// Windows: str typeinfo(T).raw_name() => _TypeDescriptor*
	// Linux:   str typeinfo(T).name()     => abi::__class_type_info*
	static std::map<std::string, const rtti_t *> s_RTTI;
	
	// Windows: str typeinfo(T).raw_name() => void **
	// Linux:   str typeinfo(T).name()     => void **
	static std::map<std::string, const void **> s_VT;
	
	
	void PreLoad()
	{
		DevMsg("RTTI::PreLoad BEGIN\n");
		
#if defined __GNUC__
		
		LibMgr::ForEachSym(Library::SERVER,
		[](Symbol *sym)
		{
			const char *buf = sym->buffer();
			char name[4096];
			
			if (sym->length >= 4 && memcmp(buf, "_ZT", 3) == 0) {
				bool is_rtti = (buf[3] == 'I');
				bool is_vt   = (buf[3] == 'V');
				
				if (is_rtti || is_vt) {
					size_t len = sym->length - 4;
					len = Min(len, sizeof(name) - 1);
					
					memcpy(name, buf + 4, len);
					name[sym->length] = '\0';
					
					if (is_rtti) {
						std::string key(name);
						auto addr = (const rtti_t *)(sym->address);
						
						if (s_RTTI.find(key) != s_RTTI.end()) {
							DevWarning("RTTI::PreLoad: duplicate symbol \"_ZTI%s\"\n", name);
						} else {
							s_RTTI[key] = addr;
						//	DevMsg("RTTI: %08x \"%s\"\n", (uintptr_t)addr, name);
						}
					} else if (is_vt) {
						std::string key(name);
						auto addr = (const void **)((uintptr_t)(sym->address) + offsetof(vtable, vfptrs));
						
						if (s_VT.find(key) != s_VT.end()) {
							DevWarning("RTTI::PreLoad: duplicate symbol \"_ZTV%s\"\n", name);
						} else {
							s_VT[key] = addr;
						//	DevMsg("VT: %08x \"%s\"\n", (uintptr_t)addr, name);
						}
					}
				}
			}
		}
		);
		
#elif defined _MSC_VER
		
		int n_total = 0;
		int n_skip  = 0;
		int n_add   = 0;
		
		std::vector<std::regex> exclude;
		exclude.emplace_back(R"(^.*@CryptoPP@@$)");
		exclude.emplace_back(R"(^.*@protobuf@google@@$)");
		exclude.emplace_back(R"(^.*@GCSDK@@$)");
		exclude.emplace_back(R"(^\.\?AVCGC.*$)");
		exclude.emplace_back(R"(^\.\?AV\?\$CGC.*$)");
		exclude.emplace_back(R"(^\.\?AVCMsg.*$)");
		exclude.emplace_back(R"(^\.\?AVCSO.*$)");
		exclude.emplace_back(R"(^\.\?AV\?\$CUtl\w+DataOps.*$)");
		exclude.emplace_back(R"(^\.\?AV\?\$CEntityFactory@.*$)");
		exclude.emplace_back(R"(^\.\?AVNetworkVar_.*$)");
		
		Prof::Begin();
		CSingleScan<ScanDir::FORWARD, 1> scan1(CLibSegBounds(Library::SERVER, ".data"), new CStringPrefixScanner(ScanResults::ALL, ".?AV"));
		Prof::End("TD scan");
		Prof::Begin();
		for (auto match : scan1.Matches()) {
			auto name = (const char *)match;
			auto addr = (const rtti_t *)((uintptr_t)match - offsetof(_TypeDescriptor, name));
			
			++n_total;
			bool skip = false;
			for (auto& filter : exclude) {
				if (std::regex_match(name, filter, std::regex_constants::match_any)) {
					skip = true;
					break;
				}
			}
			if (skip) {
				++n_skip;
				continue;
			}
			++n_add;
			
			std::string key(name);
			if (s_RTTI.find(key) != s_RTTI.end()) {
			//	DevWarning("RTTI::PreLoad: duplicate RTTI str \"%s\"\n", name);
			} else {
				s_RTTI[key] = addr;
			//	DevMsg("\"%s\" TD @ %08x\n", name, (uintptr_t)s_RTTI[key]);
			}
		}
		DevMsg("RTTI::PreLoad:\n"
			"total %d\n"
			"skip  %d\n"
			"add   %d\n",
			n_total, n_skip, n_add);
		Prof::End("TD post");
		
		
		Prof::Begin();
		std::map<IScanner *, std::string> scannermap_COL;
		std::vector<IScanner *> scanners_COL;
		for (const auto& pair : s_RTTI) {
			auto name = pair.first;
			auto p_TD = pair.second;
			
			__RTTI_CompleteObjectLocator seek_COL = {
				0x00000000,
				0x00000000,
				0x00000000,
				const_cast<_TypeDescriptor *>(p_TD),
			};
			
			IScanner *scanner = new CBasicScanner(ScanResults::FIRST, (const void *)&seek_COL, 0x10);
			
			scanners_COL.push_back(scanner);
			scannermap_COL[scanner] = name;
		}
		Prof::End("COL pre");
		Prof::Begin();
		//CMultiScan<ScanDir::REVERSE, 4> scan_COL(CLibSegBounds(Library::SERVER, ".rdata"), scanners_COL);
		CThreadedScan<ScanDir::REVERSE, 4, 4> scan_COL(CLibSegBounds(Library::SERVER, ".rdata"), scanners_COL);
		Prof::End("COL scan");
		
		Prof::Begin();
		std::map<std::string, const __RTTI_CompleteObjectLocator *> results_COL;
		for (auto scanner : scanners_COL) {
			auto& name = scannermap_COL[scanner];
			
			if (scanner->Matches().size() != 1) {
			//	DevMsg("RTTI::PreLoad: %u TD refs for \"%s\"\n", scanner->Matches().size(), name.c_str());
				continue;
			}
			
			results_COL[name] = (__RTTI_CompleteObjectLocator *)scanner->Matches()[0];
		//	DevMsg("\"%s\" COL @ %08x\n", name.c_str(), (uintptr_t)results_COL[name]);
		}
		Prof::End("COL post");
		
		
		Prof::Begin();
		std::map<IScanner *, std::string> scannermap_VT;
		std::vector<IScanner *> scanners_VT;
		for (const auto& pair : results_COL) {
			auto name  = pair.first;
			auto p_COL = pair.second;
			
			IScanner *scanner = new CBasicScanner(ScanResults::FIRST, (const void *)&p_COL, 0x4);
			
			scanners_VT.push_back(scanner);
			scannermap_VT[scanner] = name;
		}
		Prof::End("VT pre");
		Prof::Begin();
		//CMultiScan<ScanDir::FORWARD, 4> scan_VT(CLibSegBounds(Library::SERVER, ".rdata"), scanners_VT);
		CThreadedScan<ScanDir::FORWARD, 4, 4> scan_VT(CLibSegBounds(Library::SERVER, ".rdata"), scanners_VT);
		Prof::End("VT scan");
		
		Prof::Begin();
		for (auto scanner : scanners_VT) {
			auto& name = scannermap_VT[scanner];
			
			if (scanner->Matches().size() != 1) {
			//	DevMsg("RTTI::PreLoad: %u COL refs for \"%s\"\n", scanner->Matches().size(), name.c_str());
				continue;
			}
			
			s_VT[name] = (const void **)((uintptr_t)scanner->Matches()[0] + 0x4);
		//	DevMsg("\"%s\" VT @ %08x\n", name.c_str(), (uintptr_t)s_VT[name]);
		}
		Prof::End("VT post");
		
#endif
		
		
		DevMsg("RTTI::PreLoad: found %u RTTI\n", s_RTTI.size());
		DevMsg("RTTI::PreLoad: found %u VT\n", s_VT.size());
		
		// RTTI
		// Linux: find all symbols of format "_ZTI"
		// Windows: find typename strings, find refs, subtract 8 bytes
		
		// VTable
		// Linux: find all symbols of format "_ZTV", add 8 bytes
		// Windows: do some fancy backtracking from RTTI info
	}
	
	
	const rtti_t *GetRTTI(const char *name)
	{
		auto it = s_RTTI.find(std::string(name));
		if (it == s_RTTI.end()) {
			DevMsg("RTTI::GetRTTI FAIL: no RTTI addr for name \"%s\"\n", name);
			return nullptr;
		}
		
		return (*it).second;
	}
	
	const void **GetVTable(const char *name)
	{
		auto it = s_VT.find(std::string(name));
		if (it == s_VT.end()) {
			DevMsg("RTTI::GetVTable FAIL: no VT addr for name \"%s\"\n", name);
			return nullptr;
		}
		
		return (*it).second;
	}
}
