#include "addr/addr.h"
#include "mem/scan.h"
#include "prop.h"
#include "stub/gamerules.h"
#include "util/rtti.h"
#include "addr/standard.h"


static constexpr uint8_t s_Buf_g_pGameRules[] = {
	0x55,                                                       // +0000  push ebp
	0x8b, 0xec,                                                 // +0001  mov ebp,esp
	0x56,                                                       // +0003  push esi
	0x8b, 0xf1,                                                 // +0004  mov esi,ecx
	0xc7, 0x06, 0x00, 0x00, 0x00, 0x00,                         // +0006  mov dword ptr [esi],offset p_VT
	0xc7, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // +000C  mov g_pGameRules,0x00000000
};

struct CExtract_g_pGameRules : public IExtract<uintptr_t>
{
	CExtract_g_pGameRules() : IExtract<uintptr_t>(sizeof(s_Buf_g_pGameRules)) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		DevMsg("CExtract_g_pGameRules: vt   @ 0x%08x\n", (uintptr_t)this->GetVTableAddr());
		DevMsg("CExtract_g_pGameRules: dtor @ 0x%08x\n", (uintptr_t)this->GetFuncAddr());
		
		auto ptr_vt = this->GetVTableAddr();
		if (ptr_vt == nullptr) return false;
		
		buf.CopyFrom(s_Buf_g_pGameRules);
		buf.SetDword(0x0006 + 2, (uint32_t)ptr_vt);
		
		mask.SetDword(0x000c + 2, 0x00000000);
		
		return true;
	}
	
	virtual const char *GetFuncName() const override   { return nullptr; }
	virtual void *GetFuncAddr() const override         { return (void *)this->GetVTableAddr()[VT_idx_CBaseGameSystemPerFrame_dtor]; }
	virtual uint32_t GetFuncOffMin() const override    { return 0x0000; }
	virtual uint32_t GetFuncOffMax() const override    { return 0x0000; }
	virtual uint32_t GetExtractOffset() const override { return 0x000c + 2; }
	
	const void **GetVTableAddr() const { return RTTI::GetVTable<CGameRules>(); }
	static constexpr int VT_idx_CBaseGameSystemPerFrame_dtor = 0x00d;
};


class CAddr_g_pGameRules : public IAddr_Sym
{
public:
	virtual const char *GetName() const override   { return "g_pGameRules"; }
	virtual const char *GetSymbol() const override { return "g_pGameRules"; }
	
	virtual bool FindAddrWin(uintptr_t& addr) const override
	{
		CExtract_g_pGameRules extractor;
		if (!extractor.Init())  return false;
		if (!extractor.Check()) return false;
		
		addr = extractor.Extract();
		return true;
	}
};
static CAddr_g_pGameRules addr_g_pGameRules;


class CAddr_pszWpnEntTranslationList : public IAddr_Sym
{
public:
	virtual const char *GetName() const override   { return "pszWpnEntTranslationList"; }
	virtual const char *GetSymbol() const override { return "pszWpnEntTranslationList"; }
	
	virtual bool FindAddrWin(uintptr_t& addr) const override
	{
		using StrScanner = CStringScanner<ScanDir::FORWARD, ScanResults::ALL, 1>;
		using ArrScanner = CMaskedScanner<ScanDir::FORWARD, ScanResults::ALL, 4>;
		
		// +0x00 ptr: "tf_weapon_shotgun"
		// +0x04 ptr: ""
		// +0x08 ptr: ""
		// +0x0c ptr: ""
		// +0x10 ptr: "tf_weapon_shotgun_soldier"
		// +0x14 ptr: ""
		// +0x18 ptr: ""
		// +0x1c ptr: "tf_weapon_shotgun_hwg"
		// +0x20 ptr: "tf_weapon_shotgun_pyro"
		// +0x24 ptr: ""
		// +0x28 ptr: "tf_weapon_shotgun_primary"
		
		auto strscan1 = new StrScanner(CLibSegBounds(Library::SERVER, ".rdata"), "tf_weapon_shotgun");
		auto strscan2 = new StrScanner(CLibSegBounds(Library::SERVER, ".rdata"), "tf_weapon_shotgun_soldier");
		auto strscan3 = new StrScanner(CLibSegBounds(Library::SERVER, ".rdata"), "tf_weapon_shotgun_hwg");
		auto strscan4 = new StrScanner(CLibSegBounds(Library::SERVER, ".rdata"), "tf_weapon_shotgun_pyro");
		auto strscan5 = new StrScanner(CLibSegBounds(Library::SERVER, ".rdata"), "tf_weapon_shotgun_primary");
		CMultiScan<StrScanner> scan1({ strscan1, strscan2, strscan3, strscan4, strscan5 });
		if (!strscan1->ExactlyOneMatch()) { DevMsg("Fail strscan1\n"); return false; }
		if (!strscan2->ExactlyOneMatch()) { DevMsg("Fail strscan2\n"); return false; }
		if (!strscan3->ExactlyOneMatch()) { DevMsg("Fail strscan3\n"); return false; }
		if (!strscan4->ExactlyOneMatch()) { DevMsg("Fail strscan4\n"); return false; }
		if (!strscan5->ExactlyOneMatch()) { DevMsg("Fail strscan5\n"); return false; }
		
		ByteBuf seek(0x32);
		ByteBuf mask(0x32);
		mask.SetDword(0x00, 0xffffffff); seek.SetDword(0x00, (uint32_t)strscan1->FirstMatch());
		mask.SetDword(0x10, 0xffffffff); seek.SetDword(0x10, (uint32_t)strscan2->FirstMatch());
		mask.SetDword(0x1c, 0xffffffff); seek.SetDword(0x1c, (uint32_t)strscan3->FirstMatch());
		mask.SetDword(0x20, 0xffffffff); seek.SetDword(0x20, (uint32_t)strscan4->FirstMatch());
		mask.SetDword(0x28, 0xffffffff); seek.SetDword(0x28, (uint32_t)strscan5->FirstMatch());
		CScan<ArrScanner> scan2(CLibSegBounds(Library::SERVER, ".data"), seek, mask);
		if (!scan2.ExactlyOneMatch()) { DevMsg("Fail scan2 %u\n", scan2.Matches().size()); return false; }
		
		auto match = (const char **)scan2.FirstMatch();
		if (match[1][0] != '\0') { DevMsg("Fail nullstr1\n"); return false; }
		if (match[2][0] != '\0') { DevMsg("Fail nullstr2\n"); return false; }
		if (match[3][0] != '\0') { DevMsg("Fail nullstr3\n"); return false; }
		if (match[5][0] != '\0') { DevMsg("Fail nullstr5\n"); return false; }
		if (match[6][0] != '\0') { DevMsg("Fail nullstr6\n"); return false; }
		if (match[9][0] != '\0') { DevMsg("Fail nullstr9\n"); return false; }
		
		addr = (uintptr_t)match;
		return true;
	}
};
static CAddr_pszWpnEntTranslationList addr_pszWpnEntTranslationList;


class CAddr_CTFPlayer_CanBeForcedToLaugh : public IAddr_Sym
{
public:
	virtual const char *GetName() const override   { return "CTFPlayer::CanBeForcedToLaugh"; }
	virtual const char *GetSymbol() const override { return "_ZN9CTFPlayer18CanBeForcedToLaughEv"; }
	
	virtual bool FindAddrWin(uintptr_t& addr) const override
	{
		using FuncScanner = CMaskedScanner<ScanDir::FORWARD, ScanResults::ALL, 0x10>;
		
		static constexpr uint8_t buf[] = {
			0xa1, 0x00, 0x00, 0x00, 0x00,             // +0000  mov eax,g_pGameRules
			0x56,                                     // +0005  push esi
			0x8b, 0xf1,                               // +0006  mov esi,ecx
			0x85, 0xc0,                               // +0008  test eax,eax
			0x74, 0x27,                               // +000A  jz L_0033
			0x80, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, // +000C  cmp byte ptr [eax+m_bPlayingMannVsMachine],0x0
			0x74, 0x1e,                               // +0013  jz L_0033
			0x8b, 0x06,                               // +0015  mov eax,[esi]
			0x8b, 0x80, 0x00, 0x00, 0x00, 0x00,       // +0017  mov eax,[eax+VToff(CBasePlayer::IsBot)]
			0xff, 0xd0,                               // +001C  call eax
			0x84, 0xc0,                               // +001F  test al,al
			0x74, 0x10,                               // +0021  jz L_0033
			0x8b, 0xce,                               // +0023  mov ecx,esi
			0xe8, 0x00, 0x00, 0x00, 0x00,             // +0025  call CBaseEntity::GetTeamNumber
			0x83, 0xf8, 0x03,                         // +002A  cmp eax,TF_TEAM_PVE_INVADERS
			0x75, 0x04,                               // +002D  jnz L_0033
			0x32, 0xc0,                               // +002F  xor al,al
			0x5e,                                     // +0031  pop esi
			0xc3,                                     // +0032  ret
			0xb0, 0x01,                               // +0033  mov al,1
			0x5e,                                     // +0035  pop esi
			0xc3,                                     // +0036  ret
		};
		
		ByteBuf seek(sizeof(buf));
		ByteBuf mask(sizeof(buf));
		
		seek.CopyFrom(buf);
		mask.SetAll(0xff);
		
		int off_CTFGameRules_m_bPlayingMannVsMachine;
		if (!Prop::FindOffset(off_CTFGameRules_m_bPlayingMannVsMachine, "CTFGameRules", "m_bPlayingMannVsMachine")) return false;
		
		void *addr_g_pGameRules = AddrManager::GetAddr("g_pGameRules");
		if (addr_g_pGameRules == nullptr) return false;
		
		seek.SetDword(0x0000 + 1, (uint32_t)addr_g_pGameRules);
		seek.SetDword(0x000c + 2, off_CTFGameRules_m_bPlayingMannVsMachine);
		mask.SetDword(0x0017 + 2, 0x00000000);
		mask.SetDword(0x0025 + 1, 0x00000000);
		
//		DevMsg("g_pGameRules: %08x\n", (uintptr_t)addr_g_pGameRules);
//		DevMsg("m_bPlayingMannVsMachine: %08x\n", off_CTFGameRules_m_bPlayingMannVsMachine);
		
		CScan<FuncScanner> scan1(CLibSegBounds(this->GetLibrary(), ".text"), seek, mask);
		if (!scan1.ExactlyOneMatch()) {
			DevMsg("Fail scan1 %u\n", scan1.Matches().size());
			return false;
		}
		
		addr = (uintptr_t)scan1.FirstMatch();
		return true;
	}
};
static CAddr_CTFPlayer_CanBeForcedToLaugh addr_CTFPlayer_CanBeForcedToLaugh;


// TODO: finder for CBasePlayer::IsBot
// (need to differentiate from CBasePlayer::IsFakeClient)
// 8b 81 xx xx xx xx  mov eax,[ecx+m_fFlags]
// c1 e8 xx           shr eax,log2(FL_FAKECLIENT)
// 83 e0 01           and eax,1
// c3                 ret


class IAddr_InterfaceVFunc : public IAddr
{
public:
	virtual bool FindAddrLinux(uintptr_t& addr) const override
	{
		const uintptr_t *vtable = *(const uintptr_t **)(this->GetInterfacePtr());
		addr = vtable[this->GetVTableIndex()];
		return true;
	}
	
	virtual bool FindAddrWin(uintptr_t& addr) const override
	{
		// when GetVIdxOfMemberFunc is fixed for MSVC, un-break this
		return false;
	}
	
	virtual const void *GetInterfacePtr() const = 0;
	virtual int GetVTableIndex() const = 0;
};

class CAddr_InterfaceVFunc : public IAddr_InterfaceVFunc
{
public:
	CAddr_InterfaceVFunc(const void **pp_iface, const std::string& n_iface, const std::string& n_func, int vtidx) :
		m_ppIFace(pp_iface), m_strIFaceName(n_iface), m_strFuncName(n_func), m_iVTIdx(vtidx)
	{
		this->m_strName = this->m_strIFaceName + "::" + this->m_strFuncName;
	}
	
	virtual const char *GetName() const override         { return this->m_strName.c_str(); }
	virtual const void *GetInterfacePtr() const override { return *this->m_ppIFace; }
	virtual int GetVTableIndex() const override          { return this->m_iVTIdx; }
	
private:
	const void **m_ppIFace;
	std::string m_strIFaceName;
	std::string m_strFuncName;
	int m_iVTIdx;
	std::string m_strName;
};


class CAddr_ISpatialPartition : public CAddr_InterfaceVFunc
{
public:
	CAddr_ISpatialPartition(const std::string& n_func, int vtidx) :
		CAddr_InterfaceVFunc((const void **)&partition, "ISpatialPartition", n_func, vtidx) {}
};
static CAddr_ISpatialPartition addr_ISpatialPartition_EnumerateElementsInBox(   "EnumerateElementsInBox",    GetVIdxOfMemberFunc(&ISpatialPartition::EnumerateElementsInBox));
static CAddr_ISpatialPartition addr_ISpatialPartition_EnumerateElementsInSphere("EnumerateElementsInSphere", GetVIdxOfMemberFunc(&ISpatialPartition::EnumerateElementsInSphere));


class CAddr_IEngineTrace : public CAddr_InterfaceVFunc
{
public:
	CAddr_IEngineTrace(const std::string& n_func, int vtidx) :
		CAddr_InterfaceVFunc((const void **)&enginetrace, "IEngineTrace", n_func, vtidx) {}
};
static CAddr_IEngineTrace addr_IEngineTrace_EnumerateEntities_ray("EnumerateEntities_ray", GetVIdxOfMemberFunc<IEngineTrace, void, const Ray_t&, bool, IEntityEnumerator *>(&IEngineTrace::EnumerateEntities));


#if 0
class CAddr_IVDebugOverlay : public CAddr_InterfaceVFunc
{
public:
	CAddr_IVDebugOverlay(const std::string& n_func, int vtidx) :
		CAddr_InterfaceVFunc((const void **)&debugoverlay, "IVDebugOverlay", n_func, vtidx) {}
};
static CAddr_IVDebugOverlay addr_IVDebugOverlay_AddEntityTextOverlay("AddEntityTextOverlay", GetVIdxOfMemberFunc(&IVDebugOverlay::AddEntityTextOverlay));
#endif


#if 0
class RemoveMe
{
public:
	RemoveMe()
	{
		Test_ISpatialPartition((ISpatialPartition *)this);
		Test_IEngineTrace((IEngineTrace *)this);
		Test_IVDebugOverlay((IVDebugOverlay *)this);
	}
	
	void DumpMemAt(void *p)
	{
		uint32_t *i = (uint32_t *)p;
		
		DevMsg("%08x %08x %08x %08x\n", i[0], i[1], i[2], i[3]);
	}
	
	void Test_ISpatialPartition(ISpatialPartition *p)
	{
		DevMsg("EnumerateElementsInBox\n");
		p->EnumerateElementsInBox(0, vec3_origin, vec3_origin, false, nullptr);
		DevMsg("EnumerateElementsInSphere\n");
		p->EnumerateElementsInSphere(0, vec3_origin, 0.0f, false, nullptr);
	}
	
	void Test_IEngineTrace(IEngineTrace *p)
	{
		char pad1[] = "AAAAAAAA";
		int (IEngineTrace::*p1)(const Vector&, IHandleEntity**) = &IEngineTrace::GetPointContents;
		char pad2[] = "BBBBBBBB";
		void (IEngineTrace::*p2)(const Ray_t&, bool, IEntityEnumerator *) = &IEngineTrace::EnumerateEntities;
		char pad3[] = "CCCCCCCC";
		void (IEngineTrace::*p3)(const Vector&, const Vector&, IEntityEnumerator *) = &IEngineTrace::EnumerateEntities;
		char pad4[] = "DDDDDDDD";
		int (IEngineTrace::*p4)(const Vector&) = &IEngineTrace::GetLeafContainingPoint;
		char pad5[] = "EEEEEEEE";
		
		DumpMemAt((void *)&p1);
		DumpMemAt((void *)&p2);
		DumpMemAt((void *)&p3);
		DumpMemAt((void *)&p4);
		
		auto r = (Ray_t *)this;
		
		DevMsg("GetPointContents\n");
		p->GetPointContents(vec3_origin, nullptr);
		DevMsg("EnumerateEntities_ray\n");
		p->EnumerateEntities(*r, false, nullptr);
		DevMsg("EnumerateEntities_vec\n");
		p->EnumerateEntities(vec3_origin, vec3_origin, nullptr);
		DevMsg("GetLeafContainingPoint\n");
		p->GetLeafContainingPoint(vec3_origin);
	}
	
	void Test_IVDebugOverlay(IVDebugOverlay *p)
	{
		DevMsg("AddEntityTextOverlay\n");
		p->AddEntityTextOverlay(0, 0, 0.0f, 0, 0, 0, 0, "");
		DevMsg("AddBoxOverlay2\n");
		p->AddBoxOverlay2(vec3_origin, vec3_origin, vec3_origin, vec3_angle, Color(), Color(), 0.0f);
	}
};
RemoveMe removeme;
#endif


static CAddr_Func_KnownVTIdx addr_CTFBotUseItem_D2("CTFBotUseItem::~CTFBotUseItem [D2]", "<nosym>", ".?AVCTFBotUseItem@@", 0x00);
class CAddr_CTFBotUseItem_C1 : public IAddr_Sym
{
public:
	virtual const char *GetName() const override   { return "CTFBotUseItem::CTFBotUseItem [C1]"; }
	virtual const char *GetSymbol() const override { return "<ignoreme>"; }
	
	virtual bool FindAddrWin(uintptr_t& addr) const override
	{
		using VTRefScanner = CTypeScanner<ScanDir::FORWARD, ScanResults::ALL, 1, const void **>;
		
		auto p_VT   = RTTI::GetVTable(".?AVCTFBotUseItem@@");
		auto p_dtor = AddrManager::GetAddr("CTFBotUseItem::~CTFBotUseItem [D2]");
		
		CScan<VTRefScanner> scan1(CLibSegBounds(this->GetLibrary(), ".text"), p_VT);
		
		std::vector<const void *> matches;
		for (auto match : scan1.Matches()) {
			if (match != p_dtor) {
				matches.push_back(match);
			}
		}
		
		if (matches.size() != 1) {
			DevMsg("CAddr_CTFBotUseItem_C1: %u\n", matches.size());
			return false;
		}
		
		addr = (uintptr_t)matches[0];
		return true;
	}
};
static CAddr_CTFBotUseItem_C1 addr_CTFBotUseItem_C1;


class IAddr_Func_EBPPrologue_UniqueCall : public IAddr_Sym
{
public:
	virtual bool FindAddrWin(uintptr_t& addr) const override
	{
		using CallScanner = CCallScanner<ScanDir::FORWARD, ScanResults::ALL, 1>;
		
		auto p_ref = AddrManager::GetAddr(this->GetUniqueFuncName());
		if (p_ref == nullptr) {
			DevMsg("IAddr_Func_EBPPrologue_UniqueCall: \"%s\": no addr for ostensibly unique func\n", this->GetName());
			return false;
		}
		
		CScan<CallScanner> scan1(CLibSegBounds(this->GetLibrary(), ".text"), (uint32_t)p_ref);
		if (!scan1.ExactlyOneMatch()) {
			DevMsg("IAddr_Func_EBPPrologue_UniqueCall: \"%s\": found %u refs to ostensibly unique func\n", this->GetName(), scan1.Matches().size());
			return false;
		}
		auto p_in_func = (const char **)scan1.FirstMatch();
		
		auto p_func = Scan::FindFuncPrologue(p_in_func);
		if (p_func == nullptr) {
			DevMsg("IAddr_Func_EBPPrologue_UniqueCall: \"%s\": could not locate EBP prologue\n", this->GetName());
			return false;
		}
		
		addr = (uintptr_t)p_func;
		return true;
	}
	
protected:
	virtual const char *GetUniqueFuncName() const = 0;
};

class CAddr_CTFBot_OpportunisticallyUseWeaponAbilities : public IAddr_Func_EBPPrologue_UniqueCall
{
public:
	virtual const char *GetName() const override           { return "CTFBot::OpportunisticallyUseWeaponAbilities"; }
	virtual const char *GetSymbol() const override         { return "_ZN6CTFBot35OpportunisticallyUseWeaponAbilitiesEv"; }
	virtual const char *GetUniqueFuncName() const override { return "CTFBotUseItem::CTFBotUseItem [C1]"; }
};
