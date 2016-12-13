#include "mod.h"
#include "stub/tfplayer.h"
#include "stub/populators.h"
#include "stub/projectiles.h"
#include "util/scope.h"
#include "mod/pop/kv_conditional.h"
#include "re/nextbot.h"
#include "re/path.h"
#include "stub/tfbot_behavior.h"
#include "util/iterate.h"

// TODO: move to common.h
#define INVALID_PARTICLE_ATTACHMENT -1


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
	
	struct HomingRockets
	{
		bool enable                 = false;
		bool ignore_disguised_spies = true;
		bool ignore_stealthed_spies = true;
		float speed                 = 1.0f;
		float turn_power            = 10.0f;
		float min_dot_product       = -0.25f;
	};
	
	struct SpawnerData
	{
		std::vector<AddCond> addconds;
		std::vector<AddCond> dmgappliesconds;
		
		bool action_mobber = false;
		
		std::map<int, float> weapon_resists;
		
		std::map<std::string, color32> item_colors;
		
		bool use_human_model  = false;
		bool use_buster_model = false;
		std::string use_custom_model;
		
		std::string rocket_custom_model;
		std::string rocket_custom_trail;
		
		float ring_of_fire = -1.0f;
		
		HomingRockets homing_rockets;
	};
	
	
	std::map<CTFBotSpawner *, SpawnerData> spawners;
	
	std::map<CHandle<CTFBot>, CTFBotSpawner *> spawner_of_bot;
	
	
	/* this is really dodgy... should probably do this with the ECAttr
	 * extension system, provided we ever finish that contraption */
	std::vector<CHandle<CTFBot>> action_override_mobber;
	
	
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
	
	
	void RemoveSpawner(CTFBotSpawner *spawner)
	{
		for (auto it = spawner_of_bot.begin(); it != spawner_of_bot.end(); ) {
			if ((*it).second == spawner) {
				it = spawner_of_bot.erase(it);
			} else {
				++it;
			}
		}
		
		spawners.erase(spawner);
	}
	
	
	DETOUR_DECL_MEMBER(void, CTFBotSpawner_dtor0)
	{
		auto spawner = reinterpret_cast<CTFBotSpawner *>(this);
		
		DevMsg("CTFBotSpawner %08x: dtor0\n", (uintptr_t)spawner);
		RemoveSpawner(spawner);
		
		DETOUR_MEMBER_CALL(CTFBotSpawner_dtor0)();
	}
	
	DETOUR_DECL_MEMBER(void, CTFBotSpawner_dtor2)
	{
		auto spawner = reinterpret_cast<CTFBotSpawner *>(this);
		
		DevMsg("CTFBotSpawner %08x: dtor2\n", (uintptr_t)spawner);
		RemoveSpawner(spawner);
		
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
			
			if (FStrEq(name, "Index")) {
				addcond.cond = (ETFCond)subkey->GetInt();
				got_cond = true;
			} else if (FStrEq(name, "Name")) {
				ETFCond cond = GetTFConditionFromName(subkey->GetString());
				if (cond != -1) {
					addcond.cond = cond;
					got_cond = true;
				} else {
					Warning("Unrecognized condition name \"%s\" in AddCond block.\n", subkey->GetString());
				}
			} else if (FStrEq(name, "Duration")) {
				addcond.duration = subkey->GetFloat();
				got_duration = true;
			} else if (FStrEq(name, "Delay")) {
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
	
	void Parse_DamageAppliesCond(CTFBotSpawner *spawner, KeyValues *kv)
	{
		AddCond addcond;
		
		bool got_cond     = false;
		bool got_duration = false;
		
		FOR_EACH_SUBKEY(kv, subkey) {
			const char *name = subkey->GetName();
			
			if (FStrEq(name, "Index")) {
				addcond.cond = (ETFCond)subkey->GetInt();
				got_cond = true;
			} else if (FStrEq(name, "Name")) {
				ETFCond cond = GetTFConditionFromName(subkey->GetString());
				if (cond != -1) {
					addcond.cond = cond;
					got_cond = true;
				} else {
					Warning("Unrecognized condition name \"%s\" in DamageAppliesCond block.\n", subkey->GetString());
				}
			} else if (FStrEq(name, "Duration")) {
				addcond.duration = subkey->GetFloat();
				got_duration = true;
			} else {
				Warning("Unknown key \'%s\' in DamageAppliesCond block.\n", name);
			}
		}
		
		if (!got_cond) {
			Warning("Could not find a valid condition index/name in DamageAppliesCond block.\n");
			return;
		}
		
		DevMsg("CTFBotSpawner %08x: add DamageAppliesCond(%d, %f)\n", (uintptr_t)spawner, addcond.cond, addcond.duration);
		spawners[spawner].dmgappliesconds.push_back(addcond);
	}
	
	void Parse_Action(CTFBotSpawner *spawner, KeyValues *kv)
	{
		const char *value = kv->GetString();
		
		if (FStrEq(value, "Mobber")) {
			spawners[spawner].action_mobber = true;
		} else {
			Warning("Unknown value \'%s\' for TFBot Action.\n", value);
		}
	}
	
	void Parse_WeaponResist(CTFBotSpawner *spawner, KeyValues *kv)
	{
		FOR_EACH_SUBKEY(kv, subkey) {
			const char *name = subkey->GetName();
			
			int weapon_id = GetWeaponId(name);
			if (weapon_id == TF_WEAPON_NONE) {
				Warning("Unknown weapon ID \'%s\' in WeaponResist block.\n", name);
				continue;
			}
			
			DevMsg("CTFBotSpawner %08x: resist %s (0x%02x): %4.02f\n", (uintptr_t)spawner, name, weapon_id, subkey->GetFloat());
			spawners[spawner].weapon_resists[weapon_id] = subkey->GetFloat();
		}
	}
	
	void Parse_ItemColor(CTFBotSpawner *spawner, KeyValues *kv)
	{
		const char *item_name = "";
		int color_r           = 0x00;
		int color_g           = 0x00;
		int color_b           = 0x00;
		
		bool got_name  = false;
		bool got_col_r = false;
		bool got_col_g = false;
		bool got_col_b = false;
		
		FOR_EACH_SUBKEY(kv, subkey) {
			const char *name = subkey->GetName();
			
			if (FStrEq(name, "ItemName")) {
				item_name = subkey->GetString();
				got_name = true;
			} else if (FStrEq(name, "Red")) {
				color_r = Clamp(subkey->GetInt(), 0x00, 0xff);
				got_col_r = true;
			} else if (FStrEq(name, "Green")) {
				color_g = Clamp(subkey->GetInt(), 0x00, 0xff);
				got_col_g = true;
			} else if (FStrEq(name, "Blue")) {
				color_b = Clamp(subkey->GetInt(), 0x00, 0xff);
				got_col_b = true;
			} else {
				Warning("Unknown key \'%s\' in ItemColor block.\n", name);
			}
		}
		
		if (!got_name) {
			Warning("No ItemName specified in ItemColor block.\n");
			return;
		}
		
		if (!got_col_r) {
			Warning("No Red color value specified in ItemColor block.\n");
			return;
		}
		if (!got_col_g) {
			Warning("No Green color value specified in ItemColor block.\n");
			return;
		}
		if (!got_col_b) {
			Warning("No Blue color value specified in ItemColor block.\n");
			return;
		}
		
		DevMsg("CTFBotSpawner %08x: add ItemColor(\"%s\", %02X%02X%02X)\n", (uintptr_t)spawner, item_name, color_r, color_g, color_b);
		spawners[spawner].item_colors[item_name] = { color_r, color_g, color_b, 0xff };
	}
	
	void Parse_HomingRockets(CTFBotSpawner *spawner, KeyValues *kv)
	{
		HomingRockets& hr = spawners[spawner].homing_rockets;
		
		FOR_EACH_SUBKEY(kv, subkey) {
			const char *name = subkey->GetName();
			
			if (FStrEq(name, "Enable")) {
				hr.enable = subkey->GetBool();
			} else if (FStrEq(name, "IgnoreDisguisedSpies")) {
				hr.ignore_disguised_spies = subkey->GetBool();
			} else if (FStrEq(name, "IgnoreStealthedSpies")) {
				hr.ignore_stealthed_spies = subkey->GetBool();
			} else if (FStrEq(name, "RocketSpeed")) {
				hr.speed = subkey->GetFloat();
			} else if (FStrEq(name, "TurnPower")) {
				hr.turn_power = subkey->GetFloat();
			} else if (FStrEq(name, "MinDotProduct")) {
				hr.min_dot_product = Clamp(subkey->GetFloat(), -1.0f, 1.0f);
			} else if (FStrEq(name, "MaxAimError")) {
				hr.min_dot_product = std::cos(DEG2RAD(Clamp(subkey->GetFloat(), 0.0f, 180.0f)));
			} else {
				Warning("Unknown key \'%s\' in HomingRockets block.\n", name);
			}
		}
		
		DevMsg("CTFBotSpawner %08x: set HomingRockets(%s, %s, %s, %.2f, %.1f, %.2f)\n",
			(uintptr_t)spawner,
			(hr.enable                 ? "true" : "false"),
			(hr.ignore_disguised_spies ? "true" : "false"),
			(hr.ignore_stealthed_spies ? "true" : "false"),
			hr.speed, hr.turn_power, hr.min_dot_product);
	}
	
	DETOUR_DECL_MEMBER(bool, CTFBotSpawner_Parse, KeyValues *kv)
	{
		auto spawner = reinterpret_cast<CTFBotSpawner *>(this);
		
	//	DevMsg("CTFBotSpawner::Parse\n");
		
		std::vector<KeyValues *> del_kv;
		FOR_EACH_SUBKEY(kv, subkey) {
			const char *name = subkey->GetName();
			
			bool del = true;
			if (FStrEq(name, "AddCond")) {
				Parse_AddCond(spawner, subkey);
			} else if (FStrEq(name, "DamageAppliesCond")) {
				Parse_DamageAppliesCond(spawner, subkey);
			} else if (FStrEq(name, "Action")) {
				Parse_Action(spawner, subkey);
			} else if (FStrEq(name, "WeaponResist")) {
				Parse_WeaponResist(spawner, subkey);
			} else if (FStrEq(name, "ItemColor")) {
				Parse_ItemColor(spawner, subkey);
			} else if (FStrEq(name, "HomingRockets")) {
				Parse_HomingRockets(spawner, subkey);
			} else if (FStrEq(name, "UseHumanModel")) {
				spawners[spawner].use_human_model = subkey->GetBool();
			} else if (FStrEq(name, "UseBusterModel")) {
				spawners[spawner].use_buster_model = subkey->GetBool();
			} else if (FStrEq(name, "UseCustomModel")) {
				spawners[spawner].use_custom_model = subkey->GetString();
			} else if (FStrEq(name, "RocketCustomModel")) {
				spawners[spawner].rocket_custom_model = subkey->GetString();
			} else if (FStrEq(name, "RocketCustomTrail")) {
				spawners[spawner].rocket_custom_trail = subkey->GetString();
			} else if (FStrEq(name, "RingOfFire")) {
				spawners[spawner].ring_of_fire = subkey->GetFloat();
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
					spawner_of_bot[bot] = spawner;
					
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
					
					if (data.action_mobber) {
						action_override_mobber.push_back(bot);
					}
					
					if (!data.use_custom_model.empty()) {
						DevMsg("CTFBotSpawner %08x: applying UseCustomModel(\"%s\") on bot #%d\n", (uintptr_t)spawner, data.use_custom_model.c_str(), ENTINDEX(bot));
						
						bot->GetPlayerClass()->SetCustomModel(data.use_custom_model.c_str(), true);
						bot->UpdateModel();
						bot->SetBloodColor(DONT_BLEED);
						
						// TODO: RomeVision...?
					} else if (data.use_buster_model) {
						DevMsg("CTFBotSpawner %08x: applying UseBusterModel on bot #%d\n", (uintptr_t)spawner, ENTINDEX(bot));
						
						// here we mimic what CMissionPopulator::UpdateMissionDestroySentries does
						bot->GetPlayerClass()->SetCustomModel("models/bots/demo/bot_sentry_buster.mdl", true);
						bot->UpdateModel();
						bot->SetBloodColor(DONT_BLEED);
						
						// TODO: filter-out addition of Romevision cosmetics to UseBusterModel bots
						// TODO: manually add Romevision cosmetic for SentryBuster to UseBusterModel bots
					} else if (data.use_human_model) {
						DevMsg("CTFBotSpawner %08x: applying UseHumanModel on bot #%d\n", (uintptr_t)spawner, ENTINDEX(bot));
						
						// calling SetCustomModel with a nullptr string *seems* to reset the model
						// dunno what the bool parameter should be; I think it doesn't matter for the nullptr case
						bot->GetPlayerClass()->SetCustomModel(nullptr, true);
						bot->UpdateModel();
						bot->SetBloodColor(BLOOD_COLOR_RED);
						
						// TODO: filter-out addition of Romevision cosmetics to UseHumanModel bots
					}
					
					// should really be in OnEventChangeAttributes, where ItemAttributes are applied
					for (const auto& pair : data.item_colors) {
						const char *item_name     = pair.first.c_str();
						const color32& item_color = pair.second;
						
						CEconItemDefinition *item_def = GetItemSchema()->GetItemDefinitionByName(item_name);
						if (item_def == nullptr) continue;
						
						for (int slot = GetNumberOfLoadoutSlots() - 1; slot >= 0; --slot) {
							CEconEntity *econ_entity = nullptr;
							
							CEconItemView *item_view = CTFPlayerSharedUtils::GetEconItemViewByLoadoutSlot(bot, slot, &econ_entity);
							if (item_view == nullptr || econ_entity == nullptr) continue;
							
							if (item_view->GetItemDefIndex() == item_def->m_iItemDefIndex) {
								DevMsg("CTFBotSpawner %08x: applying color %02X%02X%02X to item \"%s\"\n",
									(uintptr_t)spawner, item_color.r, item_color.g, item_color.b, item_name);
								
								econ_entity->SetRenderColorR(item_color.r);
								econ_entity->SetRenderColorG(item_color.g);
								econ_entity->SetRenderColorB(item_color.b);
							}
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
	
	
	DETOUR_DECL_MEMBER(int, CTFGameRules_ApplyOnDamageModifyRules, CTakeDamageInfo& info, CBaseEntity *pVictim, bool b1)
	{
		auto pTFBot = ToTFBot(pVictim);
		if (pTFBot != nullptr) {
			CTFBotSpawner *spawner = spawner_of_bot[pTFBot];
			if (spawner != nullptr) {
				auto pTFWeapon = rtti_cast<CTFWeaponBase *>(info.GetWeapon());
				if (pTFWeapon != nullptr) {
					int weapon_id = pTFWeapon->GetWeaponID();
					
					auto it = spawners[spawner].weapon_resists.find(pTFWeapon->GetWeaponID());
					if (it != spawners[spawner].weapon_resists.end()) {
						float resist = (*it).second;
						info.ScaleDamage(resist);
						DevMsg("Bot #%d taking damage from weapon_id 0x%02x; resist is %4.2f\n", ENTINDEX(pVictim), weapon_id, resist);
					}
				}
			}
		}
		
		return DETOUR_MEMBER_CALL(CTFGameRules_ApplyOnDamageModifyRules)(info, pVictim, b1);
	}
	
	
	DETOUR_DECL_MEMBER(CTFProjectile_Rocket *, CTFWeaponBaseGun_FireRocket, CTFPlayer *player, int i1)
	{
		auto proj = DETOUR_MEMBER_CALL(CTFWeaponBaseGun_FireRocket)(player, i1);
		
		if (proj != nullptr) {
			auto pTFBot = ToTFBot(proj->GetOwnerEntity());
			if (pTFBot != nullptr) {
				CTFBotSpawner *spawner = spawner_of_bot[pTFBot];
				
				auto it = spawners.find(spawner);
				if (it != spawners.end()) {
					SpawnerData& data = (*it).second;
					
					if (!data.rocket_custom_model.empty()) {
						proj->SetModel(data.rocket_custom_model.c_str());
					}
					
					if (!data.rocket_custom_trail.empty()) {
						if (data.rocket_custom_trail.front() == '~') {
							StopParticleEffects(proj);
							DispatchParticleEffect(data.rocket_custom_trail.c_str() + 1, PATTACH_ABSORIGIN_FOLLOW, proj, INVALID_PARTICLE_ATTACHMENT, false);
						} else {
							DispatchParticleEffect(data.rocket_custom_trail.c_str(), PATTACH_ABSORIGIN_FOLLOW, proj, INVALID_PARTICLE_ATTACHMENT, false);
						}
					}
				}
			}
		}
		
		return proj;
	}
	
	
	DETOUR_DECL_MEMBER(void, CTFWeaponBase_ApplyOnHitAttributes, CBaseEntity *ent, CTFPlayer *player, const CTakeDamageInfo& info)
	{
		DETOUR_MEMBER_CALL(CTFWeaponBase_ApplyOnHitAttributes)(ent, player, info);
		
		CTFPlayer *victim = ToTFPlayer(ent);
		CTFBot *attacker  = ToTFBot(player);
		if (victim != nullptr && attacker != nullptr) {
			CTFBotSpawner *spawner = spawner_of_bot[attacker];
			
			auto it = spawners.find(spawner);
			if (it != spawners.end()) {
				SpawnerData& data = (*it).second;
				
				for (const auto& addcond : data.dmgappliesconds) {
					victim->m_Shared->AddCond(addcond.cond, addcond.duration, attacker);
				}
			}
		}
	}
	
	
	DETOUR_DECL_MEMBER(void, CTFProjectile_Rocket_Spawn)
	{
		DETOUR_MEMBER_CALL(CTFProjectile_Rocket_Spawn)();
		
		auto rocket = reinterpret_cast<CTFProjectile_Rocket *>(this);
		
		CTFBot *bot = ToTFBot(rocket->GetOwnerPlayer());
		if (bot != nullptr) {
			CTFBotSpawner *spawner = spawner_of_bot[bot];
			
			auto it = spawners.find(spawner);
			if (it != spawners.end()) {
				SpawnerData& data = (*it).second;
				
				if (data.homing_rockets.enable) {
					rocket->SetMoveType(MOVETYPE_CUSTOM);
				}
			}
		}
	}
	
	DETOUR_DECL_MEMBER(void, CBaseEntity_PerformCustomPhysics, Vector *pNewPosition, Vector *pNewVelocity, QAngle *pNewAngles, QAngle *pNewAngVelocity)
	{
		CTFProjectile_Rocket *rocket = nullptr;
		const HomingRockets *hr      = nullptr;
		
		auto ent = reinterpret_cast<CBaseEntity *>(this);
		if (ent->ClassMatches("tf_projectile_rocket") && (rocket = rtti_cast<CTFProjectile_Rocket *>(ent)) != nullptr) {
			CTFBot *bot = ToTFBot(rocket->GetOwnerPlayer());
			if (bot != nullptr) {
				CTFBotSpawner *spawner = spawner_of_bot[bot];
				
				auto it = spawners.find(spawner);
				if (it != spawners.end()) {
					SpawnerData& data = (*it).second;
					
					hr = &data.homing_rockets;
				}
			}
		}
		
		if (hr == nullptr || !hr->enable) {
			DETOUR_MEMBER_CALL(CBaseEntity_PerformCustomPhysics)(pNewPosition, pNewVelocity, pNewAngles, pNewAngVelocity);
			return;
		}
		
		constexpr float physics_interval = 0.25f;
		if (gpGlobals->tickcount % (int)(physics_interval / gpGlobals->interval_per_tick) == 0) {
			CTFPlayer *target_player = nullptr;
			float target_distsqr     = FLT_MAX;
			
			ForEachTFPlayer([&](CTFPlayer *player){
				if (!player->IsAlive())                                 return;
				if (player->GetTeamNumber() == TEAM_SPECTATOR)          return;
				if (player->GetTeamNumber() == rocket->GetTeamNumber()) return;
				
				if (hr->ignore_disguised_spies) {
					if (player->m_Shared->InCond(TF_COND_DISGUISED) && player->m_Shared->GetDisguiseTeam() == rocket->GetTeamNumber()) {
						return;
					}
				}
				
				if (hr->ignore_stealthed_spies) {
					if (player->m_Shared->IsStealthed() && player->m_Shared->GetPercentInvisible() >= 0.75f &&
						!player->m_Shared->InCond(TF_COND_STEALTHED_BLINK) && !player->m_Shared->InCond(TF_COND_BURNING) && !player->m_Shared->InCond(TF_COND_URINE) && !player->m_Shared->InCond(TF_COND_BLEEDING)) {
						return;
					}
				}
				
				Vector delta = player->WorldSpaceCenter() - rocket->WorldSpaceCenter();
				if (DotProduct(delta.Normalized(), pNewVelocity->Normalized()) < hr->min_dot_product) return;
				
				float distsqr = rocket->WorldSpaceCenter().DistToSqr(player->WorldSpaceCenter());
				if (distsqr < target_distsqr) {
					trace_t tr;
					UTIL_TraceLine(player->WorldSpaceCenter(), rocket->WorldSpaceCenter(), MASK_SOLID_BRUSHONLY, player, COLLISION_GROUP_NONE, &tr);
					
					if (!tr.DidHit() || tr.m_pEnt == rocket) {
						target_player  = player;
						target_distsqr = distsqr;
					}
				}
			});
			
			if (target_player != nullptr) {
				QAngle angToTarget;
				VectorAngles(target_player->WorldSpaceCenter() - rocket->WorldSpaceCenter(), angToTarget);
				
				pNewAngVelocity->x = Clamp(Approach(AngleDiff(angToTarget.x, pNewAngles->x) * 4.0f, pNewAngVelocity->x, hr->turn_power), -360.0f, 360.0f);
				pNewAngVelocity->y = Clamp(Approach(AngleDiff(angToTarget.y, pNewAngles->y) * 4.0f, pNewAngVelocity->y, hr->turn_power), -360.0f, 360.0f);
				pNewAngVelocity->z = Clamp(Approach(AngleDiff(angToTarget.z, pNewAngles->z) * 4.0f, pNewAngVelocity->z, hr->turn_power), -360.0f, 360.0f);
			}
		}
		
		*pNewAngles += (*pNewAngVelocity * gpGlobals->frametime);
		
		Vector vecOrientation;
		AngleVectors(*pNewAngles, &vecOrientation);
		*pNewVelocity = vecOrientation * (1100.0f * hr->speed);
		
		*pNewPosition += (*pNewVelocity * gpGlobals->frametime);
		
	//	if (gpGlobals->tickcount % 2 == 0) {
	//		NDebugOverlay::EntityText(ENTINDEX(rocket), -2, CFmtStr("  AngVel: % 6.1f % 6.1f % 6.1f", VectorExpand(*pNewAngVelocity)), 0.030f);
	//		NDebugOverlay::EntityText(ENTINDEX(rocket), -1, CFmtStr("  Angles: % 6.1f % 6.1f % 6.1f", VectorExpand(*pNewAngles)),      0.030f);
	//		NDebugOverlay::EntityText(ENTINDEX(rocket),  1, CFmtStr("Velocity: % 6.1f % 6.1f % 6.1f", VectorExpand(*pNewVelocity)),    0.030f);
	//		NDebugOverlay::EntityText(ENTINDEX(rocket),  2, CFmtStr("Position: % 6.1f % 6.1f % 6.1f", VectorExpand(*pNewPosition)),    0.030f);
	//	}
		
	//	DevMsg("[%d] PerformCustomPhysics: #%d %s\n", gpGlobals->tickcount, ENTINDEX(ent), ent->GetClassname());
	}
	
	
//	// TEST! REMOVE ME!
//	// (for making human-model MvM bots use non-robot footstep sfx)
//	DETOUR_DECL_MEMBER(const char *, CTFPlayer_GetOverrideStepSound, const char *pszBaseStepSoundName)
//	{
//		DevMsg("CTFPlayer::OverrideStepSound(\"%s\")\n", pszBaseStepSoundName);
//		return pszBaseStepSoundName;
//	}
//	
//	// TEST! REMOVE ME!
//	// (for making human-model MvM bots use non-robot vo lines)
//	DETOUR_DECL_MEMBER(const char *, CTFPlayer_GetSceneSoundToken)
//	{
//		DevMsg("CTFPlayer::GetSceneSoundToken\n");
//		return "";
//	}
	
	
	void UpdateRingOfFire()
	{
		static int ring_of_fire_tick_interval = (int)(0.500f / (float)gpGlobals->interval_per_tick);
		if (gpGlobals->tickcount % ring_of_fire_tick_interval == 0) {
			ForEachTFBot([](CTFBot *bot){
				if (!bot->IsAlive()) return;
				CTFBotSpawner *spawner = spawner_of_bot[bot];
				
				auto it = spawners.find(spawner);
				if (it != spawners.end()) {
					SpawnerData& data = (*it).second;
					
					if (data.ring_of_fire >= 0.0f) {
						ForEachEntityInSphere(bot->GetAbsOrigin(), 135.0f, [&](CBaseEntity *ent){
							CTFPlayer *victim = ToTFPlayer(ent);
							if (victim == nullptr) return;
							
							if (victim->GetTeamNumber() == bot->GetTeamNumber()) return;
							if (victim->m_Shared->IsInvulnerable())              return;
							
							Vector victim_mins = victim->GetPlayerMins();
							Vector victim_maxs = victim->GetPlayerMaxs();
							
							if (bot->GetAbsOrigin().z >= victim->GetAbsOrigin().z + victim_maxs.z) return;
							
							Vector closest;
							victim->CollisionProp()->CalcNearestPoint(bot->GetAbsOrigin(), &closest);
							if (closest.DistToSqr(bot->GetAbsOrigin()) > Square(135.0f)) return;
							
							// trace start should be minigun WSC
							trace_t tr;
							UTIL_TraceLine(bot->WorldSpaceCenter(), victim->WorldSpaceCenter(), MASK_SOLID_BRUSHONLY, bot, COLLISION_GROUP_PROJECTILE, &tr);
							
							if (tr.fraction == 1.0f || tr.m_pEnt == victim) {
								Vector bot_origin = bot->GetAbsOrigin();
								victim->TakeDamage(CTakeDamageInfo(bot, bot, bot->GetActiveTFWeapon(), vec3_origin, bot_origin, data.ring_of_fire, DMG_IGNITE, 0, &bot_origin));
							}
						});
						
						DispatchParticleEffect("heavy_ring_of_fire", bot->GetAbsOrigin(), vec3_angle);
					}
				}
			});
		}
	}
	
	
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
			
			MOD_ADD_DETOUR_MEMBER(CTFGameRules_ApplyOnDamageModifyRules, "CTFGameRules::ApplyOnDamageModifyRules");
			
			MOD_ADD_DETOUR_MEMBER(CTFWeaponBaseGun_FireRocket, "CTFWeaponBaseGun::FireRocket");
			
			MOD_ADD_DETOUR_MEMBER(CTFWeaponBase_ApplyOnHitAttributes, "CTFWeaponBase::ApplyOnHitAttributes");
			
			MOD_ADD_DETOUR_MEMBER(CTFProjectile_Rocket_Spawn,       "CTFProjectile_Rocket::Spawn");
			MOD_ADD_DETOUR_MEMBER(CBaseEntity_PerformCustomPhysics, "CBaseEntity::PerformCustomPhysics");
			
			// TEST! REMOVE ME!
//			MOD_ADD_DETOUR_MEMBER(CTFPlayer_GetOverrideStepSound, "CTFPlayer::GetOverrideStepSound");
//			MOD_ADD_DETOUR_MEMBER(CTFPlayer_GetSceneSoundToken,   "CTFPlayer::GetSceneSoundToken");
		}
		
		virtual void OnUnload() override
		{
			spawners.clear();
			spawner_of_bot.clear();
		}
		
		virtual void OnDisable() override
		{
			spawners.clear();
			spawner_of_bot.clear();
		}
		
		virtual bool ShouldReceiveFrameEvents() const override { return this->IsEnabled(); }
		
		virtual void FrameUpdatePostEntityThink() override
		{
			UpdateDelayedAddConds();
			UpdateRingOfFire();
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
