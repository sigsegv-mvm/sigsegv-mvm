#include "mod.h"
#include "stub/entities.h"
#include "util/scope.h"
#include "stub/tfbot.h"
#include "stub/gamerules.h"
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
		void Set(T val)
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
		virtual void SetValue(T val) = 0;
		
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
	template<> bool  ConVar_GetValue<bool> (const ConVarRef& cvar) { return cvar.GetBool(); }
	template<> int   ConVar_GetValue<int>  (const ConVarRef& cvar) { return cvar.GetInt(); }
	template<> float ConVar_GetValue<float>(const ConVarRef& cvar) { return cvar.GetFloat(); }
	
	template<typename T>
	class CPopOverride_ConVar : public IPopOverride<T>
	{
	public:
		CPopOverride_ConVar(const char *name) :
			m_pszConVarName(name) {}
		
		virtual T GetValue() override         { return ConVar_GetValue<T>(MyConVar()); }
		virtual void SetValue(T val) override { MyConVar().SetValue(val); }
		
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
		virtual bool GetValue() override         { return TFGameRules()->m_bPlayingMedieval; }
		virtual void SetValue(bool val) override { TFGameRules()->m_bPlayingMedieval = val; }
	};
	
	
	struct PopState
	{
		PopState() :
			m_SpellsEnabled  ("tf_spells_enabled"),
			m_GrapplingHook  ("tf_grapplinghook_enable"),
			m_RespecEnabled  ("tf_mvm_respec_enabled"),
			m_RespecLimit    ("tf_mvm_respec_limit"),
			m_BonusRatioHalf ("tf_mvm_currency_bonus_ratio_min"),
			m_BonusRatioFull ("tf_mvm_currency_bonus_ratio_max"),
			m_FixedBuybacks  ("tf_mvm_buybacks_method"),
			m_BuybacksPerWave("tf_mvm_buybacks_per_wave"),
			m_DeathPenalty   ("tf_mvm_death_penalty")
		{
			this->Reset();
		}
		
		void Reset()
		{
			this->m_bGiantsDropRareSpells = false;
			this->m_flSpellDropRateCommon = 1.00f;
			this->m_flSpellDropRateGiant  = 1.00f;
			this->m_bNoReanimators        = false;
			this->m_bNoMvMDeathTune       = false;
			
			this->m_MedievalMode.Reset();
			this->m_SpellsEnabled.Reset();
			this->m_GrapplingHook.Reset();
			this->m_RespecEnabled.Reset();
			this->m_RespecLimit.Reset();
			this->m_BonusRatioHalf.Reset();
			this->m_BonusRatioFull.Reset();
			this->m_FixedBuybacks.Reset();
			this->m_BuybacksPerWave.Reset();
			this->m_DeathPenalty.Reset();
		}
		
		bool m_bGiantsDropRareSpells;
		float m_flSpellDropRateCommon;
		float m_flSpellDropRateGiant;
		bool m_bNoReanimators;
		bool m_bNoMvMDeathTune;
		
		CPopOverride_MedievalMode  m_MedievalMode;
		CPopOverride_ConVar<bool>  m_SpellsEnabled;
		CPopOverride_ConVar<bool>  m_GrapplingHook;
		CPopOverride_ConVar<bool>  m_RespecEnabled;
		CPopOverride_ConVar<int>   m_RespecLimit;
		CPopOverride_ConVar<float> m_BonusRatioHalf;
		CPopOverride_ConVar<float> m_BonusRatioFull;
		CPopOverride_ConVar<bool>  m_FixedBuybacks;
		CPopOverride_ConVar<int>   m_BuybacksPerWave;
		CPopOverride_ConVar<int>   m_DeathPenalty;
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
		if (state.m_bNoReanimators || TFGameRules()->m_bPlayingMedieval) {
			return nullptr;
		}
		
		return DETOUR_STATIC_CALL(CTFReviveMarker_Create)(player);
	}
	
	DETOUR_DECL_MEMBER(void, CBaseEntity_EmitSound, const char *soundname, float soundtime, float *duration)
	{
		if (state.m_bNoMvMDeathTune && soundname != nullptr && strcmp(soundname, "MVM.PlayerDied") == 0) {
			return;
		}
		
		DETOUR_MEMBER_CALL(CBaseEntity_EmitSound)(soundname, soundtime, duration);
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
				if (V_stricmp(name, "BotsDropSpells") == 0) {
					state.m_SpellsEnabled.Set(subkey->GetBool());
				} else if (V_stricmp(name, "GiantsDropRareSpells") == 0) {
					state.m_bGiantsDropRareSpells = subkey->GetBool();
				} else if (V_stricmp(name, "SpellDropRateCommon") == 0) {
					state.m_flSpellDropRateCommon = Clamp(subkey->GetFloat(), 0.0f, 1.0f);
				} else if (V_stricmp(name, "SpellDropRateGiant") == 0) {
					state.m_flSpellDropRateGiant = Clamp(subkey->GetFloat(), 0.0f, 1.0f);
				} else if (V_stricmp(name, "NoReanimators") == 0) {
					state.m_bNoReanimators = subkey->GetBool();
				} else if (V_stricmp(name, "NoMvMDeathTune") == 0) {
					state.m_bNoMvMDeathTune = subkey->GetBool();
				} else if (V_stricmp(name, "MedievalMode") == 0) {
					state.m_MedievalMode.Set(subkey->GetBool());
				} else if (V_stricmp(name, "GrapplingHook") == 0) {
					state.m_GrapplingHook.Set(subkey->GetBool());
				} else if (V_stricmp(name, "RespecEnabled") == 0) {
					state.m_RespecEnabled.Set(subkey->GetBool());
				} else if (V_stricmp(name, "RespecLimit") == 0) {
					state.m_RespecLimit.Set(subkey->GetInt());
				} else if (V_stricmp(name, "BonusRatioHalf") == 0) {
					state.m_BonusRatioHalf.Set(subkey->GetFloat());
				} else if (V_stricmp(name, "BonusRatioFull") == 0) {
					state.m_BonusRatioFull.Set(subkey->GetFloat());
				} else if (V_stricmp(name, "FixedBuybacks") == 0) {
					state.m_FixedBuybacks.Set(subkey->GetBool());
				} else if (V_stricmp(name, "BuybacksPerWave") == 0) {
					state.m_BuybacksPerWave.Set(subkey->GetInt());
				} else if (V_stricmp(name, "DeathPenalty") == 0) {
					state.m_DeathPenalty.Set(subkey->GetInt());
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
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Pop:PopMgr_Extensions")
		{
			MOD_ADD_DETOUR_MEMBER(CTFGameRules_PlayerKilled,          "CTFGameRules::PlayerKilled");
			MOD_ADD_DETOUR_MEMBER(CTFGameRules_ShouldDropSpellPickup, "CTFGameRules::ShouldDropSpellPickup");
			MOD_ADD_DETOUR_MEMBER(CTFGameRules_DropSpellPickup,       "CTFGameRules::DropSpellPickup");
			MOD_ADD_DETOUR_MEMBER(CTFGameRules_IsUsingSpells,         "CTFGameRules::IsUsingSpells");
			MOD_ADD_DETOUR_STATIC(CTFReviveMarker_Create,             "CTFReviveMarker::Create");
			MOD_ADD_DETOUR_MEMBER(CBaseEntity_EmitSound,              "CBaseEntity::EmitSound [const char *, float, float *]");
			
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
}
