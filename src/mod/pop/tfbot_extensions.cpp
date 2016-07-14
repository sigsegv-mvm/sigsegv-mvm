#include "mod.h"
#include "stub/tfplayer.h"
#include "stub/populators.h"
#include "util/scope.h"
#include "mod/pop/kv_conditional.h"
#include "re/nextbot.h"
#include "re/path.h"
#include "stub/tfbot_behavior.h"


namespace Mod_Pop_TFBot_Extensions
{
	/* mobber AI, based on CTFBotAttackFlagDefenders */
	class CTFBotMobber : public IHotplugAction
	{
	public:
		CTFBotMobber()
		{
			this->m_Attack = CTFBotAttack::New();
		}
		virtual ~CTFBotMobber()
		{
			if (this->m_Attack != nullptr) {
				delete this->m_Attack;
			}
		}
		
		virtual const char *GetName() const override { return "Mobber"; }
		
		virtual ActionResult<CTFBot> OnStart(CTFBot *actor, Action<CTFBot> *action) override
		{
			this->m_PathFollower.SetMinLookAheadDistance(actor->GetDesiredPathLookAheadRange());
			
			this->m_hTarget = nullptr;
			
			return this->m_Attack->OnStart(actor, action);
		}
		
		virtual ActionResult<CTFBot> Update(CTFBot *actor, float dt) override
		{
			const CKnownEntity *threat = actor->GetVisionInterface()->GetPrimaryKnownThreat(false);
			if (threat != nullptr) {
				actor->EquipBestWeaponForThreat(threat);
			}
			
			ActionResult<CTFBot> result = this->m_Attack->Update(actor, dt);
			if (result.transition != ActionTransition::DONE) {
				return ActionResult<CTFBot>::Continue();
			}
			
			if (this->m_hTarget == nullptr || !this->m_hTarget->IsAlive()) {
				this->m_hTarget = actor->SelectRandomReachableEnemy();
				
				if (this->m_hTarget == nullptr) {
					return ActionResult<CTFBot>::Continue();
				}
			}
			
			actor->GetVisionInterface()->AddKnownEntity(this->m_hTarget);
			
			auto nextbot = rtti_cast<INextBot *>(actor);
			
			if (this->m_ctRecomputePath.IsElapsed()) {
				this->m_ctRecomputePath.Start(RandomFloat(1.0f, 3.0f));
				
				CTFBotPathCost cost_func(actor, DEFAULT_ROUTE);
				this->m_PathFollower.Compute(nextbot, this->m_hTarget, cost_func, 0.0f, true);
			}
			
			this->m_PathFollower.Update(nextbot);
			
			return ActionResult<CTFBot>::Continue();
		}
		
	private:
		CTFBotAttack *m_Attack = nullptr;
		
		CHandle<CTFPlayer> m_hTarget;
		
		PathFollower m_PathFollower;
		CountdownTimer m_ctRecomputePath;
	};
	
	
	struct AddCond
	{
		ETFCond cond   = (ETFCond)-1;
		float duration = -1.0f;
	};
	
	struct SpawnerData
	{
		std::vector<AddCond> addconds;
		
		bool action_mobber = false;
		
		bool use_human_model = false;
	};
	
	
	std::map<CTFBotSpawner *, SpawnerData> spawners;
	
	
	/* this is really dodgy... should probably do this with the ECAttr
	 * extension system, provided we ever finish that contraption */
	std::vector<CHandle<CTFBot>> action_override_mobber;
	
	
	DETOUR_DECL_MEMBER(void, CTFBotSpawner_dtor0)
	{
		auto spawner = reinterpret_cast<CTFBotSpawner *>(this);
		
//		DevMsg("CTFBotSpawner %08x: dtor0\n", (uintptr_t)spawner);
		spawners.erase(spawner);
		
		DETOUR_MEMBER_CALL(CTFBotSpawner_dtor0)();
	}
	
	DETOUR_DECL_MEMBER(void, CTFBotSpawner_dtor2)
	{
		auto spawner = reinterpret_cast<CTFBotSpawner *>(this);
		
//		DevMsg("CTFBotSpawner %08x: dtor2\n", (uintptr_t)spawner);
		spawners.erase(spawner);
		
		DETOUR_MEMBER_CALL(CTFBotSpawner_dtor2)();
	}
	
	
	void Parse_AddCond(CTFBotSpawner *spawner, KeyValues *kv)
	{
		AddCond addcond;
		
		bool got_cond     = false;
		bool got_duration = false;
		
		FOR_EACH_SUBKEY(kv, subkey) {
			const char *name = subkey->GetName();
			
			if (V_stricmp(name, "Index") == 0) {
				addcond.cond = (ETFCond)subkey->GetInt();
				got_cond = true;
			} else if (V_stricmp(name, "Name") == 0) {
				ETFCond cond = GetTFConditionFromName(subkey->GetString());
				if (cond != -1) {
					addcond.cond = cond;
					got_cond = true;
				} else {
					Warning("Unrecognized condition name \"%s\" in AddCond block.\n", subkey->GetString());
				}
			} else if (V_stricmp(name, "Duration") == 0) {
				addcond.duration = subkey->GetFloat();
				got_duration = true;
			} else {
				Warning("Unknown key \'%s\' in AddCond block.\n", name);
			}
		}
		
		if (!got_cond) {
			Warning("Could not find a valid condition index/name in AddCond block.\n");
			return;
		}
		
//		DevMsg("CTFBotSpawner %08x: add AddCond(%d, %f)\n", (uintptr_t)spawner, addcond.cond, addcond.duration);
		spawners[spawner].addconds.push_back(addcond);
	}
	
	void Parse_Action(CTFBotSpawner *spawner, KeyValues *kv)
	{
		const char *value = kv->GetString();
		
		if (V_stricmp(value, "Mobber") == 0) {
			spawners[spawner].action_mobber = true;
		} else {
			Warning("Unknown value \'%s\' for TFBot Action.\n", value);
		}
	}
	
	DETOUR_DECL_MEMBER(bool, CTFBotSpawner_Parse, KeyValues *kv)
	{
		auto spawner = reinterpret_cast<CTFBotSpawner *>(this);
		
	//	DevMsg("CTFBotSpawner::Parse\n");
		
		std::vector<KeyValues *> del_kv;
		FOR_EACH_SUBKEY(kv, subkey) {
			const char *name = subkey->GetName();
			
			bool del = true;
			if (V_stricmp(name, "AddCond") == 0) {
				Parse_AddCond(spawner, subkey);
			} else if (V_stricmp(name, "Action") == 0) {
				Parse_Action(spawner, subkey);
			} else if (V_stricmp(name, "UseHumanModel") == 0) {
				spawners[spawner].use_human_model = subkey->GetBool();
			} else {
				del = false;
			}
			
			if (del) {
	//			DevMsg("Key \"%s\": processed, will delete\n", name);
				del_kv.push_back(subkey);
			} else {
	//			DevMsg("Key \"%s\": passthru\n", name);
			}
		}
		
		for (auto subkey : del_kv) {
	//		DevMsg("Deleting key \"%s\"\n", subkey->GetName());
			kv->RemoveSubKey(subkey);
			subkey->deleteThis();
		}
		
		return DETOUR_MEMBER_CALL(CTFBotSpawner_Parse)(kv);
	}
	
	
	RefCount rc_CTFBotSpawner_Spawn;
	CTFBotSpawner *current_spawner = nullptr;
	DETOUR_DECL_MEMBER(int, CTFBotSpawner_Spawn, const Vector& where, CUtlVector<CHandle<CBaseEntity>> *ents)
	{
		auto spawner = reinterpret_cast<CTFBotSpawner *>(this);
		
		SCOPED_INCREMENT(rc_CTFBotSpawner_Spawn);
		current_spawner = spawner;
		
		auto result = DETOUR_MEMBER_CALL(CTFBotSpawner_Spawn)(where, ents);
		
		if (ents != nullptr) {
			auto it = spawners.find(spawner);
			if (it != spawners.end()) {
				SpawnerData& data = (*it).second;
				
				FOR_EACH_VEC((*ents), i) {
					CTFBot *bot = ToTFBot((*ents)[i]);
					if (bot != nullptr) {
		//				DevMsg("CTFBotSpawner %08x: found %u AddCond's\n", (uintptr_t)spawner, data.addconds.size());
						for (auto addcond : data.addconds) {
		//					DevMsg("CTFBotSpawner %08x: applying AddCond(%d, %f)\n", (uintptr_t)spawner, addcond.cond, addcond.duration);
							bot->m_Shared->AddCond(addcond.cond, addcond.duration);
						}
						
						if (data.action_mobber) {
							action_override_mobber.push_back(bot);
						}
						
						if (data.use_human_model) {
							// calling SetCustomModel with a nullptr string *seems* to reset the model
							// dunno what the bool parameter should be; I think it doesn't matter for the nullptr case
							bot->GetPlayerClass()->SetCustomModel(nullptr, true);
							bot->UpdateModel();
							bot->SetBloodColor(BLOOD_COLOR_RED);
						}
					}
				}
			}
		}
		
		return result;
	}
	
	
	DETOUR_DECL_MEMBER(Action<CTFBot> *, CTFBotScenarioMonitor_DesiredScenarioAndClassAction, CTFBot *actor)
	{
		bool override_mobber = false;
		
		for (auto it = action_override_mobber.begin(); it != action_override_mobber.end(); ) {
			if (*it == actor) {
				it = action_override_mobber.erase(it);
				override_mobber = true;
			} else {
				++it;
			}
		}
		
		if (override_mobber) {
			return new CTFBotMobber();
		}
		
		return DETOUR_MEMBER_CALL(CTFBotScenarioMonitor_DesiredScenarioAndClassAction)(actor);
	}
	
	
//	// TEST! REMOVE ME!
//	DETOUR_DECL_MEMBER(const char *, CTFPlayer_GetOverrideStepSound, const char *pszBaseStepSoundName)
//	{
//		DevMsg("CTFPlayer::OverrideStepSound(\"%s\")\n", pszBaseStepSoundName);
//		return pszBaseStepSoundName;
//	}
//	
//	// TEST! REMOVE ME!
//	DETOUR_DECL_MEMBER(const char *, CTFPlayer_GetSceneSoundToken)
//	{
//		DevMsg("CTFPlayer::GetSceneSoundToken\n");
//		return "";
//	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Pop:TFBot_Extensions")
		{
			MOD_ADD_DETOUR_MEMBER(CTFBotSpawner_dtor0, "CTFBotSpawner::~CTFBotSpawner [D0]");
			MOD_ADD_DETOUR_MEMBER(CTFBotSpawner_dtor2, "CTFBotSpawner::~CTFBotSpawner [D2]");
			
			MOD_ADD_DETOUR_MEMBER(CTFBotSpawner_Parse, "CTFBotSpawner::Parse");
			
			MOD_ADD_DETOUR_MEMBER(CTFBotSpawner_Spawn, "CTFBotSpawner::Spawn");
			
			MOD_ADD_DETOUR_MEMBER(CTFBotScenarioMonitor_DesiredScenarioAndClassAction, "CTFBotScenarioMonitor::DesiredScenarioAndClassAction");
			
			// TEST! REMOVE ME!
//			MOD_ADD_DETOUR_MEMBER(CTFPlayer_GetOverrideStepSound, "CTFPlayer::GetOverrideStepSound");
//			MOD_ADD_DETOUR_MEMBER(CTFPlayer_GetSceneSoundToken,   "CTFPlayer::GetSceneSoundToken");
		}
		
		virtual void OnUnload() override
		{
			spawners.clear();
		}
		
		virtual void OnDisable() override
		{
			spawners.clear();
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_pop_tfbot_extensions", "0", FCVAR_NOTIFY,
		"Mod: enable extended KV in CTFBotSpawner::Parse",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
	
	
	class CKVCond_TFBot : public IKVCond
	{
	public:
		virtual bool operator()() override
		{
			return s_Mod.IsEnabled();
		}
	};
	CKVCond_TFBot cond;
}

/*
Current UseHumanModel mod:
- Fix voices
- Fix step sound

- Fix bullet impact sounds?
- Fix impact particles

- Fix idle sound
- Fix death sound
- Sentry buster model/blood
*/

// TODO: look for random one-off cases of MVM_ or M_MVM_ strings
// (e.g. engineer voice lines and stuff)

// voices:
// server detour of CTFPlayer::GetSceneSoundToken
