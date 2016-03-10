#include "mod.h"
#include "re/nextbot.h"
#include "util/rtti.h"


class CTFBotMainAction : public Action<CTFBot> {};

class CTFBotDead : public Action<CTFBot>
{
public:
	CTFBotDead()
	{
		*((const void ***)this) = RTTI::GetVTable<CTFBotDead>();
		/* technically we should be writing the IContextualQuery vtable too */
	}
	
	virtual const char *GetName() const override { return "CTFBotDead"; }
	
private:
	IntervalTimer m_itTimeSinceDeath;
};


namespace Mod_AI_Prevent_Postmortem_Updates
{
	DETOUR_DECL_MEMBER(EventDesiredResult<CTFBot>, Action_CTFBot_OnKilled, CTFBot *actor, const CTakeDamageInfo& info)
	{
		auto action = reinterpret_cast<Action<CTFBot> *>(this);
		if (rtti_cast<CTFBotMainAction *>(action) != nullptr) {
			return EventDesiredResult<CTFBot>::ChangeTo(new CTFBotDead(), "I died!");
		}
		
		return DETOUR_MEMBER_CALL(Action_CTFBot_OnKilled)(actor, info);
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("AI:Prevent_Postmortem_Updates")
		{
			MOD_ADD_DETOUR_MEMBER(Action_CTFBot_OnKilled, "Action<CTFBot>::OnKilled");
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_ai_prevent_postmortem_updates", "0", FCVAR_NOTIFY,
		"Mod: fix bugs caused by CTFBot AI Update functions being called after bots have died",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
