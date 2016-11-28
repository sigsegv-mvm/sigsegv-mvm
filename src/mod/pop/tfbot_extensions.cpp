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
			
			/* added teamnum check to fix some TF_COND_REPROGRAMMED quirks */
			if (this->m_hTarget == nullptr || !this->m_hTarget->IsAlive() || this->m_hTarget->GetTeamNumber() == actor->GetTeamNumber()) {
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
		float delay    =  0.0f;
	};
	
	enum ActionType
	{
		Action_Default,
		
		// built-in
		Action_FetchFlag,
		Action_PushToCapturePoint,
		
		// custom
		Action_Mobber,
	};
	
	struct SpawnerData
	{
		std::vector<AddCond> addconds;
		
		ActionType action = Action_Default;
		
		bool use_human_model = false;
	};
	
	
	std::map<CTFBotSpawner *, SpawnerData> spawners;
	
	
//	/* this is really dodgy... should probably do this with the ECAttr
//	 * extension system, provided we ever finish that contraption */
//	std::map<CHandle<CTFBot>, ActionType> pending_action_overrides;
	
	
	struct DelayedAddCond
	{
		CHandle<CTFBot> bot;
		float when;
		ETFCond cond;
		float duration;
	};
	std::vector<DelayedAddCond> delayed_addconds;
	
	
	void UpdateDelayedAddConds()
	{
		for (auto it = delayed_addconds.begin(); it != delayed_addconds.end(); ) {
			const auto& info = *it;
			
			if (info.bot == nullptr || !info.bot->IsAlive()) {
				it = delayed_addconds.erase(it);
				continue;
			}
			
			if (gpGlobals->curtime >= info.when) {
				info.bot->m_Shared->AddCond(info.cond, info.duration);
				
				it = delayed_addconds.erase(it);
				continue;
			}
			
			++it;
		}
	}
	
	
	/* for keeping track of which spawner each bot came from */
	CTFBotSpawner *bot_spawner_tracker[33];
	CTFBotSpawner *GetSpawnerOfBot(const CTFBot *bot)
	{
		int idx = ENTINDEX(bot);
		if (idx == 0) return nullptr;
		
		assert(idx > 0);
		assert(idx < 33);
		return bot_spawner_tracker[idx];
	}
	void SetSpawnerOfBot(const CTFBot *bot, CTFBotSpawner *spawner)
	{
		int idx = ENTINDEX(bot);
		if (idx == 0) return;
		
		assert(idx > 0);
		assert(idx < 33);
		bot_spawner_tracker[idx] = spawner;
	}
	void ClearTrackingForSpawner(CTFBotSpawner *spawner)
	{
		for (auto& elem : bot_spawner_tracker) {
			if (elem == spawner) {
				elem = nullptr;
			}
		}
	}
	void ClearAllTracking()
	{
		for (auto& elem : bot_spawner_tracker) {
			elem = nullptr;
		}
	}
	
	
	DETOUR_DECL_MEMBER(void, CTFBotSpawner_dtor0)
	{
		auto spawner = reinterpret_cast<CTFBotSpawner *>(this);
		
		DevMsg("CTFBotSpawner %08x: dtor0\n", (uintptr_t)spawner);
		spawners.erase(spawner);
		ClearTrackingForSpawner(spawner);
		
		DETOUR_MEMBER_CALL(CTFBotSpawner_dtor0)();
	}
	
	DETOUR_DECL_MEMBER(void, CTFBotSpawner_dtor2)
	{
		auto spawner = reinterpret_cast<CTFBotSpawner *>(this);
		
		DevMsg("CTFBotSpawner %08x: dtor2\n", (uintptr_t)spawner);
		spawners.erase(spawner);
		ClearTrackingForSpawner(spawner);
		
		DETOUR_MEMBER_CALL(CTFBotSpawner_dtor2)();
	}
	
	
	void Parse_AddCond(CTFBotSpawner *spawner, KeyValues *kv)
	{
		AddCond addcond;
		
		bool got_cond     = false;
		bool got_duration = false;
		bool got_delay    = false;
		
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
			} else if (V_stricmp(name, "Delay") == 0) {
				addcond.delay = subkey->GetFloat();
				got_delay = true;
			} else {
				Warning("Unknown key \'%s\' in AddCond block.\n", name);
			}
		}
		
		if (!got_cond) {
			Warning("Could not find a valid condition index/name in AddCond block.\n");
			return;
		}
		
		DevMsg("CTFBotSpawner %08x: add AddCond(%d, %f)\n", (uintptr_t)spawner, addcond.cond, addcond.duration);
		spawners[spawner].addconds.push_back(addcond);
	}
	
	void Parse_Action(CTFBotSpawner *spawner, KeyValues *kv)
	{
		const char *value = kv->GetString();
		
		if (V_stricmp(value, "Default") == 0) {
			spawners[spawner].action = Action_Default;
		} else if (V_stricmp(value, "FetchFlag") == 0) {
			spawners[spawner].action = Action_FetchFlag;
		} else if (V_stricmp(value, "PushToCapturePoint") == 0) {
			spawners[spawner].action = Action_PushToCapturePoint;
		} else if (V_stricmp(value, "Mobber") == 0) {
			spawners[spawner].action = Action_Mobber;
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
		
	//	DevMsg("\nCTFBotSpawner %08x: SPAWNED\n", (uintptr_t)spawner);
	//	DevMsg("  [classicon \"%s\"] [miniboss %d]\n", STRING(spawner->GetClassIcon(0)), spawner->IsMiniBoss(0));
	//	DevMsg("- result: %d\n", result);
	//	if (ents != nullptr) {
	//		DevMsg("- ents:  ");
	//		FOR_EACH_VEC((*ents), i) {
	//			DevMsg(" #%d", ENTINDEX((*ents)[i]));
	//		}
	//		DevMsg("\n");
	//	}
		
		if (result && ents != nullptr && !ents->IsEmpty()) {
			auto it = spawners.find(spawner);
			if (it != spawners.end()) {
				SpawnerData& data = (*it).second;
				
				CTFBot *bot = ToTFBot(ents->Tail());
				if (bot != nullptr) {
					SetSpawnerOfBot(bot, spawner);
					
				//	DevMsg("CTFBotSpawner %08x: found %u AddCond's\n", (uintptr_t)spawner, data.addconds.size());
					for (auto addcond : data.addconds) {
						if (addcond.delay == 0.0f) {
							DevMsg("CTFBotSpawner %08x: applying AddCond(%d, %f)\n", (uintptr_t)spawner, addcond.cond, addcond.duration);
							bot->m_Shared->AddCond(addcond.cond, addcond.duration);
						} else {
							delayed_addconds.push_back({
								bot,
								gpGlobals->curtime + addcond.delay,
								addcond.cond,
								addcond.duration,
							});
						}
					}
					
				//	if (data.action != Action_Default) {
				//		pending_action_overrides.emplace(bot, data.action);
				//	}
					
					if (data.use_human_model) {
						DevMsg("CTFBotSpawner %08x: applying UseHumanModel on bot #%d\n", (uintptr_t)spawner, ENTINDEX(bot));
						
						// calling SetCustomModel with a nullptr string *seems* to reset the model
						// dunno what the bool parameter should be; I think it doesn't matter for the nullptr case
						bot->GetPlayerClass()->SetCustomModel(nullptr, true);
						bot->UpdateModel();
						bot->SetBloodColor(BLOOD_COLOR_RED);
					}
				}
			}
		}
		
		return result;
	}
	
	
	DETOUR_DECL_MEMBER(Action<CTFBot> *, CTFBotScenarioMonitor_DesiredScenarioAndClassAction, CTFBot *actor)
	{
		ActionType action = Action_Default;
		
		CTFBotSpawner *spawner = GetSpawnerOfBot(actor);
		if (spawner != nullptr) {
			auto it = spawners.find(spawner);
			if (it != spawners.end()) {
				SpawnerData& data = (*it).second;
				action = data.action;
			}
		}
		
		switch (action) {
		case Action_FetchFlag:
			DevMsg("CTFBotSpawner: setting initial action of bot #%d to FetchFlag\n", ENTINDEX(actor));
			return CTFBotFetchFlag::New();
		case Action_PushToCapturePoint:
			DevMsg("CTFBotSpawner: setting initial action of bot #%d to PushToCapturePoint[-->FetchFlag]\n", ENTINDEX(actor));
			return CTFBotPushToCapturePoint::New(CTFBotFetchFlag::New());
		case Action_Mobber:
			DevMsg("CTFBotSpawner: setting initial action of bot #%d to Mobber\n", ENTINDEX(actor));
			return new CTFBotMobber();
		}
		
		return DETOUR_MEMBER_CALL(CTFBotScenarioMonitor_DesiredScenarioAndClassAction)(actor);
	}
	
	
	/* make engiebots that don't have any Action override always have Attributes
	 * IgnoreFlag, so that we can safely remove the special-case class check in
	 * CTFBot::GetFlagToFetch */
	DETOUR_DECL_MEMBER(void, CTFBot_OnEventChangeAttributes, const CTFBot::EventChangeAttributes_t *ecattr)
	{
		auto bot = reinterpret_cast<CTFBot *>(this);
		
		if (bot->IsPlayerClass(TF_CLASS_ENGINEER)) {
			CTFBotSpawner *spawner = GetSpawnerOfBot(bot);
			if (spawner != nullptr) {
				auto it = spawners.find(spawner);
				if (it != spawners.end()) {
					SpawnerData& data = (*it).second;
					
					if (data.action == Action_Default) {
						DevMsg("CTFBot::OnEventChangeAttributes(engie #%d): adding Attributes IgnoreFlag\n", ENTINDEX(bot));
					//	const_cast<CTFBot::EventChangeAttributes_t *>(ecattr)->m_nBotAttrs |= CTFBot::ATTR_IGNORE_FLAG;
						(int&)(ecattr->m_nBotAttrs) |= CTFBot::ATTR_IGNORE_FLAG;
					} else {
						DevMsg("CTFBot::OnEventChangeAttributes(engie #%d): not adding Attributes IgnoreFlag due to Action override\n", ENTINDEX(bot));
					}
				}
			}
		}
		
		DETOUR_MEMBER_CALL(CTFBot_OnEventChangeAttributes)(ecattr);
	}
	
	
	RefCount rc_CTFBot_GetFlagToFetch;
	DETOUR_DECL_MEMBER(CCaptureFlag *, CTFBot_GetFlagToFetch)
	{
		SCOPED_INCREMENT(rc_CTFBot_GetFlagToFetch);
		return DETOUR_MEMBER_CALL(CTFBot_GetFlagToFetch)();
	}
	
	DETOUR_DECL_MEMBER(bool, CTFPlayer_IsPlayerClass, int iClass)
	{
		if (rc_CTFBot_GetFlagToFetch > 0 && iClass == TF_CLASS_ENGINEER) {
			return false;
		}
		
		return DETOUR_MEMBER_CALL(CTFPlayer_IsPlayerClass)(iClass);
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
	
	
	class CMod : public IMod, public IFrameUpdateListener
	{
	public:
		CMod() : IMod("Pop:TFBot_Extensions")
		{
			MOD_ADD_DETOUR_MEMBER(CTFBotSpawner_dtor0, "CTFBotSpawner::~CTFBotSpawner [D0]");
			MOD_ADD_DETOUR_MEMBER(CTFBotSpawner_dtor2, "CTFBotSpawner::~CTFBotSpawner [D2]");
			
			MOD_ADD_DETOUR_MEMBER(CTFBotSpawner_Parse, "CTFBotSpawner::Parse");
			
			MOD_ADD_DETOUR_MEMBER(CTFBotSpawner_Spawn, "CTFBotSpawner::Spawn");
			
			MOD_ADD_DETOUR_MEMBER(CTFBotScenarioMonitor_DesiredScenarioAndClassAction, "CTFBotScenarioMonitor::DesiredScenarioAndClassAction");
			
			MOD_ADD_DETOUR_MEMBER(CTFBot_OnEventChangeAttributes, "CTFBot::OnEventChangeAttributes");
			
			MOD_ADD_DETOUR_MEMBER(CTFBot_GetFlagToFetch,   "CTFBot::GetFlagToFetch");
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_IsPlayerClass, "CTFPlayer::IsPlayerClass");
			
			// TEST! REMOVE ME!
//			MOD_ADD_DETOUR_MEMBER(CTFPlayer_GetOverrideStepSound, "CTFPlayer::GetOverrideStepSound");
//			MOD_ADD_DETOUR_MEMBER(CTFPlayer_GetSceneSoundToken,   "CTFPlayer::GetSceneSoundToken");
		}
		
		virtual void OnUnload() override
		{
			spawners.clear();
			ClearAllTracking();
		}
		
		virtual void OnDisable() override
		{
			spawners.clear();
			ClearAllTracking();
		}
		
		virtual bool ShouldReceiveFrameEvents() const override { return this->IsEnabled(); }
		
		virtual void FrameUpdatePostEntityThink() override
		{
			UpdateDelayedAddConds();
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
