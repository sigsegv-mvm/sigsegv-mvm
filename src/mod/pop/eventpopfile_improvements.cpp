#include "mod.h"
#include "stub/tfbot.h"
#include "stub/tf_objective_resource.h"
#include "stub/populators.h"
#include "util/scope.h"


namespace Mod_Pop_EventPopfile_Improvements
{
	RefCount rc_CTFBotSpawner_Spawn;
	IPopulationSpawner *spawner = nullptr;
	DETOUR_DECL_MEMBER(int, CTFBotSpawner_Spawn, const Vector& where, CUtlVector<CHandle<CBaseEntity>> *ents)
	{
		spawner = reinterpret_cast<IPopulationSpawner *>(this);
		SCOPED_INCREMENT(rc_CTFBotSpawner_Spawn);
		return DETOUR_MEMBER_CALL(CTFBotSpawner_Spawn)(where, ents);
	}
	
	DETOUR_DECL_MEMBER(void, CTFBot_AddItem, const char *item)
	{
		if (rc_CTFBotSpawner_Spawn > 0 && strncmp(item, "Zombie ", strlen("Zombie ")) == 0 && spawner != nullptr) {
			auto populator = rtti_cast<CMissionPopulator *>(spawner->m_Populator);
			if (populator != nullptr && populator->m_Objective == CTFBot::MISSION_DESTROY_SENTRIES) {
				return;
			}
		}
		
		DETOUR_MEMBER_CALL(CTFBot_AddItem)(item);
	}
	
	
	DETOUR_DECL_MEMBER(void, CPopulationManager_UpdateObjectiveResource)
	{
		DETOUR_MEMBER_CALL(CPopulationManager_UpdateObjectiveResource)();
		
		/* these should be somewhere else, maybe in shareddefs? */
		constexpr int HOLIDAY_NOHOLIDAY = 0;
		constexpr int HOLIDAY_HALLOWEEN = 2;
		
		ConVarRef tf_forced_holiday("tf_forced_holiday");
		if (TFObjectiveResource()->GetMvMEventPopfileType() != 0) {
			tf_forced_holiday.SetValue(HOLIDAY_HALLOWEEN);
		} else {
			tf_forced_holiday.SetValue(HOLIDAY_NOHOLIDAY);
		}
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Pop:EventPopfile_Improvements")
		{
			MOD_ADD_DETOUR_MEMBER(CTFBotSpawner_Spawn, "CTFBotSpawner::Spawn");
			MOD_ADD_DETOUR_MEMBER(CTFBot_AddItem,      "CTFBot::AddItem");
			
			MOD_ADD_DETOUR_MEMBER(CPopulationManager_UpdateObjectiveResource, "CPopulationManager::UpdateObjectiveResource");
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_pop_eventpopfile_improvements", "0", FCVAR_NOTIFY,
		"Mod: set tf_forced_holiday based on EventPopfile; disable zombie souls for sentry busters",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
