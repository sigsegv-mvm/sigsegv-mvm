#include "addr/addr.h"
#include "mem/scan.h"


class CAddr_pszWpnEntTranslationList : public IAddr_Sym
{
public:
	virtual const char *GetName() const override { return "pszWpnEntTranslationList"; }
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
		CMultiScan scan1(ScanDir::FORWARD, CLibSegBounds(Library::SERVER, ".rdata"), 1,
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
		CSingleScan scan2(ScanDir::FORWARD, CLibSegBounds(Library::SERVER, ".data"), 4, new CMaskedScanner(ScanResults::ALL, seek, mask));
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


// TODO: finder for CBasePlayer::IsBot
// (need to differentiate from CBasePlayer::IsFakeClient)
// 8b 81 xx xx xx xx  mov eax,[ecx+m_fFlags]
// c1 e8 xx           shr eax,log2(FL_FAKECLIENT)
// 83 e0 01           and eax,1
// c3                 ret


#if 0
/* vtable indexes (valid for windows ONLY!) */
constexpr int VT_idx_CBaseCombatWeapon_ItemPostFrame = (0x41c / 4);
constexpr int VT_idx_Action_Update                   = ( 0xb8 / 4);


struct CAddr_CTFSniperRifle_ItemPostFrame : public IAddr_Sym
{
	const char *GetName() const override   { return "CTFSniperRifle::ItemPostFrame"; }
	const char *GetSymbol() const override { return "_ZN14CTFSniperRifle13ItemPostFrameEv"; }
	
	bool FindAddrWin(uintptr_t& addr) const override
	{
		auto p_VT = (uintptr_t *)AddrManager::GetAddr("[VT] CTFSniperRifle");
		if (p_VT == nullptr) return false;
		
		addr = p_VT[VT_idx_CBaseCombatWeapon_ItemPostFrame];
		return true;
	}
};
static CAddr_CTFSniperRifle_ItemPostFrame addr_CTFSniperRifle_ItemPostFrame;


struct CAddr_CTFSniperRifleClassic_ItemPostFrame : public IAddr_Sym
{
	const char *GetName() const override   { return "CTFSniperRifleClassic::ItemPostFrame"; }
	const char *GetSymbol() const override { return "_ZN21CTFSniperRifleClassic13ItemPostFrameEv"; }
	
	bool FindAddrWin(uintptr_t& addr) const override
	{
		auto p_VT = (uintptr_t *)AddrManager::GetAddr("[VT] CTFSniperRifleClassic");
		if (p_VT == nullptr) return false;
		
		addr = p_VT[VT_idx_CBaseCombatWeapon_ItemPostFrame];
		return true;
	}
};
static CAddr_CTFSniperRifleClassic_ItemPostFrame addr_CTFSniperRifleClassic_ItemPostFrame;
#endif


#if 0
struct CAddr_CTFBotMvMEngineerTeleportSpawn_Update : public IAddr_Func_UniqueStr_EBPBacktrack_KnownVTOff
{
	const char *GetName() const override       { return "CTFBotMvMEngineerTeleportSpawn::Update"; }
	const char *GetSymbol() const override     { return "_ZN30CTFBotMvMEngineerTeleportSpawn6UpdateEP6CTFBotf"; }
	const char *GetUniqueStr() const override  { return "teleported_mvm_bot"; }
	const char *GetVTableName() const override { return "[VT] CTFBotMvMEngineerTeleportSpawn"; }
	int GetVTableIndex() const override        { return VT_idx_Action_Update; }
};
static CAddr_CTFBotMvMEngineerTeleportSpawn_Update addr_CTFBotMvMEngineerTeleportSpawn_Update;


struct CAddr_CTFBotMvMEngineerBuildSentryGun_Update : public IAddr_Func_UniqueStr_EBPBacktrack_KnownVTOff
{
	const char *GetName() const override       { return "CTFBotMvMEngineerBuildSentryGun::Update"; }
	const char *GetSymbol() const override     { return "_ZN31CTFBotMvMEngineerBuildSentryGun6UpdateEP6CTFBotf"; }
	const char *GetUniqueStr() const override  { return "Built a sentry"; } // alt: "Placing sentry"
	const char *GetVTableName() const override { return "[VT] CTFBotMvMEngineerBuildSentryGun"; }
	int GetVTableIndex() const override        { return VT_idx_Action_Update; }
};
static CAddr_CTFBotMvMEngineerBuildSentryGun_Update addr_CTFBotMvMEngineerBuildSentryGun_Update;


struct CAddr_CTFBotMvMEngineerBuildTeleportExit_Update : public IAddr_Func_UniqueStr_EBPBacktrack_KnownVTOff
{
	const char *GetName() const override       { return "CTFBotMvMEngineerBuildTeleportExit::Update"; }
	const char *GetSymbol() const override     { return "_ZN34CTFBotMvMEngineerBuildTeleportExit6UpdateEP6CTFBotf"; }
	const char *GetUniqueStr() const override  { return "Engineer.MVM_AutoBuildingTeleporter02"; }
	const char *GetVTableName() const override { return "[VT] CTFBotMvMEngineerBuildTeleportExit"; }
	int GetVTableIndex() const override        { return VT_idx_Action_Update; }
};
static CAddr_CTFBotMvMEngineerBuildTeleportExit_Update addr_CTFBotMvMEngineerBuildTeleportExit_Update;
#endif
