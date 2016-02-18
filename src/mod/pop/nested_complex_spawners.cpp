#include "mod.h"
#include "stub/tf_objective_resource.h"
#include "stub/populators.h"


namespace Mod_Pop_Nested_Complex_Spawners
{
	struct WaveClassCount_t
	{
		int count;
		string_t icon;
		unsigned int flags;
	};
	
	
	const char *ClassFlags_ToString(unsigned int flags)
	{
		static char buf[4096];
		buf[0] = '\0';
		
		if ((flags & CLASSFLAG_NORMAL) != 0) {
			
		}
		
		return buf;
	}
	
	
	/* keep track of how many bots we have virtually spawned from each
	 * RandomChoice spawner so we can match how it will behave when spawning */
	std::map<CRandomChoiceSpawner *, int> randomchoice_tracker;
	
	
	void CRandomChoiceSpawner_GenerateRandomIndexes(CRandomChoiceSpawner *spawner, int index)
	{
		/* this is pretty much straight out of mvm-reversed */
		
		int size_before = spawner->m_Indexes.Count();
		int size_wanted = (index + 1);
		
		int size_growby = size_wanted - size_before;
		if (size_growby > 0) {
			spawner->m_Indexes.AddMultipleToTail(size_growby);
			
			for (int i = size_before; i < size_wanted; ++i) {
				spawner->m_Indexes[i] = RandomInt(0, spawner->m_SubSpawners.Count() - 1);
			}
		}
	}
	
	
	int CRandomChoiceSpawner_GetNextIndex(CRandomChoiceSpawner *spawner)
	{
		/* will default to zero if not yet in the map */
		int& ordinal = randomchoice_tracker[spawner];
		CRandomChoiceSpawner_GenerateRandomIndexes(spawner, ordinal);
		return spawner->m_Indexes[ordinal++];
	}
	
	
	int IPopulationSpawner_CountClasses(CWaveSpawnPopulator *wavespawn, IPopulationSpawner *spawner, std::vector<WaveClassCount_t>& counts)
	{
		/* TODO: remove this garbage */
		constexpr ptrdiff_t OFF_CWaveSpawnPopulator_m_bSupport        = 0x4cc;
		constexpr ptrdiff_t OFF_CWaveSpawnPopulator_m_bSupportLimited = 0x4cd;
		
		bool *m_bSupport        = (bool *)((uintptr_t)wavespawn + OFF_CWaveSpawnPopulator_m_bSupport);
		bool *m_bSupportLimited = (bool *)((uintptr_t)wavespawn + OFF_CWaveSpawnPopulator_m_bSupportLimited);
		
		
		auto sp_mob          = rtti_cast<CMobSpawner *         >(spawner);
		auto sp_sentrygun    = rtti_cast<CSentryGunSpawner *   >(spawner);
		auto sp_tank         = rtti_cast<CTankSpawner *        >(spawner);
		auto sp_tfbot        = rtti_cast<CTFBotSpawner *       >(spawner);
		auto sp_squad        = rtti_cast<CSquadSpawner *       >(spawner);
		auto sp_randomchoice = rtti_cast<CRandomChoiceSpawner *>(spawner);
		
		int num_spawned = 0;
		
		static int rlevel = 2;
		++rlevel;
		
		/* SentryGun: leaf */
		if (sp_sentrygun != nullptr) {
			DevMsg("%*s[SentryGun] NOT IMPLEMENTED\n", rlevel * 2, "");
		}
		
		/* Tank/TFBot: leaf */
		if (sp_tank != nullptr || sp_tfbot != nullptr) {
			WaveClassCount_t count = {
				1,
				spawner->GetClassIcon(-1),
				0,
			};
			
			if (!*m_bSupport)                                        count.flags |= CLASSFLAG_NORMAL;
			if (*m_bSupport)                                         count.flags |= CLASSFLAG_SUPPORT;
			if (*m_bSupportLimited)                                  count.flags |= CLASSFLAG_SUPPORT_LIMITED;
			if (spawner->IsMiniBoss(-1))                             count.flags |= CLASSFLAG_MINIBOSS;
			if (spawner->HasAttribute(CTFBot::ATTR_ALWAYS_CRIT, -1)) count.flags |= CLASSFLAG_CRITICAL;
			
			DevMsg("%*s[%s] \"%s\" %s\n", rlevel * 2, "",
				(sp_tfbot != nullptr ? "TFBot" : "Tank"),
				count.icon, ClassFlags_ToString(count.flags));
			counts.push_back(count);
			++num_spawned;
		}
		
		/* Mob: recursive */
		if (sp_mob != nullptr) {
			DevMsg("%*s[Mob]\n", rlevel * 2, "");
			
			num_spawned += IPopulationSpawner_CountClasses(wavespawn, sp_mob->m_SubSpawner, counts);
		}
		
		/* Squad: recursive */
		if (sp_squad != nullptr) {
			DevMsg("%*s[Squad]\n", rlevel * 2, "");
			
			FOR_EACH_VEC(sp_squad->m_SubSpawners, i) {
				auto sub = sp_squad->m_SubSpawners[i];
				num_spawned += IPopulationSpawner_CountClasses(wavespawn, sub, counts);
			}
		}
		
		/* RandomChoice: recursive */
		if (sp_randomchoice != nullptr) {
			DevMsg("%*s[RandomChoice]\n", rlevel * 2, "");
			
			auto sub = sp_randomchoice->m_SubSpawners[CRandomChoiceSpawner_GetNextIndex(sp_randomchoice)];
			num_spawned += IPopulationSpawner_CountClasses(wavespawn, sub, counts);
		}
		
		--rlevel;
		
		return num_spawned;
	}
	
	void CWave_CountClasses(CWave *wave)
	{
		DevMsg("[Wave]\n");
		
		randomchoice_tracker.clear();
		
		std::vector<WaveClassCount_t> counts;
		
		FOR_EACH_VEC(wave->m_WaveSpawns, i) {
			DevMsg("  [WaveSpawn]\n");
			auto wavespawn = wave->m_WaveSpawns[i];
			
			/* TODO: remove this garbage */
			constexpr ptrdiff_t OFF_CWaveSpawnPopulator_m_iTotalCount = 0x030;
			int *m_iTotalCount = (int *)((uintptr_t)wavespawn + OFF_CWaveSpawnPopulator_m_iTotalCount);
			
			int total = *m_iTotalCount;
			while (total > 0) {
				total -= IPopulationSpawner_CountClasses(wavespawn, wavespawn->m_Spawner, counts);
			}
		}
		
		for (const auto& count : counts) {
			wave->AddClassType(count.icon, count.count, count.flags);
		}
	}
	
	
	RefCount rc_CWave_Parse_actual;
	DETOUR_DECL_MEMBER(bool, CWave_Parse, KeyValues *kv)
	{
		++rc_CWave_Parse_actual;
		auto result = DETOUR_MEMBER_CALL(CWave_Parse)(kv);
		--rc_CWave_Parse_actual;
		
		if (result) {
			auto wave = reinterpret_cast<CWave *>(this);
			CWave_CountClasses(wave);
		}
		
		return result;
	}
	
	
	DETOUR_DECL_MEMBER(void, CWave_AddClassType, string_t icon, int count, unsigned int flags)
	{
		if (rc_CWave_Parse_actual > 0) return;
		DETOUR_MEMBER_CALL(CWave_AddClassType)(icon, count, flags);
	}
	
	DETOUR_DECL_MEMBER(string_t, CSquadSpawner_GetClassIcon, int index)
	{
		if (rc_CWave_Parse_actual > 0) return NULL_STRING;
		return DETOUR_MEMBER_CALL(CSquadSpawner_GetClassIcon)(index);
	}
	DETOUR_DECL_MEMBER(bool, CSquadSpawner_IsMiniBoss, int index)
	{
		if (rc_CWave_Parse_actual > 0) return false;
		return DETOUR_MEMBER_CALL(CSquadSpawner_IsMiniBoss)(index);
	}
	DETOUR_DECL_MEMBER(bool, CSquadSpawner_HasAttribute, CTFBot::AttributeType attr, int index)
	{
		if (rc_CWave_Parse_actual > 0) return false;
		return DETOUR_MEMBER_CALL(CSquadSpawner_HasAttribute)(attr, index);
	}
	
	DETOUR_DECL_MEMBER(string_t, CRandomChoiceSpawner_GetClassIcon, int index)
	{
		if (rc_CWave_Parse_actual > 0) return NULL_STRING;
		return DETOUR_MEMBER_CALL(CRandomChoiceSpawner_GetClassIcon)(index);
	}
	DETOUR_DECL_MEMBER(bool, CRandomChoiceSpawner_IsMiniBoss, int index)
	{
		if (rc_CWave_Parse_actual > 0) return false;
		return DETOUR_MEMBER_CALL(CRandomChoiceSpawner_IsMiniBoss)(index);
	}
	DETOUR_DECL_MEMBER(bool, CRandomChoiceSpawner_HasAttribute, CTFBot::AttributeType attr, int index)
	{
		if (rc_CWave_Parse_actual > 0) return false;
		return DETOUR_MEMBER_CALL(CRandomChoiceSpawner_HasAttribute)(attr, index);
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Pop:Nested_Complex_Spawners")
		{
			MOD_ADD_DETOUR_MEMBER(CWave_Parse,        "CWave::Parse");
			MOD_ADD_DETOUR_MEMBER(CWave_AddClassType, "CWave::AddClassType");
			
			MOD_ADD_DETOUR_MEMBER(CSquadSpawner_GetClassIcon, "CSquadSpawner::GetClassIcon");
			MOD_ADD_DETOUR_MEMBER(CSquadSpawner_IsMiniBoss,   "CSquadSpawner::IsMiniBoss");
			MOD_ADD_DETOUR_MEMBER(CSquadSpawner_HasAttribute, "CSquadSpawner::HasAttribute");
			
			MOD_ADD_DETOUR_MEMBER(CRandomChoiceSpawner_GetClassIcon, "CRandomChoiceSpawner::GetClassIcon");
			MOD_ADD_DETOUR_MEMBER(CRandomChoiceSpawner_IsMiniBoss,   "CRandomChoiceSpawner::IsMiniBoss");
			MOD_ADD_DETOUR_MEMBER(CRandomChoiceSpawner_HasAttribute, "CRandomChoiceSpawner::HasAttribute");
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_pop_nested_complex_spawners", "0", FCVAR_NOTIFY,
		"Mod: calculate correct wave class counts for nested complex spawners using a recursive algorithm",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
