#include "addr/addr.h"
#include "util/util.h"
#include "abi.h"


class CAddr_VTable : public CAddr_Sym
{
public:
	virtual bool FindAddrLinux(uintptr_t& addr) const override
	{
		bool result = CAddr_Sym::FindAddrLinux(addr);
		
		if (result) {
			addr += offsetof(vtable, vfptrs);
		}
		
		return result;
	}
	
	virtual bool FindAddrWin(uintptr_t& addr) const override
	{
		/* STEP 1: get ptr to _TypeDescriptor by finding typeinfo name string */
		
		const char *str = this->GetWinRTTIStr();
		
		std::vector<void *> str_matches;
		Scan::FindString(this->GetLibrary(), str, str_matches);
		
		if (str_matches.size() == 0) {
			DevMsg("CAddr_VTable::FindAddrWin: could not find RTTI string \"%s\"\n", str);
			return false;
		}
		if (str_matches.size() > 1) {
			DevMsg("CAddr_VTable::FindAddrWin: found multiple string matches for \"%s\"\n", str);
		}
		
		auto *p_TD = (_TypeDescriptor *)((uintptr_t)str_matches[0] - offsetof(_TypeDescriptor, name));
		
		
		/* STEP 2: get ptr to __RTTI_CompleteObjectLocator by finding references to the _TypeDescriptor */
		
		__RTTI_CompleteObjectLocator seek_COL = {
			0x00000000,
			0x00000000,
			0x00000000,
			p_TD,
		};
		
		std::vector<void *> refs_TD;
		Scan::FindFast(this->GetLibrary(), (void *)&seek_COL, 0x10, 4, refs_TD);
		
		if (refs_TD.size() == 0) {
			DevMsg("CAddr_VTable::FindAddrWin: could not find _TypeDescriptor refs for \"%s\"\n", this->GetName());
			return false;
		}
		if (refs_TD.size() > 1) {
			DevMsg("CAddr_VTable::FindAddrWin: found multiple _TypeDescriptor refs for \"%s\"\n", this->GetName());
		}
		
		auto *p_COL = (__RTTI_CompleteObjectLocator *)refs_TD[0];
		
		
		/* STEP 3: get ptr to the vtable itself by finding references to the __RTTI_CompleteObjectLocator */
		
		std::vector<void *>refs_COL;
		Scan::FindFast(this->GetLibrary(), (void *)&p_COL, 0x4, 4, refs_COL);
		
		if (refs_COL.size() == 0) {
			DevMsg("CAddr_VTable::FindAddrWin: could not find __RTTI_CompleteObjectLocator refs for \"%s\"\n", this->GetName());
			return false;
		}
		if (refs_COL.size() > 1) {
			DevMsg("CAddr_VTable::FindAddrWin: found multiple __RTTI_CompleteObjectLocator refs for \"%s\"\n", this->GetName());
		}
		
		auto p_VT = (void **)((uintptr_t)refs_COL[0] + 0x4);
		
		
		addr = (uintptr_t)p_VT;
		return true;
	}
	
protected:
	virtual const char *GetWinRTTIStr() const = 0;
};


class CAddr__VT_CBaseEntity : public CAddr_VTable
{
public:
	virtual const char *GetName() const override final { return "CBasePlayer [VT]"; }
	
protected:
	virtual const char *GetSymbol() const override final { return "_ZTV11CBasePlayer"; }
	virtual const char *GetWinRTTIStr() const override final { return ".?AVCBaseEntity@@"; }
};
