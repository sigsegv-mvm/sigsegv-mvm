#include "mod.h"
#include "stub/tfplayer.h"
#include "stub/populators.h"
#include "util/scope.h"
#include "mod/pop/kv_conditional.h"


namespace Mod_Pop_TFBot_Extensions
{
	struct AddCond
	{
		ETFCond cond   = (ETFCond)-1;
		float duration = -1.0f;
	};
	
	struct SpawnerData
	{
		std::vector<AddCond> addconds;
	};
	
	
	std::map<CTFBotSpawner *, SpawnerData> spawners;
	
	
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
	
	
	DETOUR_DECL_MEMBER(int, CTFBotSpawner_Spawn, const Vector& where, CUtlVector<CHandle<CBaseEntity>> *ents)
	{
		auto spawner = reinterpret_cast<CTFBotSpawner *>(this);
		
		auto result = DETOUR_MEMBER_CALL(CTFBotSpawner_Spawn)(where, ents);
		
		FOR_EACH_VEC((*ents), i) {
			CTFBot *bot = ToTFBot((*ents)[i]);
			if (bot != nullptr) {
				auto it = spawners.find(spawner);
				if (it != spawners.end()) {
					SpawnerData& data = (*it).second;
					
	//				DevMsg("CTFBotSpawner %08x: found %u AddCond's\n", (uintptr_t)spawner, data.addconds.size());
					for (auto addcond : data.addconds) {
	//					DevMsg("CTFBotSpawner %08x: applying AddCond(%d, %f)\n", (uintptr_t)spawner, addcond.cond, addcond.duration);
						bot->m_Shared->AddCond(addcond.cond, addcond.duration);
					}
				}
			}
		}
		
		return result;
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Pop:TFBot_Extensions")
		{
			MOD_ADD_DETOUR_MEMBER(CTFBotSpawner_dtor0, "CTFBotSpawner::~CTFBotSpawner [D0]");
			MOD_ADD_DETOUR_MEMBER(CTFBotSpawner_dtor2, "CTFBotSpawner::~CTFBotSpawner [D2]");
			
			MOD_ADD_DETOUR_MEMBER(CTFBotSpawner_Parse, "CTFBotSpawner::Parse");
			
			MOD_ADD_DETOUR_MEMBER(CTFBotSpawner_Spawn, "CTFBotSpawner::Spawn");
		}
		
		virtual void OnUnload()
		{
			spawners.clear();
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
			
			if (!enable) {
				spawners.clear();
			}
			
			this->m_bEnabled = enable;
		}
		
		bool IsEnabled() const { return this->m_bEnabled; }
		
	private:
		bool m_bEnabled = false;
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_pop_tfbot_extensions", "0", FCVAR_NOTIFY,
		"Mod: enable extended KV in CTFBotSpawner::Parse",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
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
