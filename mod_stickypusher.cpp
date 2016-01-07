#include "mod.h"
#include "re_nextbot.h"


constexpr int MISSION_PUSHSTICKIES = 7;


class CTFBotMissionPushStickies : public Action<CTFBot>
{
public:
	CTFBotMissionPushStickies()
	{
		DevMsg("%s\n", __PRETTY_FUNCTION__);
		
		// TODO
	}
	
	virtual ~CTFBotMissionPushStickies()
	{
		DevMsg("%s\n", __PRETTY_FUNCTION__);
		
		// TODO
	}
	
	virtual const char *GetName() const override { return "MissionPushStickies"; }
	
	virtual ActionResult<CTFBot> OnStart(CTFBot *actor, Action<CTFBot> *action) override
	{
		DevMsg("%s\n", __PRETTY_FUNCTION__);
		
		// TODO
		
		return ActionResult<CTFBot>::Continue();
	}
	
	virtual ActionResult<CTFBot> Update(CTFBot *actor, float dt) override
	{
		DevMsg("%s\n", __PRETTY_FUNCTION__);
		
		// TODO
		
		return ActionResult<CTFBot>::Continue();
	}
	
	virtual void OnEnd(CTFBot *actor, Action<CTFBot> *action) override
	{
		DevMsg("%s\n", __PRETTY_FUNCTION__);
		
		// TODO
	}
};


// TODO: detour to make CMissionPopulator::Update call CMissionPopulator::UpdateMission for mission numbers above 5
// TODO: detour to add "PushStickies" as a valid Objective KV for CMissionPopulator::Parse
// TODO: detour to make CTFBotScenarioMonitor::DesiredScenarioAndClassAction select CTFBotMissionPushStickies for MISSION_PUSHSTICKIES

// TODO: make sure vtable for CTFBotMissionPushStickies is getting set up right...

DETOUR_DECL_MEMBER(Action<CTFBot> *, CTFBotScenarioMonitor_DesiredScenarioAndClassAction, CTFBot *actor)
{
	return new CTFBotMissionPushStickies();
	
	//DETOUR_MEMBER_CALL(CTFBotScenarioMonitor_DesiredScenarioAndClassAction)(actor);
}


class CMod_StickyPusher : public IMod
{
public:
	CMod_StickyPusher() : IMod("StickyPusher")
	{
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
