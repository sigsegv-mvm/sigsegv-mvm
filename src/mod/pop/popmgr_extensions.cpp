#include "mod.h"
#include "stub/entities.h"
#include "stub/projectiles.h"
#include "stub/tfbot.h"
#include "stub/gamerules.h"
#include "stub/populators.h"
#include "stub/misc.h"
#include "util/iterate.h"
#include "util/scope.h"
#include "mod/pop/kv_conditional.h"


namespace Mod_Pop_PopMgr_Extensions
{
	template<typename T>
	class IPopOverride
	{
	public:
		virtual ~IPopOverride() {}
		
		void Reset()
		{
			if (this->m_bOverridden) {
				this->Restore();
				this->m_bOverridden = false;
			}
		}
		void Set(const T& val)
		{
			if (!this->m_bOverridden) {
				this->Backup();
				this->m_bOverridden = true;
			}
			this->SetValue(val);
		}
		T Get() { return this->GetValue(); }
		
	protected:
		virtual T GetValue() = 0;
		virtual void SetValue(const T& val) = 0;
		
	private:
		void Backup()
		{
			this->m_Backup = this->GetValue();
		}
		void Restore()
		{
			this->SetValue(this->m_Backup);
		}
		
		bool m_bOverridden = false;
		T m_Backup;
	};
	
	template<typename T> T ConVar_GetValue(const ConVarRef& cvar);
	template<> bool        ConVar_GetValue<bool>       (const ConVarRef& cvar) { return cvar.GetBool();   }
	template<> int         ConVar_GetValue<int>        (const ConVarRef& cvar) { return cvar.GetInt();    }
	template<> float       ConVar_GetValue<float>      (const ConVarRef& cvar) { return cvar.GetFloat();  }
	template<> std::string ConVar_GetValue<std::string>(const ConVarRef& cvar) { return cvar.GetString(); }
	
	template<typename T> void ConVar_SetValue(ConVarRef& cvar, const T& val) { cvar.SetValue(val); }
	template<> void ConVar_SetValue(ConVarRef& cvar, const std::string& val) { cvar.SetValue(val.c_str()); }
	
	template<typename T>
	class CPopOverride_ConVar : public IPopOverride<T>
	{
	public:
		CPopOverride_ConVar(const char *name) :
			m_pszConVarName(name) {}
		
		virtual T GetValue() override { return ConVar_GetValue<T>(MyConVar()); }
		
		virtual void SetValue(const T& val) override
		{
			/* set the ConVar value in a manner that circumvents:
			 * - FCVAR_NOTIFY notifications
			 * - minimum value limits
			 * - maximum value limits
			 */
			
			int old_flags   = MyConVar().Ref_Flags();
			bool old_hasmin = MyConVar().Ref_HasMin();
			bool old_hasmax = MyConVar().Ref_HasMax();
			
			MyConVar().Ref_Flags() &= ~FCVAR_NOTIFY;
			MyConVar().Ref_HasMin() = false;
			MyConVar().Ref_HasMax() = false;
			
			ConVar_SetValue<T>(MyConVar(), val);
			
			MyConVar().Ref_Flags()  = old_flags;
			MyConVar().Ref_HasMin() = old_hasmin;
			MyConVar().Ref_HasMax() = old_hasmax;
		}
		
	private:
		ConVarRef& MyConVar()
		{
			if (this->m_pConVar == nullptr) {
				this->m_pConVar = std::unique_ptr<ConVarRef>(new ConVarRef(this->m_pszConVarName));
			}
			return *(this->m_pConVar);
		}
		
		const char *m_pszConVarName;
		std::unique_ptr<ConVarRef> m_pConVar;
	};
	
	
	struct CPopOverride_MedievalMode : public IPopOverride<bool>
	{
		virtual bool GetValue() override                { return TFGameRules()->IsInMedievalMode(); }
		virtual void SetValue(const bool& val) override { TFGameRules()->Set_m_bPlayingMedieval(val); }
	};
	
	
	class CPopOverride_CustomUpgradesFile : public IPopOverride<std::string>
	{
	public:
		virtual std::string GetValue() override
		{
			std::string val = TFGameRules()->GetCustomUpgradesFile();
			
			if (val == "") {
				val = "scripts/items/mvm_upgrades.txt";
			}
			
			return val;
		}
		
		virtual void SetValue(const std::string& val) override
		{
			std::string real_val = val;
			
			if (real_val == "") {
				real_val = "scripts/items/mvm_upgrades.txt";
			}
			
			if (this->GetValue() != real_val) {
				ConColorMsg(Color(0x00, 0xff, 0xff, 0xff), "CPopOverride_CustomUpgradesFile: SetCustomUpgradesFile(\"%s\")\n", real_val.c_str());
				TFGameRules()->SetCustomUpgradesFile(real_val.c_str());
			}
		}
	};
	
	
	class ExtraTankPath
	{
	public:
		ExtraTankPath(const char *name) : m_strName(name) {}
		
		~ExtraTankPath()
		{
			for (CPathTrack *node : this->m_PathNodes) {
				if (node != nullptr) {
					node->Remove();
				}
			}
		}
		
		bool AddNode(const Vector& origin)
		{
			assert(!this->m_bSpawned);
			
			auto node = rtti_cast<CPathTrack *>(CreateEntityByName("path_track"));
			if (node == nullptr) return false;
			
			CFmtStr name("%s_%zu", this->m_strName.c_str(), this->m_PathNodes.size() + 1);
			node->SetName(AllocPooledString(name));
			
			node->SetAbsOrigin(origin);
			node->SetAbsAngles(vec3_angle);
			
			node->m_eOrientationType = 1;
			
			this->m_PathNodes.emplace_back(node);
			
			DevMsg("ExtraTankPath: AddNode [ % 7.1f % 7.1f % 7.1f ] \"%s\"\n", VectorExpand(origin), name.Get());
			return true;
		}
		
		void SpawnNodes()
		{
			assert(!this->m_bSpawned);
			
			/* connect up the nodes' target links */
			for (auto it = this->m_PathNodes.begin(); (it + 1) != this->m_PathNodes.end(); ++it) {
				CPathTrack *curr = *it;
				CPathTrack *next = *(it + 1);
				
				DevMsg("ExtraTankPath: Link \"%s\" -> \"%s\"\n", STRING(curr->GetEntityName()), STRING(next->GetEntityName()));
				curr->m_target = next->GetEntityName();
			}
			
			/* now call Spawn() on each node */
			for (CPathTrack *node : this->m_PathNodes) {
				DevMsg("ExtraTankPath: Spawn \"%s\"\n", STRING(node->GetEntityName()));
				node->Spawn();
			}
			
			/* now call Activate() on each node */
			for (CPathTrack *node : this->m_PathNodes) {
				DevMsg("ExtraTankPath: Activate \"%s\"\n", STRING(node->GetEntityName()));
				node->Activate();
			}
			
			this->m_bSpawned = true;
		}
		
	private:
		std::string m_strName;
		std::vector<CHandle<CPathTrack>> m_PathNodes;
		bool m_bSpawned = false;
	};
	
	
	struct PopState
	{
		PopState() :
			m_SpellsEnabled           ("tf_spells_enabled"),
			m_GrapplingHook           ("tf_grapplinghook_enable"),
			m_RespecEnabled           ("tf_mvm_respec_enabled"),
			m_RespecLimit             ("tf_mvm_respec_limit"),
			m_BonusRatioHalf          ("tf_mvm_currency_bonus_ratio_min"),
			m_BonusRatioFull          ("tf_mvm_currency_bonus_ratio_max"),
			m_FixedBuybacks           ("tf_mvm_buybacks_method"),
			m_BuybacksPerWave         ("tf_mvm_buybacks_per_wave"),
			m_DeathPenalty            ("tf_mvm_death_penalty"),
			m_SentryBusterFriendlyFire("tf_bot_suicide_bomb_friendly_fire"),
			m_BotPushaway             ("tf_avoidteammates_pushaway"),
			m_HumansMustJoinTeam      ("mp_humans_must_join_team"),
			m_AllowJoinTeamBlue       ("sig_mvm_jointeam_blue_allow"),
			m_AllowJoinTeamBlueMax    ("sig_mvm_jointeam_blue_allow_max"),
			m_BluHumanFlagPickup      ("sig_mvm_bluhuman_flag_pickup"),
			m_BluHumanFlagCapture     ("sig_mvm_bluhuman_flag_capture"),
			m_SetCreditTeam           ("sig_mvm_set_credit_team"),
			m_EnableDominations       ("sig_mvm_dominations")
		{
			this->Reset();
		}
		
		void Reset()
		{
			this->m_bGiantsDropRareSpells   = false;
			this->m_flSpellDropRateCommon   = 1.00f;
			this->m_flSpellDropRateGiant    = 1.00f;
			this->m_bNoReanimators          = false;
			this->m_bNoMvMDeathTune         = false;
			this->m_bSniperHideLasers       = false;
			this->m_bSniperAllowHeadshots   = false;
			this->m_bDisableUpgradeStations = false;
			this->m_flRemoveGrapplingHooks  = -1.0f;
			this->m_bReverseWinConditions   = false;
			
			this->m_MedievalMode            .Reset();
			this->m_SpellsEnabled           .Reset();
			this->m_GrapplingHook           .Reset();
			this->m_RespecEnabled           .Reset();
			this->m_RespecLimit             .Reset();
			this->m_BonusRatioHalf          .Reset();
			this->m_BonusRatioFull          .Reset();
			this->m_FixedBuybacks           .Reset();
			this->m_BuybacksPerWave         .Reset();
			this->m_DeathPenalty            .Reset();
			this->m_SentryBusterFriendlyFire.Reset();
			this->m_BotPushaway             .Reset();
			this->m_HumansMustJoinTeam      .Reset();
			
			this->m_AllowJoinTeamBlue   .Reset();
			this->m_AllowJoinTeamBlueMax.Reset();
			this->m_BluHumanFlagPickup  .Reset();
			this->m_BluHumanFlagCapture .Reset();
			this->m_SetCreditTeam       .Reset();
			this->m_EnableDominations   .Reset();
			
			this->m_CustomUpgradesFile.Reset();
			
			this->m_DisableSounds  .clear();
			this->m_ItemWhitelist  .clear();
			this->m_ItemBlacklist  .clear();
		//	this->m_DisallowedItems.clear();
			
			this->m_FlagResetTimes.clear();
			
			for (CTFTeamSpawn *spawn : this->m_ExtraSpawnPoints) {
				if (spawn != nullptr) {
					spawn->Remove();
				}
			}
			this->m_ExtraSpawnPoints.clear();
			
			this->m_ExtraTankPaths.clear();
		}
		
		bool m_bGiantsDropRareSpells;
		float m_flSpellDropRateCommon;
		float m_flSpellDropRateGiant;
		bool m_bNoReanimators;
		bool m_bNoMvMDeathTune;
		bool m_bSniperHideLasers;
		bool m_bSniperAllowHeadshots;
		bool m_bDisableUpgradeStations;
		float m_flRemoveGrapplingHooks;
		bool m_bReverseWinConditions;
		
		CPopOverride_MedievalMode        m_MedievalMode;
		CPopOverride_ConVar<bool>        m_SpellsEnabled;
		CPopOverride_ConVar<bool>        m_GrapplingHook;
		CPopOverride_ConVar<bool>        m_RespecEnabled;
		CPopOverride_ConVar<int>         m_RespecLimit;
		CPopOverride_ConVar<float>       m_BonusRatioHalf;
		CPopOverride_ConVar<float>       m_BonusRatioFull;
		CPopOverride_ConVar<bool>        m_FixedBuybacks;
		CPopOverride_ConVar<int>         m_BuybacksPerWave;
		CPopOverride_ConVar<int>         m_DeathPenalty;
		CPopOverride_ConVar<bool>        m_SentryBusterFriendlyFire;
		CPopOverride_ConVar<bool>        m_BotPushaway;
		CPopOverride_ConVar<std::string> m_HumansMustJoinTeam;
		
		CPopOverride_ConVar<bool> m_AllowJoinTeamBlue;
		CPopOverride_ConVar<int>  m_AllowJoinTeamBlueMax;
		CPopOverride_ConVar<bool> m_BluHumanFlagPickup;
		CPopOverride_ConVar<bool> m_BluHumanFlagCapture;
		CPopOverride_ConVar<int>  m_SetCreditTeam;
		CPopOverride_ConVar<bool> m_EnableDominations;
		
		CPopOverride_CustomUpgradesFile m_CustomUpgradesFile;
		
		std::set<std::string> m_DisableSounds;
		std::set<std::string> m_ItemWhitelist;
		std::set<std::string> m_ItemBlacklist;
	//	std::set<int>         m_DisallowedItems;
		
		std::map<std::string, int> m_FlagResetTimes;
		
		std::vector<CHandle<CTFTeamSpawn>> m_ExtraSpawnPoints;
		
		std::vector<ExtraTankPath> m_ExtraTankPaths;
	};
	PopState state;
	
	
	RefCount rc_CTFGameRules_PlayerKilled;
	CBasePlayer *killed = nullptr;
	DETOUR_DECL_MEMBER(void, CTFGameRules_PlayerKilled, CBasePlayer *pVictim, const CTakeDamageInfo& info)
	{
	//	DevMsg("CTFGameRules::PlayerKilled\n");
		
		killed = pVictim;
		SCOPED_INCREMENT(rc_CTFGameRules_PlayerKilled);
		DETOUR_MEMBER_CALL(CTFGameRules_PlayerKilled)(pVictim, info);
	}
	
	DETOUR_DECL_MEMBER(bool, CTFGameRules_ShouldDropSpellPickup)
	{
	//	DevMsg("CTFGameRules::ShouldDropSpellPickup\n");
		
		if (TFGameRules()->IsMannVsMachineMode() && rc_CTFGameRules_PlayerKilled > 0) {
			CTFBot *bot = ToTFBot(killed);
			if (bot == nullptr) return false;
			if (!state.m_SpellsEnabled.Get()) return false;
			
			float rnd  = RandomFloat(0.0f, 1.0f);
			float rate = (bot->IsMiniBoss() ? state.m_flSpellDropRateGiant : state.m_flSpellDropRateCommon);
			
			if (rnd > rate) {
	//			DevMsg("  %.3f > %.3f, returning false\n", rnd, rate);
				return false;
			}
			
	//		DevMsg("  %.3f <= %.3f, returning true\n", rnd, rate);
			return true;
		}
		
		return DETOUR_MEMBER_CALL(CTFGameRules_ShouldDropSpellPickup)();
	}
	
	DETOUR_DECL_MEMBER(void, CTFGameRules_DropSpellPickup, const Vector& where, int tier)
	{
	//	DevMsg("CTFGameRules::DropSpellPickup\n");
		
		if (TFGameRules()->IsMannVsMachineMode() && rc_CTFGameRules_PlayerKilled > 0) {
			CTFBot *bot = ToTFBot(killed);
			if (bot != nullptr) {
	//			DevMsg("  is a bot\n");
				if (state.m_bGiantsDropRareSpells && bot->IsMiniBoss()) {
	//				DevMsg("  dropping rare spell for miniboss\n");
					tier = 1;
				}
			}
		}
		
		DETOUR_MEMBER_CALL(CTFGameRules_DropSpellPickup)(where, tier);
	}
	
	DETOUR_DECL_MEMBER(bool, CTFGameRules_IsUsingSpells)
	{
	//	DevMsg("CTFGameRules::IsUsingSpells\n");
		
		if (TFGameRules()->IsMannVsMachineMode() && rc_CTFGameRules_PlayerKilled > 0) {
	//		DevMsg("  returning true\n");
			return true;
		}
		
		return DETOUR_MEMBER_CALL(CTFGameRules_IsUsingSpells)();
	}
	
	DETOUR_DECL_STATIC(CTFReviveMarker *, CTFReviveMarker_Create, CTFPlayer *player)
	{
		if (state.m_bNoReanimators || TFGameRules()->IsInMedievalMode()) {
			return nullptr;
		}
		
		return DETOUR_STATIC_CALL(CTFReviveMarker_Create)(player);
	}
	
	DETOUR_DECL_MEMBER(void, CTFSniperRifle_CreateSniperDot)
	{
		auto rifle = reinterpret_cast<CTFSniperRifle *>(this);
		
		if (state.m_bSniperHideLasers && TFGameRules()->IsMannVsMachineMode()) {
			CTFPlayer *owner = rifle->GetTFPlayerOwner();
			if (owner != nullptr && owner->GetTeamNumber() == TF_TEAM_BLUE) {
				return;
			}
		}
		
		DETOUR_MEMBER_CALL(CTFSniperRifle_CreateSniperDot)();
	}
	
	RefCount rc_CTFSniperRifle_CanFireCriticalShot;
	DETOUR_DECL_MEMBER(bool, CTFSniperRifle_CanFireCriticalShot, bool bIsHeadshot)
	{
		SCOPED_INCREMENT(rc_CTFSniperRifle_CanFireCriticalShot);
		return DETOUR_MEMBER_CALL(CTFSniperRifle_CanFireCriticalShot)(bIsHeadshot);
	}
	
	DETOUR_DECL_MEMBER(bool, CTFWeaponBase_CanFireCriticalShot, bool bIsHeadshot)
	{
		auto weapon = reinterpret_cast<CTFWeaponBase *>(this);
		
		if (state.m_bSniperAllowHeadshots && rc_CTFSniperRifle_CanFireCriticalShot > 0 && TFGameRules()->IsMannVsMachineMode()) {
			CTFPlayer *owner = weapon->GetTFPlayerOwner();
			if (owner != nullptr && owner->GetTeamNumber() == TF_TEAM_BLUE) {
				return true;
			}
		}
		
		return DETOUR_MEMBER_CALL(CTFWeaponBase_CanFireCriticalShot)(bIsHeadshot);
	}
	
	RefCount rc_CTFProjectile_Arrow_StrikeTarget;
	DETOUR_DECL_MEMBER(bool, CTFProjectile_Arrow_StrikeTarget, mstudiobbox_t *bbox, CBaseEntity *ent)
	{
		SCOPED_INCREMENT(rc_CTFProjectile_Arrow_StrikeTarget);
		return DETOUR_MEMBER_CALL(CTFProjectile_Arrow_StrikeTarget)(bbox, ent);
	}
	
	DETOUR_DECL_MEMBER(bool, CTFGameRules_IsPVEModeControlled, CBaseEntity *ent)
	{
		if (state.m_bSniperAllowHeadshots && rc_CTFProjectile_Arrow_StrikeTarget > 0 && TFGameRules()->IsMannVsMachineMode()) {
			return false;
		}
		
		return DETOUR_MEMBER_CALL(CTFGameRules_IsPVEModeControlled)(ent);
	}
	
	DETOUR_DECL_MEMBER(void, CUpgrades_UpgradeTouch, CBaseEntity *pOther)
	{
		if (state.m_bDisableUpgradeStations && TFGameRules()->IsMannVsMachineMode()) {
			CTFPlayer *player = ToTFPlayer(pOther);
			if (player != nullptr) {
				gamehelpers->TextMsg(ENTINDEX(player), TEXTMSG_DEST_CENTER, "The Upgrade Station is disabled for this mission!");
				return;
			}
		}
		
		DETOUR_MEMBER_CALL(CUpgrades_UpgradeTouch)(pOther);
	}
	
	DETOUR_DECL_MEMBER(void, CTeamplayRoundBasedRules_BroadcastSound, int iTeam, const char *sound, int iAdditionalSoundFlags)
	{
		if (TFGameRules()->IsMannVsMachineMode()) {
		//	DevMsg("CTeamplayRoundBasedRules::BroadcastSound(%d, \"%s\", 0x%08x)\n", iTeam, sound, iAdditionalSoundFlags);
			
			if (sound != nullptr && state.m_DisableSounds.count(std::string(sound)) != 0) {
				DevMsg("Blocked sound \"%s\" via CTeamplayRoundBasedRules::BroadcastSound\n", sound);
				return;
			}
		}
		
		DETOUR_MEMBER_CALL(CTeamplayRoundBasedRules_BroadcastSound)(iTeam, sound, iAdditionalSoundFlags);
	}
	
	DETOUR_DECL_STATIC(void, CBaseEntity_EmitSound_static_emitsound, IRecipientFilter& filter, int iEntIndex, const EmitSound_t& params)
	{
		if (TFGameRules()->IsMannVsMachineMode()) {
			const char *sound = params.m_pSoundName;
			
		//	DevMsg("CBaseEntity::EmitSound(#%d, \"%s\")\n", iEntIndex, sound);
			
			if (state.m_bNoMvMDeathTune && sound != nullptr && strcmp(sound, "MVM.PlayerDied") == 0) {
				return;
			}
			
			if (sound != nullptr && state.m_DisableSounds.count(std::string(sound)) != 0) {
				DevMsg("Blocked sound \"%s\" via CBaseEntity::EmitSound\n", sound);
				return;
			}
		}
		
		DETOUR_STATIC_CALL(CBaseEntity_EmitSound_static_emitsound)(filter, iEntIndex, params);
	}
	
	DETOUR_DECL_STATIC(void, CBaseEntity_EmitSound_static_emitsound_handle, IRecipientFilter& filter, int iEntIndex, const EmitSound_t& params, HSOUNDSCRIPTHANDLE& handle)
	{
		if (TFGameRules()->IsMannVsMachineMode()) {
			const char *sound = params.m_pSoundName;
			
		//	DevMsg("CBaseEntity::EmitSound(#%d, \"%s\", 0x%04x)\n", iEntIndex, sound, (uint16_t)handle);
			
			if (sound != nullptr && state.m_DisableSounds.count(std::string(sound)) != 0) {
				DevMsg("Blocked sound \"%s\" via CBaseEntity::EmitSound\n", sound);
				return;
			}
		}
		
		DETOUR_STATIC_CALL(CBaseEntity_EmitSound_static_emitsound_handle)(filter, iEntIndex, params, handle);
	}
	
//	RefCount rc_CTFPlayer_GiveDefaultItems;
//	DETOUR_DECL_MEMBER(void, CTFPlayer_GiveDefaultItems)
//	{
//		SCOPED_INCREMENT(rc_CTFPlayer_GiveDefaultItems);
//		DETOUR_MEMBER_CALL(CTFPlayer_GiveDefaultItems)();
//	}
	
	DETOUR_DECL_MEMBER(CBaseEntity *, CTFPlayer_GiveNamedItem, const char *classname, int i1, const CEconItemView *item_view, bool b1)
	{
		auto player = reinterpret_cast<CTFPlayer *>(this);
		
		/* this only applies to red team, for what essentially amount to "legacy" reasons */
		if (TFGameRules()->IsMannVsMachineMode() && player->GetTeamNumber() == TF_TEAM_RED) {
			/* only enforce the whitelist/blacklist if they are non-empty */
			
			if (!state.m_ItemWhitelist.empty() && state.m_ItemWhitelist.count(std::string(classname)) == 0) {
				DevMsg("[%s] GiveNamedItem(\"%s\"): denied by whitelist\n", player->GetPlayerName(), classname);
				return nullptr;
			}
			
			if (!state.m_ItemBlacklist.empty() && state.m_ItemBlacklist.count(std::string(classname)) != 0) {
				DevMsg("[%s] GiveNamedItem(\"%s\"): denied by blacklist\n", player->GetPlayerName(), classname);
				return nullptr;
			}
		}
		
	//	DevMsg("[%s] GiveNamedItem(\"%s\"): provisionally allowed\n", player->GetPlayerName(), classname);
		return DETOUR_MEMBER_CALL(CTFPlayer_GiveNamedItem)(classname, i1, item_view, b1);
	}
	
#if 0
	DETOUR_DECL_MEMBER(bool, CTFPlayer_ItemIsAllowed, CEconItemView *item_view)
	{
		auto player = reinterpret_cast<CTFPlayer *>(this);
		
		if (TFGameRules()->IsMannVsMachineMode() && item_view != nullptr) {
			int item_defidx = item_view->GetItemDefIndex();
			
			if (state.m_DisallowedItems.count(item_defidx) != 0) {
				DevMsg("[%s] CTFPlayer::ItemIsAllowed(%d): denied\n", player->GetPlayerName(), item_defidx);
				return false;
			}
		}
		
		return DETOUR_MEMBER_CALL(CTFPlayer_ItemIsAllowed)(item_view);
	}
#endif
	
	DETOUR_DECL_MEMBER(int, CCaptureFlag_GetMaxReturnTime)
	{
		auto flag = reinterpret_cast<CCaptureFlag *>(this);
		
		if (TFGameRules()->IsMannVsMachineMode()) {
			auto it = state.m_FlagResetTimes.find(STRING(flag->GetEntityName()));
			if (it != state.m_FlagResetTimes.end()) {
				return (*it).second;
			}
		}
		
		return DETOUR_MEMBER_CALL(CCaptureFlag_GetMaxReturnTime)();
	}
	
	
#if 0
	DETOUR_DECL_MEMBER(void, CTFGameRules_SetWinningTeam, int team, int iWinReason, bool bForceMapReset, bool bSwitchTeams, bool bDontAddScore, bool bFinal)
	{
		if (TFGameRules()->IsMannVsMachineMode() && state.m_bReverseWinConditions && team == TF_TEAM_BLUE) {
			// don't forward the call to SetWinningTeam
			// instead, imitate what is done when a wave is completed
			
			ForEachTFPlayer([](CTFPlayer *player){
				if (!player->IsAlive())                     return;
				if (player->GetTeamNumber() != TF_TEAM_RED) return;
				
				player->CommitSuicide(true, false);
			});
			
			// ...
			
			return;
		}
		
		DETOUR_MEMBER_CALL(CTFGameRules_SetWinningTeam)(team, iWinReason, bForceMapReset, bSwitchTeams, bDontAddScore, bFinal);
	}
#endif
	
	DETOUR_DECL_MEMBER(void, CTeamplayRoundBasedRules_State_Enter, gamerules_roundstate_t newState)
	{
		gamerules_roundstate_t oldState = TFGameRules()->State_Get();
		
	//	ConColorMsg(Color(0x00, 0xff, 0x00, 0xff), "[State] MvM:%d Reverse:%d oldState:%d newState:%d\n",
	//		TFGameRules()->IsMannVsMachineMode(), state.m_bReverseWinConditions, oldState, newState);
		
		if (TFGameRules()->IsMannVsMachineMode() && state.m_bReverseWinConditions && oldState == GR_STATE_TEAM_WIN && newState == GR_STATE_PREROUND) {
			ConColorMsg(Color(0x00, 0xff, 0x00, 0xff), "GR_STATE_TEAM_WIN --> GR_STATE_PREROUND\n");
			
			CWave *wave = g_pPopulationManager->GetCurrentWave();
			
			// TODO(?): find all TFBots not on TEAM_SPECTATOR and switch them to TEAM_SPECTATOR
			
			/* call this twice to ensure all wavespawns get to state DONE */
			ConColorMsg(Color(0xff, 0xff, 0x00, 0xff), "PRE  CWave::ForceFinish\n");
			wave->ForceFinish();
			ConColorMsg(Color(0xff, 0xff, 0x00, 0xff), "MID  CWave::ForceFinish\n");
			wave->ForceFinish();
			ConColorMsg(Color(0xff, 0xff, 0x00, 0xff), "POST CWave::ForceFinish\n");
			
			// is this necessary or not? unsure...
			// TODO: if enabled, need to block CTFPlayer::CommitSuicide call from ActiveWaveUpdate
		//	ConColorMsg(Color(0xff, 0xff, 0x00, 0xff), "PRE  CWave::ActiveWaveUpdate\n");
		//	wave->ActiveWaveUpdate();
		//	ConColorMsg(Color(0xff, 0xff, 0x00, 0xff), "POST CWave::ActiveWaveUpdate\n");
			
			ConColorMsg(Color(0xff, 0xff, 0x00, 0xff), "PRE  CWave::WaveCompleteUpdate\n");
			wave->WaveCompleteUpdate();
			ConColorMsg(Color(0xff, 0xff, 0x00, 0xff), "POST CWave::WaveCompleteUpdate\n");
			
			// TODO(?): for all human players on TF_TEAM_BLUE: if !IsAlive(), then call ForceRespawn()
			
			return;
		}
		
		DETOUR_MEMBER_CALL(CTeamplayRoundBasedRules_State_Enter)(newState);
	}
	
//	RefCount rc_CTFGameRules_FireGameEvent_teamplay_round_start;
//	DETOUR_DECL_MEMBER(void, CTFGameRules_FireGameEvent, IGameEvent *event)
//	{
//		SCOPED_INCREMENT_IF(rc_CTFGameRules_FireGameEvent_teamplay_round_start, FStrEq(event->GetName(), "teamplay_round_start"));
//		
//		DETOUR_MEMBER_CALL(CTFGameRules_FireGameEvent)(event);
//	}
//	
//	DETOUR_DECL_MEMBER(void, CTFPlayer_ChangeTeam, int iTeamNum, bool b1, bool b2, bool b3)
//	{
//		auto player = reinterpret_cast<CTFPlayer *>(this);
//		
//		if (rc_CTFGameRules_FireGameEvent_teamplay_round_start > 0 && TFGameRules()->IsMannVsMachineMode() && state.m_bReverseWinConditions && iTeamNum == TEAM_SPECTATOR) {
//			ConColorMsg(Color(0x00, 0xff, 0xff, 0xff), "BLOCKING ChangeTeam(TEAM_SPECTATOR) for player #%d \"%s\" on team %d\n",
//				ENTINDEX(player), player->GetPlayerName(), player->GetTeamNumber());
//			return;
//		}
//		
//		DETOUR_MEMBER_CALL(CTFPlayer_ChangeTeam)(iTeamNum, b1, b2, b3);
//	}
	
	
//	DETOUR_DECL_MEMBER(void, CPopulationManager_PostInitialize)
//	{
//		DETOUR_MEMBER_CALL(CPopulationManager_PostInitialize)();
//	}
	
	
	void Parse_ItemWhitelist(KeyValues *kv)
	{
		FOR_EACH_SUBKEY(kv, subkey) {
			DevMsg("ItemWhitelist: add \"%s\"\n", subkey->GetString());
			state.m_ItemWhitelist.emplace(subkey->GetString());
		}
	}
	
	void Parse_ItemBlacklist(KeyValues *kv)
	{
		FOR_EACH_SUBKEY(kv, subkey) {
			DevMsg("ItemBlacklist: add \"%s\"\n", subkey->GetString());
			state.m_ItemBlacklist.emplace(subkey->GetString());
		}
	}
	
	void Parse_FlagResetTime(KeyValues *kv)
	{
		const char *name = nullptr; // required
		int reset_time;             // required
		
		bool got_time = false;
		
		FOR_EACH_SUBKEY(kv, subkey) {
			if (FStrEq(subkey->GetName(), "Name")) {
				name = subkey->GetString();
			} else if (FStrEq(subkey->GetName(), "ResetTime")) {
				reset_time = subkey->GetInt();
				got_time = true;
			} else {
				Warning("Unknown key \'%s\' in FlagResetTime block.\n", subkey->GetName());
			}
		}
		
		bool fail = false;
		if (name == nullptr) {
			Warning("Missing Name key in FlagResetTime block.\n");
			fail = true;
		}
		if (!got_time) {
			Warning("Missing ResetTime key in FlagResetTime block.\n");
			fail = true;
		}
		if (fail) return;
		
		DevMsg("FlagResetTime: \"%s\" => %d\n", name, reset_time);
		state.m_FlagResetTimes.emplace(name, reset_time);
	}
	
	void Parse_ExtraSpawnPoint(KeyValues *kv)
	{
		const char *name = nullptr; // required
		int teamnum = TEAM_INVALID; // required
		Vector origin;              // required
		
		bool got_x = false;
		bool got_y = false;
		bool got_z = false;
		
		FOR_EACH_SUBKEY(kv, subkey) {
			if (FStrEq(subkey->GetName(), "Name")) {
				name = subkey->GetString();
			} else if (FStrEq(subkey->GetName(), "TeamNum")) {
				teamnum = Clamp(subkey->GetInt(), (int)TF_TEAM_RED, (int)TF_TEAM_BLUE);
			} else if (FStrEq(subkey->GetName(), "X")) {
				origin.x = subkey->GetFloat(); got_x = true;
			} else if (FStrEq(subkey->GetName(), "Y")) {
				origin.y = subkey->GetFloat(); got_y = true;
			} else if (FStrEq(subkey->GetName(), "Z")) {
				origin.z = subkey->GetFloat(); got_z = true;
			} else {
				Warning("Unknown key \'%s\' in ExtraSpawnPoint block.\n", subkey->GetName());
			}
		}
		
		bool fail = false;
		if (name == nullptr) {
			Warning("Missing Name key in ExtraSpawnPoint block.\n");
			fail = true;
		}
		if (teamnum == TEAM_INVALID) {
			Warning("Missing TeamNum key in ExtraSpawnPoint block.\n");
			fail = true;
		}
		if (!got_x) {
			Warning("Missing X key in ExtraSpawnPoint block.\n");
			fail = true;
		}
		if (!got_y) {
			Warning("Missing Y key in ExtraSpawnPoint block.\n");
			fail = true;
		}
		if (!got_z) {
			Warning("Missing Z key in ExtraSpawnPoint block.\n");
			fail = true;
		}
		if (fail) return;
		
		// Is it actually OK that we're spawning this stuff during parsing...?
		
		auto spawnpoint = rtti_cast<CTFTeamSpawn *>(CreateEntityByName("info_player_teamspawn"));
		if (spawnpoint == nullptr) {
			Warning("Parse_ExtraSpawnPoint: CreateEntityByName(\"info_player_teamspawn\") failed\n");
			return;
		}
		
		spawnpoint->SetName(AllocPooledString(name));
		
		spawnpoint->SetAbsOrigin(origin);
		spawnpoint->SetAbsAngles(vec3_angle);
		
		spawnpoint->ChangeTeam(teamnum);
		
		spawnpoint->Spawn();
		spawnpoint->Activate();
		
		state.m_ExtraSpawnPoints.emplace_back(spawnpoint);
		
		DevMsg("Parse_ExtraSpawnPoint: #%d, %08x, name \"%s\", teamnum %d, origin [ % 7.1f % 7.1f % 7.1f ], angles [ % 7.1f % 7.1f % 7.1f ]\n",
			ENTINDEX(spawnpoint), (uintptr_t)spawnpoint, STRING(spawnpoint->GetEntityName()), spawnpoint->GetTeamNumber(),
			VectorExpand(spawnpoint->GetAbsOrigin()), VectorExpand(spawnpoint->GetAbsAngles()));
	}
	
	void Parse_ExtraTankPath(KeyValues *kv)
	{
		const char *name = nullptr; // required
		std::vector<Vector> points; // required
		
		FOR_EACH_SUBKEY(kv, subkey) {
			if (FStrEq(subkey->GetName(), "Name")) {
				name = subkey->GetString();
			} else if (FStrEq(subkey->GetName(), "Node")) {
				Vector point; char c;
				if (sscanf(subkey->GetString(), " %f %f %f %c", &point.x, &point.y, &point.z, &c) == 3) {
					points.push_back(point);
				} else {
					Warning("Invalid value \'%s\' for Node key in ExtraTankPath block.\n", subkey->GetString());
				}
			} else {
				Warning("Unknown key \'%s\' in ExtraTankPath block.\n", subkey->GetName());
			}
		}
		
		bool fail = false;
		if (name == nullptr) {
			Warning("Missing Name key in ExtraTankPath block.\n");
			fail = true;
		}
		if (points.empty()) {
			Warning("No Node keys specified in ExtraTankPath block.\n");
			fail = true;
		}
		if (fail) return;
		
		
		state.m_ExtraTankPaths.emplace_back(name);
		auto& path = state.m_ExtraTankPaths.back();
		
		for (const auto& point : points) {
			if (!path.AddNode(point)) {
				Warning("Parse_ExtraTankPath: ExtraTankPath::AddNode reports that entity creation failed!\n");
				state.m_ExtraTankPaths.pop_back();
				return;
			}
		}
		
		// Is it actually OK that we're spawning this stuff during parsing...?
		path.SpawnNodes();
		
		DevMsg("Parse_ExtraTankPath: name \"%s\", %zu nodes\n", name, points.size());
	}
	
	RefCount rc_CPopulationManager_Parse;
	DETOUR_DECL_MEMBER(bool, CPopulationManager_Parse)
	{
	//	DevMsg("CPopulationManager::Parse\n");
		
		state.Reset();
		
		SCOPED_INCREMENT(rc_CPopulationManager_Parse);
		return DETOUR_MEMBER_CALL(CPopulationManager_Parse)();
	}
	
	RefCount rc_KeyValues_LoadFromFile;
	DETOUR_DECL_MEMBER(bool, KeyValues_LoadFromFile, IBaseFileSystem *filesystem, const char *resourceName, const char *pathID, bool refreshCache)
	{
	//	DevMsg("KeyValues::LoadFromFile\n");
		
		++rc_KeyValues_LoadFromFile;
		auto result = DETOUR_MEMBER_CALL(KeyValues_LoadFromFile)(filesystem, resourceName, pathID, refreshCache);
		--rc_KeyValues_LoadFromFile;
		
		if (result && rc_CPopulationManager_Parse > 0 && rc_KeyValues_LoadFromFile == 0) {
			auto kv = reinterpret_cast<KeyValues *>(this);
			
			std::vector<KeyValues *> del_kv;
			FOR_EACH_SUBKEY(kv, subkey) {
				const char *name = subkey->GetName();
				
				bool del = true;
				if (FStrEq(name, "BotsDropSpells")) {
					state.m_SpellsEnabled.Set(subkey->GetBool());
				} else if (FStrEq(name, "GiantsDropRareSpells")) {
					state.m_bGiantsDropRareSpells = subkey->GetBool();
				} else if (FStrEq(name, "SpellDropRateCommon")) {
					state.m_flSpellDropRateCommon = Clamp(subkey->GetFloat(), 0.0f, 1.0f);
				} else if (FStrEq(name, "SpellDropRateGiant")) {
					state.m_flSpellDropRateGiant = Clamp(subkey->GetFloat(), 0.0f, 1.0f);
				} else if (FStrEq(name, "NoReanimators")) {
					state.m_bNoReanimators = subkey->GetBool();
				} else if (FStrEq(name, "NoMvMDeathTune")) {
					state.m_bNoMvMDeathTune = subkey->GetBool();
				} else if (FStrEq(name, "SniperHideLasers")) {
					state.m_bSniperHideLasers = subkey->GetBool();
				} else if (FStrEq(name, "SniperAllowHeadshots")) {
					state.m_bSniperAllowHeadshots = subkey->GetBool();
				} else if (FStrEq(name, "DisableUpgradeStations")) {
					state.m_bDisableUpgradeStations = subkey->GetBool();
				} else if (FStrEq(name, "RemoveGrapplingHooks")) {
					state.m_flRemoveGrapplingHooks = subkey->GetFloat();
				} else if (FStrEq(name, "ReverseWinConditions")) {
					state.m_bReverseWinConditions = subkey->GetBool();
				} else if (FStrEq(name, "MedievalMode")) {
					state.m_MedievalMode.Set(subkey->GetBool());
				} else if (FStrEq(name, "GrapplingHook")) {
					state.m_GrapplingHook.Set(subkey->GetBool());
				} else if (FStrEq(name, "RespecEnabled")) {
					state.m_RespecEnabled.Set(subkey->GetBool());
				} else if (FStrEq(name, "RespecLimit")) {
					state.m_RespecLimit.Set(subkey->GetInt());
				} else if (FStrEq(name, "BonusRatioHalf")) {
					state.m_BonusRatioHalf.Set(subkey->GetFloat());
				} else if (FStrEq(name, "BonusRatioFull")) {
					state.m_BonusRatioFull.Set(subkey->GetFloat());
				} else if (FStrEq(name, "FixedBuybacks")) {
					state.m_FixedBuybacks.Set(subkey->GetBool());
				} else if (FStrEq(name, "BuybacksPerWave")) {
					state.m_BuybacksPerWave.Set(subkey->GetInt());
				} else if (FStrEq(name, "DeathPenalty")) {
					state.m_DeathPenalty.Set(subkey->GetInt());
				} else if (FStrEq(name, "SentryBusterFriendlyFire")) {
					state.m_SentryBusterFriendlyFire.Set(subkey->GetBool());
				} else if (FStrEq(name, "BotPushaway")) {
					state.m_BotPushaway.Set(subkey->GetBool());
				} else if (FStrEq(name, "HumansMustJoinTeam")) {
					state.m_HumansMustJoinTeam.Set(subkey->GetString());
				} else if (FStrEq(name, "AllowJoinTeamBlue")) {
					state.m_AllowJoinTeamBlue.Set(subkey->GetBool());
				} else if (FStrEq(name, "AllowJoinTeamBlueMax")) {
					state.m_AllowJoinTeamBlueMax.Set(subkey->GetInt());
				} else if (FStrEq(name, "BluHumanFlagPickup")) {
					state.m_BluHumanFlagPickup.Set(subkey->GetBool());
				} else if (FStrEq(name, "BluHumanFlagCapture")) {
					state.m_BluHumanFlagCapture.Set(subkey->GetBool());
				} else if (FStrEq(name, "SetCreditTeam")) {
					state.m_SetCreditTeam.Set(subkey->GetInt());
				} else if (FStrEq(name, "EnableDominations")) {
					state.m_EnableDominations.Set(subkey->GetBool());
				} else if (FStrEq(name, "CustomUpgradesFile")) {
					static const std::string prefix("download/scripts/items/");
					state.m_CustomUpgradesFile.Set(prefix + subkey->GetString());
				} else if (FStrEq(name, "DisableSound")) {
					state.m_DisableSounds.emplace(subkey->GetString());
				} else if (FStrEq(name, "ItemWhitelist")) {
					Parse_ItemWhitelist(subkey);
				} else if (FStrEq(name, "ItemBlacklist")) {
					Parse_ItemBlacklist(subkey);
			//	} else if (FStrEq(name, "DisallowedItems")) {
			//		Parse_DisallowedItems(subkey);
				} else if (FStrEq(name, "FlagResetTime")) {
					Parse_FlagResetTime(subkey);
				} else if (FStrEq(name, "ExtraSpawnPoint")) {
					Parse_ExtraSpawnPoint(subkey);
				} else if (FStrEq(name, "ExtraTankPath")) {
					Parse_ExtraTankPath(subkey);
				} else if (FStrEq(name, "PrecacheScriptSound"))  { CBaseEntity::PrecacheScriptSound (subkey->GetString());
				} else if (FStrEq(name, "PrecacheSound"))        { enginesound->PrecacheSound       (subkey->GetString(), false);
				} else if (FStrEq(name, "PrecacheModel"))        { engine     ->PrecacheModel       (subkey->GetString(), false);
				} else if (FStrEq(name, "PrecacheSentenceFile")) { engine     ->PrecacheSentenceFile(subkey->GetString(), false);
				} else if (FStrEq(name, "PrecacheDecal"))        { engine     ->PrecacheDecal       (subkey->GetString(), false);
				} else if (FStrEq(name, "PrecacheGeneric"))      { engine     ->PrecacheGeneric     (subkey->GetString(), false);
				} else if (FStrEq(name, "PreloadSound"))         { enginesound->PrecacheSound       (subkey->GetString(), true);
				} else if (FStrEq(name, "PreloadModel"))         { engine     ->PrecacheModel       (subkey->GetString(), true);
				} else if (FStrEq(name, "PreloadSentenceFile"))  { engine     ->PrecacheSentenceFile(subkey->GetString(), true);
				} else if (FStrEq(name, "PreloadDecal"))         { engine     ->PrecacheDecal       (subkey->GetString(), true);
				} else if (FStrEq(name, "PreloadGeneric"))       { engine     ->PrecacheGeneric     (subkey->GetString(), true);
				} else {
					del = false;
				}
				
				if (del) {
				//	DevMsg("Key \"%s\": processed, will delete\n", name);
					del_kv.push_back(subkey);
				} else {
				//	DevMsg("Key \"%s\": passthru\n", name);
				}
			}
			
			for (auto subkey : del_kv) {
			//	DevMsg("Deleting key \"%s\"\n", subkey->GetName());
				kv->RemoveSubKey(subkey);
				subkey->deleteThis();
			}
		}
		
		return result;
	}
	
	
	class CMod : public IMod, public IFrameUpdateListener
	{
	public:
		CMod() : IMod("Pop:PopMgr_Extensions")
		{
			MOD_ADD_DETOUR_MEMBER(CTFGameRules_PlayerKilled,                     "CTFGameRules::PlayerKilled");
			MOD_ADD_DETOUR_MEMBER(CTFGameRules_ShouldDropSpellPickup,            "CTFGameRules::ShouldDropSpellPickup");
			MOD_ADD_DETOUR_MEMBER(CTFGameRules_DropSpellPickup,                  "CTFGameRules::DropSpellPickup");
			MOD_ADD_DETOUR_MEMBER(CTFGameRules_IsUsingSpells,                    "CTFGameRules::IsUsingSpells");
			MOD_ADD_DETOUR_STATIC(CTFReviveMarker_Create,                        "CTFReviveMarker::Create");
			MOD_ADD_DETOUR_MEMBER(CTFSniperRifle_CreateSniperDot,                "CTFSniperRifle::CreateSniperDot");
			MOD_ADD_DETOUR_MEMBER(CTFSniperRifle_CanFireCriticalShot,            "CTFSniperRifle::CanFireCriticalShot");
			MOD_ADD_DETOUR_MEMBER(CTFWeaponBase_CanFireCriticalShot,             "CTFWeaponBase::CanFireCriticalShot");
			MOD_ADD_DETOUR_MEMBER(CTFProjectile_Arrow_StrikeTarget,              "CTFProjectile_Arrow::StrikeTarget");
			MOD_ADD_DETOUR_MEMBER(CTFGameRules_IsPVEModeControlled,              "CTFGameRules::IsPVEModeControlled");
			MOD_ADD_DETOUR_MEMBER(CUpgrades_UpgradeTouch,                        "CUpgrades::UpgradeTouch");
			MOD_ADD_DETOUR_MEMBER(CTeamplayRoundBasedRules_BroadcastSound,       "CTeamplayRoundBasedRules::BroadcastSound");
			MOD_ADD_DETOUR_STATIC(CBaseEntity_EmitSound_static_emitsound,        "CBaseEntity::EmitSound [static: emitsound]");
			MOD_ADD_DETOUR_STATIC(CBaseEntity_EmitSound_static_emitsound_handle, "CBaseEntity::EmitSound [static: emitsound + handle]");
		//	MOD_ADD_DETOUR_MEMBER(CTFPlayer_GiveDefaultItems,                    "CTFPlayer::GiveDefaultItems");
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_GiveNamedItem,                       "CTFPlayer::GiveNamedItem");
		//	MOD_ADD_DETOUR_MEMBER(CTFPlayer_ItemIsAllowed,                       "CTFPlayer::ItemIsAllowed");
			MOD_ADD_DETOUR_MEMBER(CCaptureFlag_GetMaxReturnTime,                 "CCaptureFlag::GetMaxReturnTime");
			
		//	MOD_ADD_DETOUR_MEMBER(CTFGameRules_SetWinningTeam,          "CTFGameRules::SetWinningTeam");
			MOD_ADD_DETOUR_MEMBER(CTeamplayRoundBasedRules_State_Enter, "CTeamplayRoundBasedRules::State_Enter");
		//	MOD_ADD_DETOUR_MEMBER(CTFGameRules_FireGameEvent,           "CTFGameRules::FireGameEvent");
		//	MOD_ADD_DETOUR_MEMBER(CTFPlayer_ChangeTeam,                 "CTFPlayer::ChangeTeam");
			
		//	MOD_ADD_DETOUR_MEMBER(CPopulationManager_PostInitialize, "CPopulationManager::PostInitialize");
			
			MOD_ADD_DETOUR_MEMBER(CPopulationManager_Parse, "CPopulationManager::Parse");
			MOD_ADD_DETOUR_MEMBER(KeyValues_LoadFromFile,   "KeyValues::LoadFromFile");
		}
		
		virtual void OnUnload() override
		{
			state.Reset();
		}
		
		virtual void OnDisable() override
		{
			state.Reset();
		}
		
		virtual bool ShouldReceiveFrameEvents() const override { return this->IsEnabled(); }
		
		virtual void FrameUpdatePostEntityThink() override
		{
			if (state.m_flRemoveGrapplingHooks >= 0.0f) {
				ForEachEntityByRTTI<CTFProjectile_GrapplingHook>([](CTFProjectile_GrapplingHook *proj){
					float dt = gpGlobals->curtime - proj->m_flTimeInit;
					if (dt > state.m_flRemoveGrapplingHooks) {
						DevMsg("Removing tf_projectile_grapplinghook #%d (alive for %.3f seconds)\n", ENTINDEX(proj), dt);
						proj->Remove();
					}
				});
			}
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_pop_popmgr_extensions", "0", FCVAR_NOTIFY,
		"Mod: enable extended KV in CPopulationManager::Parse",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
	
	
	class CKVCond_PopMgr : public IKVCond
	{
	public:
		virtual bool operator()() override
		{
			return s_Mod.IsEnabled();
		}
	};
	CKVCond_PopMgr cond;
	
	
//	// REMOVE ME
//	#warning REMOVE ME
//	CON_COMMAND(sig_test_stopsound, "")
//	{
//		if (args.ArgC() < 3) {
//			Warning("Expected 2 parameters: sound, flags.\n");
//			return;
//		}
//		
//		int team = -1;
//		const char *snd = args[1];
//		int flags = std::stol(args[2], nullptr, 0);
//		
//		IGameEvent *event = gameeventmanager->CreateEvent("teamplay_broadcast_audio");
//		if (event != nullptr) {
//			event->SetInt   ("team",             team);
//			event->SetString("sound",            snd);
//			event->SetInt   ("additional_flags", flags);
//			gameeventmanager->FireEvent(event);
//			
//			Msg("Fired event: teamplay_broadcast_audio\n"
//				"- team:             %d\n"
//				"- sound:            \"%s\"\n"
//				"- additional_flags: 0x%08x\n",
//				team, snd, flags);
//		}
//		
//		// nope: kills all sounds
//	}
}
