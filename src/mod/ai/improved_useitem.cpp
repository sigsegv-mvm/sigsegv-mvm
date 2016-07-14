#include "mod.h"
#include "stub/tfweaponbase.h"
#include "re/nextbot.h"
#include "stub/tfbot.h"


namespace Mod_AI_Improved_UseItem
{
	ConVar cvar_debug("sig_ai_improved_useitem_debug", "0", FCVAR_NOTIFY,
		"Mod: debug CTFBotUseItemImproved and descendants");
	
	
	class CTFBotUseItem : public Action<CTFBot>
	{
	public:
		CHandle<CTFWeaponBase> m_hItem;
	};
	
	
	class CTFBotUseItemImproved : public Action<CTFBot>
	{
	public:
		enum class State : int
		{
			FIRE, // wait for the weapon switch delay, then use it
			WAIT, // wait for the item to take effect, then finish
		};
		
		CTFBotUseItemImproved(CTFWeaponBase *item) :
			m_hItem(item) {}
		
		virtual ActionResult<CTFBot> OnStart(CTFBot *actor, Action<CTFBot> *action) override
		{
			if (cvar_debug.GetBool()) {
				DevMsg("[%8.3f] CTFBotUseItemImproved(#%d): OnStart\n", gpGlobals->curtime, ENTINDEX(actor));
			}
			
			this->m_State = State::FIRE;
			
			this->m_flSwitchTime = this->m_hItem->m_flNextPrimaryAttack;
			actor->PushRequiredWeapon(this->m_hItem);
			
			return ActionResult<CTFBot>::Continue();
		}
		
		virtual ActionResult<CTFBot> Update(CTFBot *actor, float dt) override
		{
			switch (this->m_State) {
				
			case State::FIRE:
				if (gpGlobals->curtime >= this->m_flSwitchTime) {
					if (cvar_debug.GetBool()) {
						DevMsg("[%8.3f] CTFBotUseItemImproved(#%d): Using item now\n", gpGlobals->curtime, ENTINDEX(actor));
					}
					actor->PressFireButton(1.0f);
					this->m_State = State::WAIT;
				}
				break;
				
			case State::WAIT:
				if (this->IsDone(actor)) {
					if (cvar_debug.GetBool()) {
						DevMsg("[%8.3f] CTFBotUseItemImproved(#%d): Done using item\n", gpGlobals->curtime, ENTINDEX(actor));
					}
					return ActionResult<CTFBot>::Done("Item used");
				}
				break;
			}
			
			return ActionResult<CTFBot>::Continue();
		}
		
		virtual void OnEnd(CTFBot *actor, Action<CTFBot> *action) override
		{
			if (cvar_debug.GetBool()) {
				DevMsg("[%8.3f] CTFBotUseItemImproved(#%d): OnEnd\n", gpGlobals->curtime, ENTINDEX(actor));
			}
			
			actor->PopRequiredWeapon();
		}
		
	protected:
		virtual bool IsDone(CTFBot *actor) const = 0;
		
	private:
		CHandle<CTFWeaponBase> m_hItem;
		
		State m_State;
		float m_flSwitchTime;
	};
	
	
	class CTFBotUseBuffItem : public CTFBotUseItemImproved
	{
	public:
		CTFBotUseBuffItem(CTFWeaponBase *item) :
			CTFBotUseItemImproved(item) {}
		
		virtual const char *GetName() const override { return "UseBuffItem"; }
		
	private:
		virtual bool IsDone(CTFBot *actor) const override
		{
			return actor->m_Shared->m_bRageDraining;
		}
	};
	
	class CTFBotUseLunchBoxItem : public CTFBotUseItemImproved
	{
	public:
		CTFBotUseLunchBoxItem(CTFWeaponBase *item) :
			CTFBotUseItemImproved(item) {}
		
		virtual const char *GetName() const override { return "UseLunchBoxItem"; }
		
	private:
		virtual bool IsDone(CTFBot *actor) const override
		{
			// FIXME: this is actually backwards
			return actor->m_Shared->InCond(TF_COND_TAUNTING);
		}
	};
	
	
	DETOUR_DECL_MEMBER(Action<CTFBot> *, CTFBot_OpportunisticallyUseWeaponAbilities)
	{
		CTFBotUseItem *result = reinterpret_cast<CTFBotUseItem *>(DETOUR_MEMBER_CALL(CTFBot_OpportunisticallyUseWeaponAbilities)());
		if (result != nullptr) {
			CTFWeaponBase *item = result->m_hItem;
			
			if (item->GetWeaponID() == TF_WEAPON_BUFF_ITEM) {
				delete result;
				return new CTFBotUseBuffItem(item);
			}
			
			if (item->GetWeaponID() == TF_WEAPON_LUNCHBOX) {
				delete result;
				return new CTFBotUseLunchBoxItem(item);
			}
		}
		
		return result;
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("AI:Improved_UseItem")
		{
			MOD_ADD_DETOUR_MEMBER(CTFBot_OpportunisticallyUseWeaponAbilities, "CTFBot::OpportunisticallyUseWeaponAbilities");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_ai_improved_useitem", "0", FCVAR_NOTIFY,
		"Mod: use improved replacement for CTFBotUseItem",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
