#include "mod.h"
#include "mod/pop/kv_conditional.h"
#include "stub/entities.h"
#include "stub/populators.h"
#include "util/scope.h"


namespace Mod_Pop_Tank_Extensions
{
	struct SpawnerData
	{
		bool disable_smokestack = false;
		float scale             = 1.00f;
	};
	
	
	std::map<CTankSpawner *, SpawnerData> spawners;
	
	
	DETOUR_DECL_MEMBER(void, CTankSpawner_dtor0)
	{
		auto spawner = reinterpret_cast<CTankSpawner *>(this);
		
	//	DevMsg("CTankSpawner %08x: dtor0\n", (uintptr_t)spawner);
		spawners.erase(spawner);
		
		DETOUR_MEMBER_CALL(CTankSpawner_dtor0)();
	}
	
	DETOUR_DECL_MEMBER(void, CTankSpawner_dtor2)
	{
		auto spawner = reinterpret_cast<CTankSpawner *>(this);
		
	//	DevMsg("CTankSpawner %08x: dtor2\n", (uintptr_t)spawner);
		spawners.erase(spawner);
		
		DETOUR_MEMBER_CALL(CTankSpawner_dtor2)();
	}
	
	
	DETOUR_DECL_MEMBER(bool, CTankSpawner_Parse, KeyValues *kv)
	{
		auto spawner = reinterpret_cast<CTankSpawner *>(this);
		
		DevMsg("CTankSpawner::Parse\n");
		
		std::vector<KeyValues *> del_kv;
		FOR_EACH_SUBKEY(kv, subkey) {
			const char *name = subkey->GetName();
			
			bool del = true;
			if (V_stricmp(name, "DisableSmokestack") == 0) {
			//	DevMsg("Got \"DisableSmokeStack\" = %d\n", subkey->GetBool());
				spawners[spawner].disable_smokestack = subkey->GetBool();
			} else if (V_stricmp(name, "Scale") == 0) {
			//	DevMsg("Got \"Scale\" = %f\n", subkey->GetFloat());
				spawners[spawner].scale = subkey->GetFloat();
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
		
		return DETOUR_MEMBER_CALL(CTankSpawner_Parse)(kv);
	}
	
	
	RefCount rc_CTankSpawner_Spawn;
	CTankSpawner *current_spawner = nullptr;
	DETOUR_DECL_MEMBER(int, CTankSpawner_Spawn, const Vector& where, CUtlVector<CHandle<CBaseEntity>> *ents)
	{
		auto spawner = reinterpret_cast<CTankSpawner *>(this);
		
	//	DevMsg("CTankSpawner::Spawn %08x\n", (uintptr_t)this);
		
		SCOPED_INCREMENT(rc_CTankSpawner_Spawn);
		current_spawner = spawner;
		
		auto result = DETOUR_MEMBER_CALL(CTankSpawner_Spawn)(where, ents);
		
		if (ents != nullptr) {
			auto it = spawners.find(spawner);
			if (it != spawners.end()) {
				SpawnerData& data = (*it).second;
				
				FOR_EACH_VEC((*ents), i) {
					CBaseEntity *ent = (*ents)[i];
					
					auto tank = rtti_cast<CTFTankBoss *>(ent);
					if (tank != nullptr) {
						if (data.scale != 1.00f) {
							/* need to call this BEFORE changing the scale; otherwise,
							 * the collision bounding box will be very screwed up */
							tank->UpdateCollisionBounds();
							
							tank->SetModelScale(data.scale);
						}
					}
				}
			}
		}
		
		return result;
	}
	
	
	DETOUR_DECL_MEMBER(int, CBaseAnimating_LookupAttachment, const char *szName)
	{
		if (rc_CTankSpawner_Spawn > 0 && current_spawner != nullptr &&
			szName != nullptr && strcmp(szName, "smoke_attachment") == 0) {
			
			auto it = spawners.find(current_spawner);
			if (it != spawners.end()) {
				SpawnerData& data = (*it).second;
				
				if (data.disable_smokestack) {
					/* return 0 so that CTFTankBoss::Spawn will assign 0 to its
					 * m_iSmokeAttachment variable, which results in skipping
					 * the particle logic in CTFTankBoss::TankBossThink */
					return 0;
				}
			}
		}
		
		return DETOUR_MEMBER_CALL(CBaseAnimating_LookupAttachment)(szName);
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Pop:Tank_Extensions")
		{
			MOD_ADD_DETOUR_MEMBER(CTankSpawner_dtor0, "CTankSpawner::~CTankSpawner [D0]");
			MOD_ADD_DETOUR_MEMBER(CTankSpawner_dtor2, "CTankSpawner::~CTankSpawner [D2]");
			
			MOD_ADD_DETOUR_MEMBER(CTankSpawner_Parse, "CTankSpawner::Parse");
			
			MOD_ADD_DETOUR_MEMBER(CTankSpawner_Spawn, "CTankSpawner::Spawn");
			
			MOD_ADD_DETOUR_MEMBER(CBaseAnimating_LookupAttachment, "CBaseAnimating::LookupAttachment");
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
	
	
	ConVar cvar_enable("sig_pop_tank_extensions", "0", FCVAR_NOTIFY,
		"Mod: enable extended KV in CTankSpawner::Parse",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
	
	
	class CKVCond_Tank : public IKVCond
	{
	public:
		virtual bool operator()() override
		{
			return s_Mod.IsEnabled();
		}
	};
	CKVCond_Tank cond;
}
