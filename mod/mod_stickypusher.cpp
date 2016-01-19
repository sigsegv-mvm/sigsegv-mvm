#include "mod.h"
#include "re/nextbot.h"
#include "re/path.h"


constexpr int MISSION_PUSHSTICKIES = 7;


class CTFBotMissionPushStickies : public Action<CTFBot>
{
public:
	CTFBotMissionPushStickies()
	{
		DevMsg("CTFBotMissionPushStickies::CTFBotMissionPushStickies\n");
		
		// TODO
	}
	
	virtual ~CTFBotMissionPushStickies()
	{
		DevMsg("CTFBotMissionPushStickies::~CTFBotMissionPushStickies\n");
		
		// TODO
	}
	
	virtual const char *GetName() const override { return "MissionPushStickies"; }
	
	virtual ActionResult<CTFBot> OnStart(CTFBot *actor, Action<CTFBot> *action) override
	{
		DevMsg("CTFBotMissionPushStickies::OnStart(#%d)\n", ENTINDEX(actor));
		
		// TODO
		
		return ActionResult<CTFBot>::Continue();
	}
	
	virtual ActionResult<CTFBot> Update(CTFBot *actor, float dt) override
	{
		DevMsg("CTFBotMissionPushStickies::Update(#%d)\n", ENTINDEX(actor));
		
		CTFBotPathCost cost_func((CTFBot *)0x11111111, RETREAT_ROUTE);
		
		
		// TODO
		
		return ActionResult<CTFBot>::Continue();
	}
	
	virtual void OnEnd(CTFBot *actor, Action<CTFBot> *action) override
	{
		DevMsg("CTFBotMissionPushStickies::OnEnd(#%d)\n", ENTINDEX(actor));
		
		// TODO
	}
};


static RefCount rc_CMissionPopulator_Parse;
DETOUR_DECL_MEMBER(void, CMissionPopulator_Parse, KeyValues *kv)
{
	SCOPED_INCREMENT(rc_CMissionPopulator_Parse);
	return DETOUR_MEMBER_CALL(CMissionPopulator_Parse)(kv);
}

DETOUR_DECL_MEMBER(bool, CSpawnLocation_Parse, KeyValues *kv)
{
	if (rc_CMissionPopulator_Parse.NonZero()) {
		if (V_stricmp(kv->GetName(), "Objective") == 0 &&
			V_stricmp(kv->GetString(), "PushStickies") == 0) {
			int *p_m_Objective = (int *)((uintptr_t)this - (offsetof(CMissionPopulator, m_Where) -
				offsetof(CMissionPopulator, m_Objective)));
			
			*p_m_Objective = MISSION_PUSHSTICKIES;
			return true;
		}
	}
	
	return DETOUR_MEMBER_CALL(CSpawnLocation_Parse)(kv);
}


DETOUR_DECL_MEMBER(void, CMissionPopulator_Update)
{
	DETOUR_MEMBER_CALL(CMissionPopulator_Update)();
	
	CMissionPopulator *realthis = reinterpret_cast<CMissionPopulator *>(this);
	
	int objective = realthis->m_Objective;
	if (objective == MISSION_PUSHSTICKIES) {
		realthis->UpdateMission(objective);
	}
}


// TODO: detour to make CMissionPopulator::Update call CMissionPopulator::UpdateMission for mission numbers above 5
// TODO: detour to add "PushStickies" as a valid Objective KV for CMissionPopulator::Parse
// TODO: detour to make CTFBotScenarioMonitor::DesiredScenarioAndClassAction select CTFBotMissionPushStickies for MISSION_PUSHSTICKIES


DETOUR_DECL_MEMBER(Action<CTFBot> *, CTFBotScenarioMonitor_DesiredScenarioAndClassAction, CTFBot *actor)
{
	DevMsg("CTFBotScenarioMonitor::DesiredScenarioAndClassAction\n");
	
	int mission = actor->GetMission();
	DevMsg("actor->m_nMission = %d\n", mission);
	
	return new CTFBotMissionPushStickies();
	
	//DETOUR_MEMBER_CALL(CTFBotScenarioMonitor_DesiredScenarioAndClassAction)(actor);
}


class CMod_StickyPusher : public IMod
{
public:
	CMod_StickyPusher() : IMod("StickyPusher")
	{
		MOD_ADD_DETOUR_MEMBER(CMissionPopulator, Parse);
		MOD_ADD_DETOUR_MEMBER(CSpawnLocation, Parse);
		
		MOD_ADD_DETOUR_MEMBER(CMissionPopulator, Update);
		
		MOD_ADD_DETOUR_MEMBER(CTFBotScenarioMonitor, DesiredScenarioAndClassAction);
	}
	
	void SetEnabled(bool enable)
	{
		this->ToggleAllDetours(enable);
	}
};
static CMod_StickyPusher s_Mod;


static ConVar cvar_enable("sigsegv_stickypusher_enable", "0", FCVAR_NOTIFY,
	"Mod: add new CTFBotMissionStickyPusher AI behavior",
	[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
		ConVarRef var(pConVar);
		s_Mod.SetEnabled(var.GetBool());
	});
