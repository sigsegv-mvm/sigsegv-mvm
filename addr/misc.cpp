#include "addr/addr.h"
#include "mem/scan.h"
#include "prop.h"
#include "stub/gamerules.h"


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
//		DevMsg("CExtract_g_pGameRules: vt   @ 0x%08x\n", this->GetVTableAddr());
//		DevMsg("CExtract_g_pGameRules: dtor @ 0x%08x\n", (uintptr_t)this->GetFuncAddr());
		
		buf.CopyFrom(s_Buf_g_pGameRules);
		buf.SetDword(0x0006 + 2, (uint32_t)this->GetVTableAddr());
		
		mask.SetDword(0x000c + 2, 0x00000000);
		
		return true;
	}
	
	virtual const char *GetFuncName() const override   { return nullptr; }
	virtual void *GetFuncAddr() const override         { return (void *)this->GetVTableAddr()[VT_idx_CBaseGameSystemPerFrame_dtor]; }
	virtual uint32_t GetFuncOffMin() const override    { return 0x0000; }
	virtual uint32_t GetFuncOffMax() const override    { return 0x0000; }
	virtual uint32_t GetExtractOffset() const override { return 0x000c + 2; }
	
	const uintptr_t *GetVTableAddr() const { return (const uintptr_t *)AddrManager::GetAddr("[VT] CGameRules"); }
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
		
		auto strscan1 = new CStringScanner(ScanResults::ALL, "tf_weapon_shotgun");
		auto strscan2 = new CStringScanner(ScanResults::ALL, "tf_weapon_shotgun_soldier");
		auto strscan3 = new CStringScanner(ScanResults::ALL, "tf_weapon_shotgun_hwg");
		auto strscan4 = new CStringScanner(ScanResults::ALL, "tf_weapon_shotgun_pyro");
		auto strscan5 = new CStringScanner(ScanResults::ALL, "tf_weapon_shotgun_primary");
		CMultiScan<ScanDir::FORWARD, 1> scan1(CLibSegBounds(Library::SERVER, ".rdata"),
			{ strscan1, strscan2, strscan3, strscan4, strscan5 });
		if (strscan1->Matches().size() != 1) { DevMsg("Fail strscan1\n"); return false; }
		if (strscan2->Matches().size() != 1) { DevMsg("Fail strscan2\n"); return false; }
		if (strscan3->Matches().size() != 1) { DevMsg("Fail strscan3\n"); return false; }
		if (strscan4->Matches().size() != 1) { DevMsg("Fail strscan4\n"); return false; }
		if (strscan5->Matches().size() != 1) { DevMsg("Fail strscan5\n"); return false; }
		
		ByteBuf seek(0x32);
		ByteBuf mask(0x32);
		mask.SetDword(0x00, 0xffffffff); seek.SetDword(0x00, (uint32_t)strscan1->Matches()[0]);
		mask.SetDword(0x10, 0xffffffff); seek.SetDword(0x10, (uint32_t)strscan2->Matches()[0]);
		mask.SetDword(0x1c, 0xffffffff); seek.SetDword(0x1c, (uint32_t)strscan3->Matches()[0]);
		mask.SetDword(0x20, 0xffffffff); seek.SetDword(0x20, (uint32_t)strscan4->Matches()[0]);
		mask.SetDword(0x28, 0xffffffff); seek.SetDword(0x28, (uint32_t)strscan5->Matches()[0]);
		CSingleScan<ScanDir::FORWARD, 4> scan2(CLibSegBounds(Library::SERVER, ".data"), new CMaskedScanner(ScanResults::ALL, seek, mask));
		if (scan2.Matches().size() != 1) { DevMsg("Fail scan2 %u\n", scan2.Matches().size()); return false; }
		
		auto match = (const char **)scan2.Matches()[0];
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
		
		CSingleScan<ScanDir::FORWARD, 0x10> scan1(CLibSegBounds(this->GetLibrary(), ".text"), new CMaskedScanner(ScanResults::ALL, seek, mask));
		if (scan1.Matches().size() != 1) {
			DevMsg("Fail scan1 %u\n", scan1.Matches().size());
			return false;
		}
		
		addr = (uintptr_t)scan1.Matches()[0];
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
