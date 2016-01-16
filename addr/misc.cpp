#include "addr/addr.h"


/* vtable indexes (valid for windows ONLY!) */
constexpr int VT_idx_CBaseCombatWeapon_ItemPostFrame = (0x41c / 4);
constexpr int VT_idx_Action_Update                   = ( 0xb8 / 4);


#if 0
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
