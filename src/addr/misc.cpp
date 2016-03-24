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


class CAddr_IServerGameDLL : public CAddr_InterfaceVFunc
{
public:
	CAddr_IServerGameDLL(const std::string& n_func, int vtidx) :
		CAddr_InterfaceVFunc((const void **)&gamedll, "IServerGameDLL", n_func, vtidx) {}
};
static CAddr_IServerGameDLL addr_IServerGameDLL_GameFrame("GameFrame", GetVIdxOfMemberFunc(&IServerGameDLL::GameFrame));


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


class CAddr_Pointer : public IAddr
{
public:
	CAddr_Pointer(const std::string& name, const void *ptr) :
		m_strName(name), m_pAddr(ptr) {}
	
	virtual const char *GetName() const override { return this->m_strName.c_str(); }
	
	virtual bool FindAddrLinux(uintptr_t& addr) const override
	{
		if (this->m_pAddr == nullptr) return false;
		
		addr = (uintptr_t)this->m_pAddr;
		return true;
	}
	virtual bool FindAddrWin(uintptr_t& addr) const override
	{
		return this->FindAddrLinux(addr);
	}
	
private:
	std::string m_strName;
	const void *m_pAddr;
};
static CAddr_Pointer addr_NDebugOverlay_Box(                       "[local] NDebugOverlay::Box",                        (const void *)&NDebugOverlay::Box);
static CAddr_Pointer addr_NDebugOverlay_BoxDirection(              "[local] NDebugOverlay::BoxDirection",               (const void *)&NDebugOverlay::BoxDirection);
static CAddr_Pointer addr_NDebugOverlay_BoxAngles(                 "[local] NDebugOverlay::BoxAngles",                  (const void *)&NDebugOverlay::BoxAngles);
static CAddr_Pointer addr_NDebugOverlay_SweptBox(                  "[local] NDebugOverlay::SweptBox",                   (const void *)&NDebugOverlay::SweptBox);
static CAddr_Pointer addr_NDebugOverlay_EntityBounds(              "[local] NDebugOverlay::EntityBounds",               (const void *)&NDebugOverlay::EntityBounds);
static CAddr_Pointer addr_NDebugOverlay_Line(                      "[local] NDebugOverlay::Line",                       (const void *)&NDebugOverlay::Line);
static CAddr_Pointer addr_NDebugOverlay_Triangle(                  "[local] NDebugOverlay::Triangle",                   (const void *)&NDebugOverlay::Triangle);
static CAddr_Pointer addr_NDebugOverlay_EntityText(                "[local] NDebugOverlay::EntityText",                 (const void *)&NDebugOverlay::EntityText);
static CAddr_Pointer addr_NDebugOverlay_EntityTextAtPosition(      "[local] NDebugOverlay::EntityTextAtPosition",       (const void *)&NDebugOverlay::EntityTextAtPosition);
static CAddr_Pointer addr_NDebugOverlay_Grid(                      "[local] NDebugOverlay::Grid",                       (const void *)&NDebugOverlay::Grid);
static CAddr_Pointer addr_NDebugOverlay_Text(                      "[local] NDebugOverlay::Text",                       (const void *)&NDebugOverlay::Text);
static CAddr_Pointer addr_NDebugOverlay_ScreenText(                "[local] NDebugOverlay::ScreenText",                 (const void *)&NDebugOverlay::ScreenText);
static CAddr_Pointer addr_NDebugOverlay_Cross3D_ext(               "[local] NDebugOverlay::Cross3D_ext",                (const void *)(void (*)(const Vector&, const Vector&, const Vector&, int, int, int, bool, float))&NDebugOverlay::Cross3D);
static CAddr_Pointer addr_NDebugOverlay_Cross3D_size(              "[local] NDebugOverlay::Cross3D_size",               (const void *)(void (*)(const Vector&, float, int, int, int, bool, float))&NDebugOverlay::Cross3D);
static CAddr_Pointer addr_NDebugOverlay_Cross3DOriented_ang(       "[local] NDebugOverlay::Cross3DOriented_ang",        (const void *)(void (*)(const Vector&, const QAngle&, float, int, int, int, bool, float))&NDebugOverlay::Cross3DOriented);
static CAddr_Pointer addr_NDebugOverlay_Cross3DOriented_mat(       "[local] NDebugOverlay::Cross3DOriented_mat",        (const void *)(void (*)(const matrix3x4_t&, float, int, bool, float))&NDebugOverlay::Cross3DOriented);
static CAddr_Pointer addr_NDebugOverlay_HorzArrow(                 "[local] NDebugOverlay::HorzArrow",                  (const void *)&NDebugOverlay::HorzArrow);
static CAddr_Pointer addr_NDebugOverlay_YawArrow(                  "[local] NDebugOverlay::YawArrow",                   (const void *)&NDebugOverlay::YawArrow);
static CAddr_Pointer addr_NDebugOverlay_VertArrow(                 "[local] NDebugOverlay::VertArrow",                  (const void *)&NDebugOverlay::VertArrow);
static CAddr_Pointer addr_NDebugOverlay_Axis(                      "[local] NDebugOverlay::Axis",                       (const void *)&NDebugOverlay::Axis);
static CAddr_Pointer addr_NDebugOverlay_Sphere(                    "[local] NDebugOverlay::Sphere",                     (const void *)(void (*)(const Vector&, float, int, int, int, bool, float))&NDebugOverlay::Sphere);
static CAddr_Pointer addr_NDebugOverlay_Circle(                    "[local] NDebugOverlay::Circle",                     (const void *)(void (*)(const Vector&, float, int, int, int, int, bool, float))&NDebugOverlay::Circle);
static CAddr_Pointer addr_NDebugOverlay_Circle_ang(                "[local] NDebugOverlay::Circle_ang",                 (const void *)(void (*)(const Vector&, const QAngle&, float, int, int, int, int, bool, float))&NDebugOverlay::Circle);
static CAddr_Pointer addr_NDebugOverlay_Circle_axes(               "[local] NDebugOverlay::Circle_axes",                (const void *)(void (*)(const Vector&, const Vector&, const Vector&, float, int, int, int, int, bool, float))&NDebugOverlay::Circle);
static CAddr_Pointer addr_NDebugOverlay_Sphere_ang(                "[local] NDebugOverlay::Sphere_ang",                 (const void *)(void (*)(const Vector&, const QAngle&, float, int, int, int, int, bool, float))&NDebugOverlay::Sphere);
static CAddr_Pointer addr_NDebugOverlay_Clear(                     "[local] NDebugOverlay::Clear",                      (const void *)&NDebugOverlay::Clear);
static CAddr_Pointer addr_NDebugOverlay_LineAlpha(                 "[local] NDebugOverlay::LineAlpha",                  (const void *)&NDebugOverlay::LineAlpha);


class CAddr_Client_UserMessages : public IAddr_Sym
{
public:
	CAddr_Client_UserMessages()
	{
		this->SetLibrary(Library::CLIENT);
	}
	
	virtual const char *GetName() const override   { return "[client] usermessages"; }
	virtual const char *GetSymbol() const override { return "usermessages"; }
	
	virtual bool FindAddrWin(uintptr_t& addr) const override
	{
		using MyScanner = CMaskedScanner<ScanDir::FORWARD, ScanResults::ALL, 1>;
		
		const char *p_str = Scan::FindUniqueConstStr(this->GetLibrary(), "MVMWaveFailed");
		if (p_str == nullptr) {
			DevMsg("%s: no const str\n", this->GetName());
			return false;
		}
		
		constexpr uint8_t buf[] = {
			0x8b, 0x0d, 0x00, 0x00, 0x00, 0x00, // +0000  mov ecx,0xVVVVVVVV
			0x6a, 0x00,                         // +0006  push 0x00000000
			0x68, 0x00, 0x00, 0x00, 0x00,       // +0008  push p_str
			0xe8, 0x00, 0x00, 0x00, 0x00,       // +000D  call 0xXXXXXXXX
		};
		
		ByteBuf seek(sizeof(buf));
		ByteBuf mask(sizeof(buf));
		seek.CopyFrom(buf);
		mask.SetAll(0xff);
		
		mask.SetRange(0x00 + 2, 0x04, 0x00);
		seek.SetDword(0x08 + 1, (uint32_t)p_str);
		mask.SetRange(0x0d + 1, 0x04, 0x00);
		
		CScan<MyScanner> scan1(CLibSegBounds(Library::CLIENT, ".text"), seek, mask);
		if (!scan1.ExactlyOneMatch()) {
			DevMsg("%s: %u matches\n", this->GetName(), scan1.Matches().size());
			return false;
		}
		
		addr = *(uintptr_t *)((uintptr_t)scan1.FirstMatch() + 0x02);
		return true;
	}
};
static CAddr_Client_UserMessages addr_client_UserMessages;


class CAddr_Client_CUserMessages_Register : public IAddr_Sym
{
public:
	CAddr_Client_CUserMessages_Register()
	{
		this->SetLibrary(Library::CLIENT);
	}
	
	virtual const char *GetName() const override   { return "[client] CUserMessages::Register"; }
	virtual const char *GetSymbol() const override { return "_ZN13CUserMessages8RegisterEPKci"; }
	
	virtual bool FindAddrWin(uintptr_t& addr) const override
	{
		using MyScanner = CMaskedScanner<ScanDir::FORWARD, ScanResults::ALL, 1>;
		
		const char *p_str = Scan::FindUniqueConstStr(this->GetLibrary(), "MVMWaveFailed");
		if (p_str == nullptr) {
			DevMsg("%s: no const str\n", this->GetName());
			return false;
		}
		
		constexpr uint8_t buf[] = {
			0x8b, 0x0d, 0x00, 0x00, 0x00, 0x00, // +0000  mov ecx,0xXXXXXXXX
			0x6a, 0x00,                         // +0006  push 0x00000000
			0x68, 0x00, 0x00, 0x00, 0x00,       // +0008  push p_str
			0xe8, 0x00, 0x00, 0x00, 0x00,       // +000D  call 0xVVVVVVVV
		};
		
		ByteBuf seek(sizeof(buf));
		ByteBuf mask(sizeof(buf));
		seek.CopyFrom(buf);
		mask.SetAll(0xff);
		
		mask.SetRange(0x00 + 2, 0x04, 0x00);
		seek.SetDword(0x08 + 1, (uint32_t)p_str);
		mask.SetRange(0x0d + 1, 0x04, 0x00);
		
		CScan<MyScanner> scan1(CLibSegBounds(Library::CLIENT, ".text"), seek, mask);
		if (!scan1.ExactlyOneMatch()) {
			DevMsg("%s: %u matches\n", this->GetName(), scan1.Matches().size());
			return false;
		}
		
		auto match = (uintptr_t)scan1.FirstMatch() + 0x0e;
		addr = *(uintptr_t *)match + (match + 4);
		return true;
	}
};
static CAddr_Client_CUserMessages_Register addr_client_CUserMessages_Register;


class CAddr_Client_CUserMessages_HookMessage : public IAddr_Sym
{
public:
	CAddr_Client_CUserMessages_HookMessage()
	{
		this->SetLibrary(Library::CLIENT);
	}
	
	virtual const char *GetName() const override   { return "[client] CUserMessages::HookMessage"; }
	virtual const char *GetSymbol() const override { return "_ZN13CUserMessages11HookMessageEPKcPFvR7bf_readE"; }
	
	virtual bool FindAddrWin(uintptr_t& addr) const override
	{
		using MyScanner = CMaskedScanner<ScanDir::FORWARD, ScanResults::ALL, 1>;
		
		const char *p_str = Scan::FindUniqueConstStr(this->GetLibrary(), "MVMWaveFailed");
		if (p_str == nullptr) {
			DevMsg("%s: no const str\n", this->GetName());
			return false;
		}
		
		constexpr uint8_t buf[] = {
			0x8b, 0x0d, 0x00, 0x00, 0x00, 0x00, // +0000  mov ecx,0xXXXXXXXX
			0x68, 0x00, 0x00, 0x00, 0x00,       // +0006  push 0xXXXXXXXX
			0x68, 0x00, 0x00, 0x00, 0x00,       // +000B  push p_str
			0xe8, 0x00, 0x00, 0x00, 0x00,       // +0010  call 0xVVVVVVVV
		};
		
		ByteBuf seek(sizeof(buf));
		ByteBuf mask(sizeof(buf));
		seek.CopyFrom(buf);
		mask.SetAll(0xff);
		
		mask.SetRange(0x00 + 2, 0x04, 0x00);
		mask.SetRange(0x06 + 1, 0x04, 0x00);
		seek.SetDword(0x0b + 1, (uint32_t)p_str);
		mask.SetRange(0x10 + 1, 0x04, 0x00);
		
		CScan<MyScanner> scan1(CLibSegBounds(Library::CLIENT, ".text"), seek, mask);
		if (!scan1.ExactlyOneMatch()) {
			DevMsg("%s: %u matches\n", this->GetName(), scan1.Matches().size());
			return false;
		}
		
		auto match = (uintptr_t)scan1.FirstMatch() + 0x11;
		addr = *(uintptr_t *)match + (match + 4);
		return true;
	}
};
static CAddr_Client_CUserMessages_HookMessage addr_client_CUserMessages_HookMessage;


/* string literal reference + nearby pattern match + back to prologue */
class IAddr_Client_CDebugOverlay : public IAddr_Sym
{
public:
	virtual bool FindAddrWin(uintptr_t& addr) const override
	{
		using StrRefScanner        = CTypeScanner  <ScanDir::FORWARD, ScanResults::ALL, 1, const char *>;
		using NearbyPatternScanner = CMaskedScanner<ScanDir::FORWARD, ScanResults::ALL, 1>;
		
		constexpr const char *str = "s_OverlayMutex";
		const char *p_str = Scan::FindUniqueConstStr(this->GetLibrary(), str);
		if (p_str == nullptr) {
			DevMsg("IAddr_Client_CDebugOverlay: \"%s\": failed to find string \"%s\"\n", this->GetName(), str);
			return false;
		}
		
		CScan<StrRefScanner> scan1(CLibSegBounds(this->GetLibrary(), ".text"), p_str);
		DevMsg("IAddr_Client_CDebugOverlay: \"%s\": found %u preliminary matches\n", this->GetName(), scan1.Matches().size());
		for (auto match : scan1.Matches()) {
			DevMsg("  %08x\n", (uintptr_t)match);
		}
		
		int buf_size = this->GetBufferSize();
		ByteBuf seek(buf_size);
		ByteBuf mask(buf_size);
		this->SetUpBuffers(seek, mask);
		
		std::vector<const void *>matches;
		for (auto match : scan1.Matches()) {
			CScan<NearbyPatternScanner> scan2(CAddrOffBounds(match, 0x80), seek, mask);
			DevMsg("  %08x: %d sub-matches\n", (uintptr_t)match, scan2.Matches().size());
			
			if (!scan2.ExactlyOneMatch()) continue;
			auto p_in_func = scan2.FirstMatch();
			
			auto p_func = Scan::FindFuncPrologue(p_in_func);
			if (p_func == nullptr) continue;
			
			matches.push_back(p_func);
		}
		
		if (matches.size() != 1) {
			DevMsg("IAddr_Client_CDebugOverlay: \"%s\": found %u potential matches\n", this->GetName(), matches.size());
			return false;
		}
		
		addr = (uintptr_t)matches[0];
		return true;
	}
	
protected:
	virtual int GetBufferSize() const = 0;
	virtual void SetUpBuffers(ByteBuf& seek, ByteBuf& mask) const = 0;
};

static constexpr uint8_t s_Buf_CDebugOverlay_AddSphereOverlay[] = {
//	0x6a, 0x3c,                                     // +0000  push 0x3c
//	0xe8, 0x00, 0x00, 0x00, 0x00,                   // +0002  call 0xXXXXXXXX
//	0xf3, 0x0f, 0x10, 0x0d, 0x00, 0x00, 0x00, 0x00, // +0007  movss xmm0,ds:0xXXXXXXXX
//	0x8b, 0xf0,                                     // +000F  mov esi,eax
//	0x83, 0xc4, 0x00,                               // +0011  add esp,0xXX
//	0x85, 0xf6,                                     // +0014  test esi,esi
//	0x74, 0x00,                                     // +0016  jz +0xXX
//	0xc7, 0x46, 0x08, 0xff, 0xff, 0xff, 0xff,       // +0018  mov dword ptr [esi+0x08],0xffffffff
//	0xc7, 0x46, 0x04, 0xff, 0xff, 0xff, 0xff,       // +001F  mov dword ptr [esi+0x04],0xffffffff
//	0xc7, 0x46, 0x0c, 0x00, 0x00, 0x00, 0x00,       // +0026  mov dword ptr [esi+0x0c],0x00000000
//	0xc7, 0x46, 0x10, 0x00, 0x00, 0x00, 0x00,       // +002D  mov dword ptr [esi+0x10],0x00000000
//	0xc7, 0x06, 0x01, 0x00, 0x00, 0x00,             // +0034  mov dword ptr [esi+0x00],0x00000001
	
	0xc7, 0x46, 0x08, 0xff, 0xff, 0xff, 0xff, // +0000  mov dword ptr [esi+0x08],0xffffffff
	0xc7, 0x46, 0x04, 0xff, 0xff, 0xff, 0xff, // +0007  mov dword ptr [esi+0x04],0xffffffff
	0xc7, 0x46, 0x0c, 0x00, 0x00, 0x00, 0x00, // +000E  mov dword ptr [esi+0x0c],0x00000000
	0xc7, 0x46, 0x10, 0x00, 0x00, 0x00, 0x00, // +0015  mov dword ptr [esi+0x10],0x00000000
	0xc7, 0x06, 0x01, 0x00, 0x00, 0x00,       // +001C  mov dword ptr [esi+0x00],0x00000001
};

struct CAddr_Client_CDebugOverlay_AddSphereOverlay : public IAddr_Client_CDebugOverlay
{
	CAddr_Client_CDebugOverlay_AddSphereOverlay()
	{
		this->SetLibrary(Library::ENGINE);
	}
	
	virtual const char *GetName() const override   { return "[client] CDebugOverlay::AddSphereOverlay"; }
	virtual const char *GetSymbol() const override { return "_ZN13CDebugOverlay16AddSphereOverlayERK6Vectorfiiiiiif"; }
	
	virtual int GetBufferSize() const override { return sizeof(s_Buf_CDebugOverlay_AddSphereOverlay); }
	
	virtual void SetUpBuffers(ByteBuf& seek, ByteBuf& mask) const override
	{
		seek.CopyFrom(s_Buf_CDebugOverlay_AddSphereOverlay);
		mask.SetAll(0xff);
		
	//	mask.SetRange(0x02 + 1, 0x04, 0x00);
	//	mask.SetRange(0x07 + 4, 0x04, 0x00);
	//	mask.SetRange(0x11 + 2, 0x01, 0x00);
	//	mask.SetRange(0x16 + 1, 0x01, 0x00);
	}
};
static CAddr_Client_CDebugOverlay_AddSphereOverlay addr_client_CDebugOverlay_AddSphereOverlay;
