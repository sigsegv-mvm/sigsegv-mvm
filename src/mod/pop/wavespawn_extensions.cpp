#include "mod.h"
#include "mod/pop/kv_conditional.h"
#include "stub/populators.h"


namespace Mod::Pop::WaveSpawn_Extensions
{
	struct WaveSpawnData
	{
		/* nothing yet */
	};
	
	
	std::map<CWaveSpawnPopulator *, WaveSpawnData> wavespawns;
	
	
	DETOUR_DECL_MEMBER(void, CWaveSpawnPopulator_dtor0)
	{
		auto wavespawn = reinterpret_cast<CWaveSpawnPopulator *>(this);
		
	//	DevMsg("CWaveSpawnPopulator %08x: dtor0\n", (uintptr_t)wavespawn);
		wavespawns.erase(wavespawn);
		
		DETOUR_MEMBER_CALL(CWaveSpawnPopulator_dtor0)();
	}
	
	DETOUR_DECL_MEMBER(void, CWaveSpawnPopulator_dtor2)
	{
		auto wavespawn = reinterpret_cast<CWaveSpawnPopulator *>(this);
		
	//	DevMsg("CWaveSpawnPopulator %08x: dtor2\n", (uintptr_t)wavespawn);
		wavespawns.erase(wavespawn);
		
		DETOUR_MEMBER_CALL(CWaveSpawnPopulator_dtor2)();
	}
	
	
	DETOUR_DECL_MEMBER(bool, CWaveSpawnPopulator_Parse, KeyValues *kv_orig)
	{
		auto wavespawn = reinterpret_cast<CWaveSpawnPopulator *>(this);
		
		// make a temporary copy of the KV subtree for this populator
		// the reason for this: `kv_orig` *might* be a ptr to a shared template KV subtree
		// we'll be deleting our custom keys after we parse them so that the Valve code doesn't see them
		// but we don't want to delete them from the shared template KV subtree (breaks multiple uses of the template)
		// so we use this temp copy, delete things from it, pass it to the Valve code, then delete it
		// (we do the same thing in Pop:TFBot_Extensions)
		KeyValues *kv = kv_orig->MakeCopy();
		
	//	DevMsg("CWaveSpawnPopulator::Parse\n");
		
		std::vector<KeyValues *> del_kv;
		FOR_EACH_SUBKEY(kv, subkey) {
			const char *name = subkey->GetName();
			
			bool del = true;
			if (false /* FStrEq(name, "Something") */) {
				/* do something custom here */
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
		
		bool result = DETOUR_MEMBER_CALL(CWaveSpawnPopulator_Parse)(kv);
		
		// delete the temporary copy of the KV subtree
		kv->deleteThis();
		
		return result;
	}
	
	
	class CMod : public IMod, public IModCallbackListener
	{
	public:
		CMod() : IMod("Pop:WaveSpawn_Extensions")
		{
			MOD_ADD_DETOUR_MEMBER(CWaveSpawnPopulator_dtor0, "CWaveSpawnPopulator::~CWaveSpawnPopulator [D0]");
			MOD_ADD_DETOUR_MEMBER(CWaveSpawnPopulator_dtor2, "CWaveSpawnPopulator::~CWaveSpawnPopulator [D2]");
			
			MOD_ADD_DETOUR_MEMBER(CWaveSpawnPopulator_Parse, "CWaveSpawnPopulator::Parse");
		}
		
		virtual void OnUnload() override
		{
			wavespawns.clear();
		}
		
		virtual void OnDisable() override
		{
			wavespawns.clear();
		}
		
		virtual bool ShouldReceiveCallbacks() const override { return this->IsEnabled(); }
		
		virtual void LevelInitPreEntity() override
		{
			wavespawns.clear();
		}
		
		virtual void LevelShutdownPostEntity() override
		{
			wavespawns.clear();
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_pop_wavespawn_extensions", "0", FCVAR_NOTIFY,
		"Mod: enable extended KV in CWaveSpawnPopulator::Parse",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
	
	
	class CKVCond_WaveSpawn : public IKVCond
	{
	public:
		virtual bool operator()() override
		{
			return s_Mod.IsEnabled();
		}
	};
	CKVCond_WaveSpawn cond;
}
