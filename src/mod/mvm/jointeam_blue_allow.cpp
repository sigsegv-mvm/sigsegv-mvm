#include "mod.h"
#include "stub/gamerules.h"
#include "stub/tfbot.h"
#include "stub/tf_shareddefs.h"
#include "util/clientmsg.h"
#include "util/admin.h"
#include "util/iterate.h"

// TODO: move to common.h
#include <igamemovement.h>
#include <in_buttons.h>


/* HACK */
namespace Mod::Pop::PopMgr_Extensions
{
	bool PopFileIsOverridingJoinTeamBlueConVarOn();
}


namespace Mod::MvM::JoinTeam_Blue_Allow
{
	using CollectPlayersFunc_t = int (*)(CUtlVector<CTFPlayer *> *, int, bool, bool);
	
	
	constexpr uint8_t s_Buf_CollectPlayers_Caller1[] = {
		0xc7, 0x44, 0x24, 0x0c, 0x00, 0x00, 0x00, 0x00, // +0000  mov dword ptr [esp+0xc],<bool:shouldAppend>
		0xc7, 0x44, 0x24, 0x08, 0x00, 0x00, 0x00, 0x00, // +0008  mov dword ptr [esp+0x8],<bool:isAlive>
		0xc7, 0x44, 0x24, 0x04, 0x00, 0x00, 0x00, 0x00, // +0010  mov dword ptr [esp+0x4],<int:team>
		0x89, 0x04, 0x24,                               // +0018  mov [esp],exx
		0xc7, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00,       // +001B  mov [ebp-0xXXX],0x00000000
		0xc7, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00,       // +0022  mov [ebp-0xXXX],0x00000000
		0xc7, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00,       // +0029  mov [ebp-0xXXX],0x00000000
		0xc7, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00,       // +0030  mov [ebp-0xXXX],0x00000000
		0xc7, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00,       // +0037  mov [ebp-0xXXX],0x00000000
		0xe8, 0x00, 0x00, 0x00, 0x00,                   // +003E  call CollectPlayers<CTFPlayer>
	};
	
	template<uint32_t OFF_MIN, uint32_t OFF_MAX, int TEAM, bool IS_ALIVE, bool SHOULD_APPEND, CollectPlayersFunc_t REPLACE_FUNC>
	struct CPatch_CollectPlayers_Caller1 : public CPatch
	{
		CPatch_CollectPlayers_Caller1(const char *func_name) : CPatch(sizeof(s_Buf_CollectPlayers_Caller1)), m_pszFuncName(func_name) {}
		
		virtual const char *GetFuncName() const override { return this->m_pszFuncName; }
		virtual uint32_t GetFuncOffMin() const override  { return OFF_MIN; }
		virtual uint32_t GetFuncOffMax() const override  { return OFF_MAX; }
		
		virtual bool GetVerifyInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			buf.CopyFrom(s_Buf_CollectPlayers_Caller1);
			
			buf.SetDword(0x00 + 4, (uint32_t)SHOULD_APPEND);
			buf.SetDword(0x08 + 4, (uint32_t)IS_ALIVE);
			buf.SetDword(0x10 + 4, (uint32_t)TEAM);
			
			/* allow any 3-bit source register code */
			mask[0x18 + 1] = 0b11000111;
			
			mask.SetDword(0x1b + 2, 0xfffff003);
			mask.SetDword(0x22 + 2, 0xfffff003);
			mask.SetDword(0x29 + 2, 0xfffff003);
			mask.SetDword(0x30 + 2, 0xfffff003);
			mask.SetDword(0x37 + 2, 0xfffff003);
			
			mask.SetDword(0x3e + 1, 0x00000000);
			
			return true;
		}
		
		virtual bool GetPatchInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			/* for now, replace the call instruction with 'int3' opcodes */
			buf .SetRange(0x3e, 5, 0xcc);
			mask.SetRange(0x3e, 5, 0xff);
			
			return true;
		}
		
		virtual bool AdjustPatchInfo(ByteBuf& buf) const override
		{
			auto src = (uint8_t *)this->GetActualLocation() + (0x3e + 5);
			auto dst = (uint8_t *)REPLACE_FUNC;
			
			ptrdiff_t off = (dst - src);
			
			/* set the opcode back to 'call' */
			buf[0x3e] = 0xe8;
			
			/* put the relative offset into place */
			buf.SetDword(0x3e + 1, off);
			
			ConColorMsg(Color(0x00, 0xff, 0x00, 0xff), "[CPatch_CollectPlayers_Caller1::AdjustPatchInfo] src: 0x%08x\n", (uintptr_t)src);
			ConColorMsg(Color(0x00, 0xff, 0x00, 0xff), "[CPatch_CollectPlayers_Caller1::AdjustPatchInfo] dst: 0x%08x\n", (uintptr_t)dst);
			ConColorMsg(Color(0x00, 0xff, 0x00, 0xff), "[CPatch_CollectPlayers_Caller1::AdjustPatchInfo] off: 0x%08x\n", (uintptr_t)off);
			ConColorMsg(Color(0x00, 0xff, 0x00, 0xff), "[CPatch_CollectPlayers_Caller1::AdjustPatchInfo] BUF DUMP:\n");
			buf.Dump();
			
			return true;
		}
		
	private:
		const char *m_pszFuncName;
	};
	
	
	constexpr uint8_t s_Buf_CollectPlayers_Caller2[] = {
		0x89, 0x04, 0x24,                               // +0000  mov [esp],exx
		0xc7, 0x44, 0x24, 0x0c, 0x00, 0x00, 0x00, 0x00, // +0003  mov dword ptr [esp+0xc],<bool:shouldAppend>
		0xc7, 0x44, 0x24, 0x08, 0x00, 0x00, 0x00, 0x00, // +000B  mov dword ptr [esp+0x8],<bool:isAlive>
		0xc7, 0x44, 0x24, 0x04, 0x00, 0x00, 0x00, 0x00, // +0013  mov dword ptr [esp+0x4],<int:team>
		0xa3, 0x00, 0x00, 0x00, 0x00,                   // +001B  mov ds:0xXXXXXXXX,eax
		0xc7, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00,       // +0020  mov [ebp-0xXXX],0x00000000
		0xc7, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00,       // +0027  mov [ebp-0xXXX],0x00000000
		0xc7, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00,       // +002E  mov [ebp-0xXXX],0x00000000
		0xc7, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00,       // +0035  mov [ebp-0xXXX],0x00000000
		0xc7, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00,       // +003C  mov [ebp-0xXXX],0x00000000
		0xe8, 0x00, 0x00, 0x00, 0x00,                   // +0043  call CollectPlayers<CTFPlayer>
	};
	
	template<uint32_t OFF_MIN, uint32_t OFF_MAX, int TEAM, bool IS_ALIVE, bool SHOULD_APPEND, CollectPlayersFunc_t REPLACE_FUNC>
	struct CPatch_CollectPlayers_Caller2 : public CPatch
	{
		CPatch_CollectPlayers_Caller2(const char *func_name) : CPatch(sizeof(s_Buf_CollectPlayers_Caller2)), m_pszFuncName(func_name) {}
		
		virtual const char *GetFuncName() const override { return this->m_pszFuncName; }
		virtual uint32_t GetFuncOffMin() const override  { return OFF_MIN; }
		virtual uint32_t GetFuncOffMax() const override  { return OFF_MAX; }
		
		virtual bool GetVerifyInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			buf.CopyFrom(s_Buf_CollectPlayers_Caller2);
			
			/* allow any 3-bit source register code */
			mask[0x00 + 1] = 0b11000111;
			
			buf.SetDword(0x03 + 4, (uint32_t)SHOULD_APPEND);
			buf.SetDword(0x0b + 4, (uint32_t)IS_ALIVE);
			buf.SetDword(0x13 + 4, (uint32_t)TEAM);
			
			mask.SetDword(0x1b + 1, 0x00000000);
			
			mask.SetDword(0x20 + 2, 0xfffff003);
			mask.SetDword(0x27 + 2, 0xfffff003);
			mask.SetDword(0x2e + 2, 0xfffff003);
			mask.SetDword(0x35 + 2, 0xfffff003);
			mask.SetDword(0x3c + 2, 0xfffff003);
			
			mask.SetDword(0x43 + 1, 0x00000000);
			
			return true;
		}
		
		virtual bool GetPatchInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			/* for now, replace the call instruction with 'int3' opcodes */
			buf .SetRange(0x43, 5, 0xcc);
			mask.SetRange(0x43, 5, 0xff);
			
			return true;
		}
		
		virtual bool AdjustPatchInfo(ByteBuf& buf) const override
		{
			auto src = (uint8_t *)this->GetActualLocation() + (0x43 + 5);
			auto dst = (uint8_t *)REPLACE_FUNC;
			
			ptrdiff_t off = (dst - src);
			
			/* set the opcode back to 'call' */
			buf[0x43] = 0xe8;
			
			/* put the relative offset into place */
			buf.SetDword(0x43 + 1, off);
			
			ConColorMsg(Color(0x00, 0xff, 0x00, 0xff), "[CPatch_CollectPlayers_Caller2::AdjustPatchInfo] src: 0x%08x\n", (uintptr_t)src);
			ConColorMsg(Color(0x00, 0xff, 0x00, 0xff), "[CPatch_CollectPlayers_Caller2::AdjustPatchInfo] dst: 0x%08x\n", (uintptr_t)dst);
			ConColorMsg(Color(0x00, 0xff, 0x00, 0xff), "[CPatch_CollectPlayers_Caller2::AdjustPatchInfo] off: 0x%08x\n", (uintptr_t)off);
			ConColorMsg(Color(0x00, 0xff, 0x00, 0xff), "[CPatch_CollectPlayers_Caller2::AdjustPatchInfo] BUF DUMP:\n");
			buf.Dump();
			
			return true;
		}
		
	private:
		const char *m_pszFuncName;
	};
	
	
	constexpr uint8_t s_Buf_CollectPlayers_Caller3[] = {
		0x01, 0x45, 0x00,                               // +0000  add [ebp-0xXX],eax
		0x8b, 0x45, 0x08,                               // +0003  mov eax,[ebp+this]
		0x8b, 0x80, 0x00, 0x00, 0x00, 0x00,             // +0006  mov eax,[eax+0xXXXX]
		0xc7, 0x44, 0x24, 0x0c, 0x00, 0x00, 0x00, 0x00, // +000C  mov dword ptr [esp+0xc],<bool:shouldAppend>
		0xc7, 0x44, 0x24, 0x08, 0x00, 0x00, 0x00, 0x00, // +0014  mov dword ptr [esp+0x8],<bool:isAlive>
		0xc7, 0x44, 0x24, 0x04, 0x00, 0x00, 0x00, 0x00, // +001C  mov dword ptr [esp+0x4],<int:team>
		0x01, 0x45, 0x00,                               // +0024  add [ebp-0xXX],eax
		0x8d, 0x45, 0x00,                               // +0027  lea eax,[ebp-0xXX]
		0x89, 0x04, 0x24,                               // +002A  mov [esp],exx
		0xe8, 0x00, 0x00, 0x00, 0x00,                   // +002D  call CollectPlayers<CTFPlayer>
	};
	
	template<uint32_t OFF_MIN, uint32_t OFF_MAX, int TEAM, bool IS_ALIVE, bool SHOULD_APPEND, CollectPlayersFunc_t REPLACE_FUNC>
	struct CPatch_CollectPlayers_Caller3 : public CPatch
	{
		CPatch_CollectPlayers_Caller3(const char *func_name) : CPatch(sizeof(s_Buf_CollectPlayers_Caller3)), m_pszFuncName(func_name) {}
		
		virtual const char *GetFuncName() const override { return this->m_pszFuncName; }
		virtual uint32_t GetFuncOffMin() const override  { return OFF_MIN; }
		virtual uint32_t GetFuncOffMax() const override  { return OFF_MAX; }
		
		virtual bool GetVerifyInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			buf.CopyFrom(s_Buf_CollectPlayers_Caller3);
			
			mask[0x00 + 2] = 0x00;
			
			mask.SetDword(0x06 + 2, 0x00000000);
			
			buf.SetDword(0x0c + 4, (uint32_t)SHOULD_APPEND);
			buf.SetDword(0x14 + 4, (uint32_t)IS_ALIVE);
			buf.SetDword(0x1c + 4, (uint32_t)TEAM);
			
			mask[0x24 + 2] = 0x00;
			mask[0x27 + 2] = 0x00;
			
			/* allow any 3-bit source register code */
			mask[0x2a + 1] = 0b11000111;
			
			mask.SetDword(0x2d + 1, 0x00000000);
			
			return true;
		}
		
		virtual bool GetPatchInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			/* for now, replace the call instruction with 'int3' opcodes */
			buf .SetRange(0x2d, 5, 0xcc);
			mask.SetRange(0x2d, 5, 0xff);
			
			return true;
		}
		
		virtual bool AdjustPatchInfo(ByteBuf& buf) const override
		{
			auto src = (uint8_t *)this->GetActualLocation() + (0x2d + 5);
			auto dst = (uint8_t *)REPLACE_FUNC;
			
			ptrdiff_t off = (dst - src);
			
			/* set the opcode back to 'call' */
			buf[0x2d] = 0xe8;
			
			/* put the relative offset into place */
			buf.SetDword(0x2d + 1, off);
			
			ConColorMsg(Color(0x00, 0xff, 0x00, 0xff), "[CPatch_CollectPlayers_Caller3::AdjustPatchInfo] src: 0x%08x\n", (uintptr_t)src);
			ConColorMsg(Color(0x00, 0xff, 0x00, 0xff), "[CPatch_CollectPlayers_Caller3::AdjustPatchInfo] dst: 0x%08x\n", (uintptr_t)dst);
			ConColorMsg(Color(0x00, 0xff, 0x00, 0xff), "[CPatch_CollectPlayers_Caller3::AdjustPatchInfo] off: 0x%08x\n", (uintptr_t)off);
			ConColorMsg(Color(0x00, 0xff, 0x00, 0xff), "[CPatch_CollectPlayers_Caller3::AdjustPatchInfo] BUF DUMP:\n");
			buf.Dump();
			
			return true;
		}
		
	private:
		const char *m_pszFuncName;
	};
	
	
	constexpr uint8_t s_Buf_RadiusSpyScan[] = {
		0x8b, 0x87, 0x00, 0x00, 0x00, 0x00, // +0000  mov exx,[exx+offsetof(CTFPlayerShared, m_pOuter)]
		0x89, 0x04, 0x24,                   // +0006  mov [esp],exx
		0xe8, 0x00, 0x00, 0x00, 0x00,       // +0009  call CBaseEntity::GetTeamNumber
		0x83, 0xf8, 0x02,                   // +000E  cmp eax,TF_TEAM_RED
	};
	
	struct CPatch_RadiusSpyScan : public CPatch
	{
		CPatch_RadiusSpyScan() : CPatch(sizeof(s_Buf_RadiusSpyScan)) {}
		
		virtual const char *GetFuncName() const override { return "CTFPlayerShared::RadiusSpyScan"; }
		virtual uint32_t GetFuncOffMin() const override  { return 0x0000; }
		virtual uint32_t GetFuncOffMax() const override  { return 0x0020; } // @ +0x000c
		
		virtual bool GetVerifyInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			buf.CopyFrom(s_Buf_RadiusSpyScan);
			
			int off_CTFPlayerShared_m_pOuter;
			if (!Prop::FindOffset(off_CTFPlayerShared_m_pOuter, "CTFPlayerShared", "m_pOuter")) return false;
			buf.SetDword(0x00 + 2, off_CTFPlayerShared_m_pOuter);
			
			/* allow any 3-bit source or destination register code */
			mask[0x00 + 1] = 0b11000000;
			
			/* allow any 3-bit source register code */
			mask[0x06 + 1] = 0b11000111;
			
			mask.SetDword(0x09 + 1, 0x00000000);
			
			return true;
		}
		
		virtual bool GetPatchInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			/* replace 'cmp eax,TF_TEAM_RED' with 'cmp eax,eax; nop' */
			buf[0x0e + 0] = 0x39;
			buf[0x0e + 1] = 0xc0;
			buf[0x0e + 2] = 0x90;
			
			mask.SetRange(0x0e, 3, 0xff);
			
			return true;
		}
	};
	
	
	ConVar cvar_max("sig_mvm_jointeam_blue_allow_max", "-1", FCVAR_NOTIFY,
		"Blue humans in MvM: max humans to allow on blue team (-1 for no limit)");
	
	ConVar cvar_flag_pickup("sig_mvm_bluhuman_flag_pickup", "0", FCVAR_NOTIFY,
		"Blue humans in MvM: allow picking up the flag");
	ConVar cvar_flag_capture("sig_mvm_bluhuman_flag_capture", "0", FCVAR_NOTIFY,
		"Blue humans in MvM: allow scoring a flag Capture by carrying it to the capture zone");
	
	ConVar cvar_spawn_protection("sig_mvm_bluhuman_spawn_protection", "1", FCVAR_NOTIFY,
		"Blue humans in MvM: enable spawn protection invulnerability");
	ConVar cvar_spawn_no_shoot("sig_mvm_bluhuman_spawn_noshoot", "1", FCVAR_NOTIFY,
		"Blue humans in MvM: when spawn protection invulnerability is enabled, disallow shooting from spawn");
	
	ConVar cvar_infinite_cloak("sig_mvm_bluhuman_infinite_cloak", "1", FCVAR_NOTIFY,
		"Blue humans in MvM: enable infinite spy cloak meter");
	ConVar cvar_infinite_cloak_deadringer("sig_mvm_bluhuman_infinite_cloak_deadringer", "0", FCVAR_NOTIFY,
		"Blue humans in MvM: enable infinite spy cloak meter (Dead Ringer)");
	
	
	// TODO: probably need to add in a check for TF_COND_REPROGRAMMED here and:
	// - exclude humans on TF_TEAM_BLUE who are in TF_COND_REPROGRAMMED
	// - include humans on TF_TEAM_RED who are in TF_COND_REPROGRAMMED
	
	bool IsMvMBlueHuman(CTFPlayer *player)
	{
		if (player == nullptr)                       return false;
		if (player->GetTeamNumber() != TF_TEAM_BLUE) return false;
		if (player->IsBot())                         return false;
		if (!TFGameRules()->IsMannVsMachineMode())   return false;
		
		return true;
	}
	
	int GetMvMBlueHumanCount()
	{
		int count = 0;
		
		ForEachTFPlayer([&](CTFPlayer *player){
			if (IsMvMBlueHuman(player)) {
				++count;
			}
		});
		
		return count;
	}
	
	bool IsInBlueSpawnRoom(CTFPlayer *player)
	{
		player->UpdateLastKnownArea();
		auto area = static_cast<CTFNavArea *>(player->GetLastKnownArea());
		return (area != nullptr && area->HasTFAttributes(BLUE_SPAWN_ROOM));
	}
	
	
	static int CollectPlayers_RedAndBlue(CUtlVector<CTFPlayer *> *playerVector, int team, bool isAlive, bool shouldAppend)
	{
		(void) CollectPlayers(playerVector, TF_TEAM_RED,  isAlive, shouldAppend);
		return CollectPlayers(playerVector, TF_TEAM_BLUE, isAlive, true);
	}
	
	static int CollectPlayers_RedAndBlue_IsBot(CUtlVector<CTFPlayer *> *playerVector, int team, bool isAlive, bool shouldAppend)
	{
		CUtlVector<CTFPlayer *> tempVector;
		CollectPlayers(&tempVector, TF_TEAM_RED,  isAlive, true);
		CollectPlayers(&tempVector, TF_TEAM_BLUE, isAlive, true);
		
		if (!shouldAppend) {
			playerVector->RemoveAll();
		}
		
		for (auto player : tempVector) {
			if (player->IsBot()) {
				playerVector->AddToTail(player);
			}
		}
		
		return playerVector->Count();
	}
	
	static int CollectPlayers_RedAndBlue_NotBot(CUtlVector<CTFPlayer *> *playerVector, int team, bool isAlive, bool shouldAppend)
	{
		CUtlVector<CTFPlayer *> tempVector;
		CollectPlayers(&tempVector, TF_TEAM_RED,  isAlive, true);
		CollectPlayers(&tempVector, TF_TEAM_BLUE, isAlive, true);
		
		if (!shouldAppend) {
			playerVector->RemoveAll();
		}
		
		for (auto player : tempVector) {
			if (!player->IsBot()) {
				playerVector->AddToTail(player);
			}
		}
		
		return playerVector->Count();
	}
	
	
	DETOUR_DECL_MEMBER(int, CTFGameRules_GetTeamAssignmentOverride, CTFPlayer *pPlayer, int iWantedTeam, bool b1)
	{
		/* it's important to let the call happen, because pPlayer->m_nCurrency
		 * is set to its proper value in the call (stupid, but whatever) */
		auto iResult = DETOUR_MEMBER_CALL(CTFGameRules_GetTeamAssignmentOverride)(pPlayer, iWantedTeam, b1);
		
		// debug message for the "sometimes bots don't get put on TEAM_SPECTATOR properly at wave end" situation
		if (TFGameRules()->IsMannVsMachineMode() && pPlayer->IsBot() && iResult != iWantedTeam) {
			DevMsg("[CTFGameRules::GetTeamAssignmentOverride] Bot [ent:%d userid:%d name:\"%s\"]: on team %d, wanted %d, forced onto %d!\n",
				ENTINDEX(pPlayer), pPlayer->GetUserID(), pPlayer->GetPlayerName(), pPlayer->GetTeamNumber(), iWantedTeam, iResult);
			BACKTRACE();
		}
		
		if (TFGameRules()->IsMannVsMachineMode() && !pPlayer->IsBot() && iWantedTeam == TF_TEAM_BLUE && iResult != iWantedTeam) {
			// NOTE: if the pop file had custom param 'AllowJoinTeamBlue 1', then disregard admin-only restrictions
			extern ConVar cvar_adminonly;
			if (Mod::Pop::PopMgr_Extensions::PopFileIsOverridingJoinTeamBlueConVarOn() ||
				!cvar_adminonly.GetBool() || PlayerIsSMAdmin(pPlayer)) {
				if (cvar_max.GetInt() < 0 || GetMvMBlueHumanCount() < cvar_max.GetInt()) {
					DevMsg("Player #%d \"%s\" requested team %d but was forced onto team %d; overriding to allow them to join team %d.\n",
						ENTINDEX(pPlayer), pPlayer->GetPlayerName(), iWantedTeam, iResult, iWantedTeam);
					iResult = iWantedTeam;
				} else {
					DevMsg("Player #%d \"%s\" requested team %d but was forced onto team %d; would have overridden to allow joining team %d but limit has been met.\n",
						ENTINDEX(pPlayer), pPlayer->GetPlayerName(), iWantedTeam, iResult, iWantedTeam);
					ClientMsg(pPlayer, "Cannot join team blue: the maximum number of human players on blue team has already been met.\n");
				}
			} else {
				ClientMsg(pPlayer, "You are not authorized to use this command because you are not a SourceMod admin. Sorry.\n");
			}
		}
		
		return iResult;
	}
	
	
	DETOUR_DECL_STATIC(CTFReviveMarker *, CTFReviveMarker_Create, CTFPlayer *player)
	{
		if (IsMvMBlueHuman(player)) {
			return nullptr;
		}
		
		return DETOUR_STATIC_CALL(CTFReviveMarker_Create)(player);
	}
	
	
	DETOUR_DECL_MEMBER(bool, CTFPlayer_ClientCommand, const CCommand& args)
	{
		auto player = reinterpret_cast<CTFPlayer *>(this);
		
		if (FStrEq(args[0], "upgrade")) {
			if (IsMvMBlueHuman(player) && IsInBlueSpawnRoom(player)) {
				player->m_Shared->m_bInUpgradeZone = true;
			}
			
			return true;
		}
		
		return DETOUR_MEMBER_CALL(CTFPlayer_ClientCommand)(args);
	}
	
	DETOUR_DECL_MEMBER(void, CTFPlayer_OnNavAreaChanged, CNavArea *enteredArea, CNavArea *leftArea)
	{
		auto player = reinterpret_cast<CTFPlayer *>(this);
		
		DETOUR_MEMBER_CALL(CTFPlayer_OnNavAreaChanged)(enteredArea, leftArea);
		
		if (IsMvMBlueHuman(player) &&
			(enteredArea == nullptr ||  static_cast<CTFNavArea *>(enteredArea)->HasTFAttributes(BLUE_SPAWN_ROOM)) &&
			(leftArea    == nullptr || !static_cast<CTFNavArea *>(leftArea)   ->HasTFAttributes(BLUE_SPAWN_ROOM))) {
			player->m_Shared->m_bInUpgradeZone = false;
		}
	}
	
	DETOUR_DECL_MEMBER(void, CTFGameRules_ClientCommandKeyValues, edict_t *pEntity, KeyValues *pKeyValues)
	{
		if (FStrEq(pKeyValues->GetName(), "MvM_UpgradesDone")) {
			CTFPlayer *player = ToTFPlayer(GetContainingEntity(pEntity));
			if (IsMvMBlueHuman(player)) {
				player->m_Shared->m_bInUpgradeZone = false;
			}
		}
		
		DETOUR_MEMBER_CALL(CTFGameRules_ClientCommandKeyValues)(pEntity, pKeyValues);
	}
	
	
	DETOUR_DECL_MEMBER(bool, CTFPlayer_IsAllowedToPickUpFlag)
	{
		auto player = reinterpret_cast<CTFPlayer *>(this);
		
		if (IsMvMBlueHuman(player) && !cvar_flag_pickup.GetBool()) {
			return false;
		}
		
		return DETOUR_MEMBER_CALL(CTFPlayer_IsAllowedToPickUpFlag)();
	}
	
	DETOUR_DECL_MEMBER(void, CCaptureZone_ShimTouch, CBaseEntity *pOther)
	{
		auto zone = reinterpret_cast<CCaptureZone *>(this);
		
		[&]{
			if (zone->IsDisabled()) return;
			
			CTFPlayer *player = ToTFPlayer(pOther);
			if (player == nullptr)       return;
			if (!IsMvMBlueHuman(player)) return;
			
			if (!cvar_flag_capture.GetBool()) return;
			
			if (!player->HasItem()) return;
			CTFItem *item = player->GetItem();
			
			if (item->GetItemID() != TF_ITEM_CAPTURE_FLAG) return;
			
			auto flag = dynamic_cast<CCaptureFlag *>(item);
			if (flag == nullptr) return;
			
			// skipping flag->m_nType check out of laziness
			
			if (!TFGameRules()->FlagsMayBeCapped()) return;
			
			#warning Should have flag->IsCaptured() check in here
		//	if (flag->IsCaptured() || zone->GetTeamNumber() == TEAM_UNASSIGNED || player->GetTeamNumber() == TEAM_UNASSIGNED || zone->GetTeamNumber() == player->GetTeamNumber()) {
			if (zone->GetTeamNumber() == TEAM_UNASSIGNED || player->GetTeamNumber() == TEAM_UNASSIGNED || zone->GetTeamNumber() == player->GetTeamNumber()) {
				zone->Capture(player);
			}
		}();
		
		DETOUR_MEMBER_CALL(CCaptureZone_ShimTouch)(pOther);
	}
	
	
//	DETOUR_DECL_MEMBER(void, CPlayerMove_StartCommand, CBasePlayer *player, CUserCmd *ucmd)
//	{
//		DETOUR_MEMBER_CALL(CPlayerMove_StartCommand)(player, ucmd);
//		
//		DevMsg("CPlayerMove::StartCommand(#%d): buttons = %08x\n", ENTINDEX(player), ucmd->buttons);
//		
//		/* ideally we'd either do this or not do this based on the value of
//		 * CanBotsAttackWhileInSpawnRoom in g_pPopulationManager, but tracking
//		 * down the offset of that boolean is more work than it's worth */
//		CTFPlayer *tfplayer = ToTFPlayer(player);
//		if (cvar_spawn_protection.GetBool() && cvar_spawn_no_shoot.GetBool() && IsMvMBlueHuman(tfplayer) && IsInBlueSpawnRoom(tfplayer)) {
//			ucmd->buttons &= ~(IN_ATTACK | IN_ATTACK2 | IN_ATTACK3);
//			DevMsg("- stripped attack buttons: %08x\n", ucmd->buttons);
//		}
//	}
	
	
	RefCount rc_CTFGameRules_FireGameEvent__teamplay_round_start;
	DETOUR_DECL_MEMBER(void, CTFGameRules_FireGameEvent, IGameEvent *event)
	{
		SCOPED_INCREMENT_IF(rc_CTFGameRules_FireGameEvent__teamplay_round_start,
			(event != nullptr && strcmp(event->GetName(), "teamplay_round_start") == 0));
		DETOUR_MEMBER_CALL(CTFGameRules_FireGameEvent)(event);
	}
	
	DETOUR_DECL_STATIC(int, CollectPlayers_CTFPlayer, CUtlVector<CTFPlayer *> *playerVector, int team, bool isAlive, bool shouldAppend)
	{
		if (rc_CTFGameRules_FireGameEvent__teamplay_round_start > 0 && (team == TF_TEAM_BLUE && !isAlive && !shouldAppend)) {
			/* collect players on BOTH teams */
			return CollectPlayers_RedAndBlue_IsBot(playerVector, team, isAlive, shouldAppend);
		}
		
		return DETOUR_STATIC_CALL(CollectPlayers_CTFPlayer)(playerVector, team, isAlive, shouldAppend);
	}
	
	
	RefCount rc_CTFPlayerShared_RadiusSpyScan;
	int radius_spy_scan_teamnum = TEAM_INVALID;
	DETOUR_DECL_MEMBER(void, CTFPlayerShared_RadiusSpyScan)
	{
		auto player = reinterpret_cast<CTFPlayerShared *>(this)->GetOuter();
		
		/* instead of restricting the ability to team red, restrict it to human players */
		if (player->IsBot()) {
			return;
		}
		
		SCOPED_INCREMENT(rc_CTFPlayerShared_RadiusSpyScan);
		radius_spy_scan_teamnum = player->GetTeamNumber();
		
		DETOUR_MEMBER_CALL(CTFPlayerShared_RadiusSpyScan)();
	}
	
	static int CollectPlayers_RadiusSpyScan(CUtlVector<CTFPlayer *> *playerVector, int team, bool isAlive, bool shouldAppend)
	{
		/* sanity checks */
		assert(rc_CTFPlayerShared_RadiusSpyScan > 0);
		assert(radius_spy_scan_teamnum == TF_TEAM_RED || radius_spy_scan_teamnum == TF_TEAM_BLUE);
		
		/* rather than always affecting blue players, affect players on the opposite team of the player with the ability */
		return CollectPlayers(playerVector, GetEnemyTeam(radius_spy_scan_teamnum), isAlive, shouldAppend);
	}
	
	
	/* log cases where bots are spawning at weird times (not while the wave is running) */
	DETOUR_DECL_MEMBER(void, CTFBot_Spawn)
	{
		auto bot = reinterpret_cast<CTFBot *>(this);
		
		DETOUR_MEMBER_CALL(CTFBot_Spawn)();
		
		if (TFGameRules()->IsMannVsMachineMode()) {
			// ========= MANN VS MACHINE MODE ROUND STATE TRANSITIONS ==========
			// [CPopulationManager::JumpToWave]                     --> PREROUND
			// [CPopulationManager::StartCurrentWave]               --> RND_RUNNING
			// [CPopulationManager::WaveEnd]                        --> BETWEEN_RNDS
			// [CPopulationManager::WaveEnd]                        --> GAME_OVER
			// -----------------------------------------------------------------
			// [CTeamplayRoundBasedRules::CTeamplayRoundBasedRules] --> PREGAME
			// [CTeamplayRoundBasedRules::State_Think_INIT]         --> PREGAME
			// [CTeamplayRoundBasedRules::State_Think_RND_RUNNING]  --> PREGAME
			// [CTeamplayRoundBasedRules::State_Think_PREGAME]      --> STARTGAME
			// [CTeamplayRoundBasedRules::CheckReadyRestart]        --> RESTART
			// [CTeamplayRoundBasedRules::State_Enter_RESTART]      --> PREROUND
			// [CTeamplayRoundBasedRules::State_Think_STARTGAME]    --> PREROUND
			// [CTeamplayRoundBasedRules::CheckWaitingForPlayers]   --> PREROUND
			// [CTeamplayRoundBasedRules::State_Think_PREROUND]     --> RND_RUNNING
			// [CTeamplayRoundBasedRules::State_Enter_PREROUND]     --> BETWEEN_RNDS
			// [CTeamplayRoundBasedRules::State_Think_TEAM_WIN]     --> PREROUND
			// [CTeamplayRoundBasedRules::State_Think_TEAM_WIN]     --> GAME_OVER
			// =================================================================
			
			bool sketchy;
			switch (TFGameRules()->State_Get()) {
				case GR_STATE_INIT:         sketchy = true;  break;
				case GR_STATE_PREGAME:      sketchy = true;  break;
				case GR_STATE_STARTGAME:    sketchy = false; break;
				case GR_STATE_PREROUND:     sketchy = true;  break;
				case GR_STATE_RND_RUNNING:  sketchy = false; break;
				case GR_STATE_TEAM_WIN:     sketchy = true;  break;
				case GR_STATE_RESTART:      sketchy = true;  break;
				case GR_STATE_STALEMATE:    sketchy = true;  break;
				case GR_STATE_GAME_OVER:    sketchy = true;  break;
				case GR_STATE_BONUS:        sketchy = true;  break;
				case GR_STATE_BETWEEN_RNDS: sketchy = true;  break;
				default:                    sketchy = true;  break;
			}
			
			if (sketchy) {
				DevMsg("[CTFBot::Spawn] Bot [ent:%d userid:%d name:\"%s\"]: spawned during game state %s!\n",
					ENTINDEX(bot), bot->GetUserID(), bot->GetPlayerName(), GetRoundStateName(TFGameRules()->State_Get()));
				BACKTRACE();
			}
		}
	}
	
	
	// TODO: fix fast engie re-deploy only working on red
	
	// TODO: on mod disable, force blue humans back onto red team
	// - use IsMvMBlueHuman
	// - beware of the call order between IMod::OnDisable and when the patches/detours are actually disabled...
	
	
	class CMod : public IMod, public IFrameUpdateListener
	{
	public:
		CMod() : IMod("MvM:JoinTeam_Blue_Allow")
		{
			/* change the team assignment rules */
			MOD_ADD_DETOUR_MEMBER(CTFGameRules_GetTeamAssignmentOverride, "CTFGameRules::GetTeamAssignmentOverride");
			
			/* don't drop reanimators for blue humans */
			MOD_ADD_DETOUR_STATIC(CTFReviveMarker_Create, "CTFReviveMarker::Create");
			
			/* enable upgrading in blue spawn zones via "upgrade" client command */
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_ClientCommand,                "CTFPlayer::ClientCommand");
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_OnNavAreaChanged,             "CTFPlayer::OnNavAreaChanged");
			MOD_ADD_DETOUR_MEMBER(CTFGameRules_ClientCommandKeyValues,    "CTFGameRules::ClientCommandKeyValues");
			
			/* allow flag pickup and capture depending on convar values */
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_IsAllowedToPickUpFlag, "CTFPlayer::IsAllowedToPickUpFlag");
			MOD_ADD_DETOUR_MEMBER(CCaptureZone_ShimTouch,          "CCaptureZone::ShimTouch");
			
			/* disallow attacking while in the blue spawn room */
		//	MOD_ADD_DETOUR_MEMBER(CPlayerMove_StartCommand, "CPlayerMove::StartCommand");
			
			/* fix hardcoded teamnum check when forcing bots to move to team spec at round change */
			MOD_ADD_DETOUR_MEMBER(CTFGameRules_FireGameEvent, "CTFGameRules::FireGameEvent");
			MOD_ADD_DETOUR_STATIC(CollectPlayers_CTFPlayer,   "CollectPlayers<CTFPlayer>");
			
			/* fix hardcoded teamnum check when clearing MvM checkpoints */
			this->AddPatch(new CPatch_CollectPlayers_Caller2<0x0000, 0x0100, TF_TEAM_RED, false, false, CollectPlayers_RedAndBlue>("CPopulationManager::ClearCheckpoint"));
			
			/* fix hardcoded teamnum check when restoring MvM checkpoints */
			this->AddPatch(new CPatch_CollectPlayers_Caller1<0x0000, 0x0200, TF_TEAM_RED, false, false, CollectPlayers_RedAndBlue>("CPopulationManager::RestoreCheckpoint"));
			
			/* fix hardcoded teamnum check when restoring player currency */
			this->AddPatch(new CPatch_CollectPlayers_Caller3<0x0000, 0x0100, TF_TEAM_RED, false, false, CollectPlayers_RedAndBlue>("CPopulationManager::RestorePlayerCurrency"));
			
			/* fix hardcoded teamnum check when respawning dead players and resetting their sentry stats at wave end */
			this->AddPatch(new CPatch_CollectPlayers_Caller1<0x0000, 0x0400, TF_TEAM_RED, false, false, CollectPlayers_RedAndBlue_NotBot>("CWave::WaveCompleteUpdate"));
			
			/* fix hardcoded teamnum checks in the radius spy scan ability */
			MOD_ADD_DETOUR_MEMBER(CTFPlayerShared_RadiusSpyScan, "CTFPlayerShared::RadiusSpyScan");
			this->AddPatch(new CPatch_CollectPlayers_Caller1<0x0000, 0x0100, TF_TEAM_BLUE, true, false, CollectPlayers_RadiusSpyScan>("CTFPlayerShared::RadiusSpyScan"));
			this->AddPatch(new CPatch_RadiusSpyScan());
			
			/* this is purely for debugging the blue-robots-spawning-between-waves situation */
			MOD_ADD_DETOUR_MEMBER(CTFBot_Spawn, "CTFBot::Spawn");
		}
		
		virtual bool ShouldReceiveFrameEvents() const override { return this->IsEnabled(); }
		
		virtual void FrameUpdatePostEntityThink() override
		{
			if (TFGameRules()->IsMannVsMachineMode()) {
				ForEachTFPlayer([](CTFPlayer *player){
					if (player->GetTeamNumber() != TF_TEAM_BLUE) return;
					if (player->IsBot())                         return;
					
					if (cvar_spawn_protection.GetBool()) {
						if (IsInBlueSpawnRoom(player)) {
							player->m_Shared->AddCond(TF_COND_INVULNERABLE_HIDE_UNLESS_DAMAGED, 0.500f);
							
							if (cvar_spawn_no_shoot.GetBool()) {
								player->m_Shared->m_flStealthNoAttackExpire = gpGlobals->curtime + 0.500f;
								// alternative method: set m_Shared->m_bFeignDeathReady to true
							}
						}
					}
					
					if (cvar_infinite_cloak.GetBool()) {
						bool should_refill_cloak = true;
						
						if (!cvar_infinite_cloak_deadringer.GetBool()) {
							/* check for attribute "set cloak is feign death" */
							auto invis = rtti_cast<CTFWeaponInvis *>(player->Weapon_GetWeaponByType(TF_WPN_TYPE_ITEM1));
							if (invis != nullptr && CAttributeManager::AttribHookValue<int>(0, "set_weapon_mode", invis) == 1) {
								should_refill_cloak = false;
							}
						}
						
						if (should_refill_cloak) {
							player->m_Shared->m_flCloakMeter = 100.0f;
						}
					}
				});
			}
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_mvm_jointeam_blue_allow", "0", FCVAR_NOTIFY,
		"Mod: permit client command 'jointeam blue' from human players",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue){
			s_Mod.Toggle(static_cast<ConVar *>(pConVar)->GetBool());
		});
	
	/* default: admin-only mode ENABLED */
	ConVar cvar_adminonly("sig_mvm_jointeam_blue_allow_adminonly", "1", FCVAR_NOTIFY,
		"Mod: restrict this mod's functionality to SM admins only"
		" [NOTE: missions with WaveSchedule param AllowJoinTeamBlue 1 will OVERRIDE this and allow non-admins for the duration of the mission]");
}
