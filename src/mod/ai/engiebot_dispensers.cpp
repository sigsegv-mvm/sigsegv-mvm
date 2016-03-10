#include "mod.h"
#include "re/nextbot.h"
#include "re/path.h"
#include "stub/tfbot.h"
#include "stub/objects.h"
#include "stub/entities.h"
#include "util/rtti.h"


#define TRACE_ENABLE 1
#define TRACE_TERSE  0
#include "util/trace.h"


namespace Mod_AI_EngieBot_Dispensers
{
	class CTFBotMvMEngineerBuildSGDispenser : public IHotplugAction<CTFBotMvMEngineerBuildSGDispenser>
	{
	public:
		CTFBotMvMEngineerBuildSGDispenser(CTFBotHintSentrygun *hint) :
			m_hHint(hint)
		{
			TRACE("[this: %08x] [hint: #%d %08x]", (uintptr_t)this, ENTINDEX(hint), (uintptr_t)hint);
		}
		
		virtual const char *GetName() const override { return "MvMEngineerBuildSGDispenser"; }
		
		
		virtual ActionResult<CTFBot> OnStart(CTFBot *actor, Action<CTFBot> *action) override
		{
			TRACE("[this: %08x] [actor: #%d]", (uintptr_t)this, ENTINDEX(actor));
			
			actor->StartBuildingObjectOfType(OBJ_DISPENSER, 0);
			
			return ActionResult<CTFBot>::Continue();
		}
		
		virtual ActionResult<CTFBot> Update(CTFBot *actor, float dt) override
		{
			TRACE("[this: %08x] [actor: #%d]", (uintptr_t)this, ENTINDEX(actor));
			
			if (this->m_hHint == nullptr) {
				return ActionResult<CTFBot>::Done("No hint entity");
			}
			
			INextBot *nextbot = rtti_cast<INextBot *>(actor);
			
			float range_to_hint = nextbot->GetRangeTo(this->m_hHint->GetAbsOrigin());
			
			if (range_to_hint < 200.0f) {
				TRACE_MSG("range_to_hint < 200: crouching/aiming\n");
				actor->PressCrouchButton();
				actor->GetBodyInterface()->AimHeadTowards(this->m_hHint->GetAbsOrigin(),
					IBody::LookAtPriorityType::OVERRIDE, 0.1f, nullptr, "Placing dispenser");
			}
			
			if (range_to_hint > 25.0f) {
				TRACE_MSG("range_to_hint > 25: pathing\n");
				
				if (this->m_ctRecomputePath.IsElapsed()) {
					TRACE_MSG("recomputing path\n");
					
					this->m_ctRecomputePath.Start(RandomFloat(1.0f, 2.0f));
					
					CTFBotPathCost cost_func(actor, SAFEST_ROUTE);
					this->m_PathFollower.Compute(nextbot, this->m_hHint->GetAbsOrigin(), cost_func, 0.0f, true);
				}
				
				this->m_PathFollower.Update(nextbot);
				if (!this->m_PathFollower.IsValid()) {
					return ActionResult<CTFBot>::Done("Path failed");
				}
				
				return ActionResult<CTFBot>::Continue();
			}
			
			TRACE_MSG("at hint: creating dispenser entity\n");
			
			CBaseEntity *ent = CreateEntityByName("obj_dispenser");
			if (ent == nullptr) {
				return ActionResult<CTFBot>::Done("Couldn't create entity");
			}
			
			// TODO: increment hint dword 0x370 (not important for mvm)
			
			auto dispenser = rtti_cast<CObjectDispenser *>(ent);
			dispenser->SetName(this->m_hHint->GetEntityName());
			dispenser->m_nDefaultUpgradeLevel = 2;
			dispenser->SetAbsOrigin(this->m_hHint->GetAbsOrigin());
			dispenser->SetAbsAngles(this->m_hHint->GetAbsAngles());
			dispenser->Spawn();
			
			dispenser->StartPlacement(actor);
			dispenser->StartBuilding(actor);
			
			dispenser->SetOwnerEntity(actor);
			
			return ActionResult<CTFBot>::Done("Built a dispenser");
		}
		
		
	private:
		CHandle<CTFBotHintSentrygun> m_hHint;
		PathFollower m_PathFollower;
		CountdownTimer m_ctRecomputePath;
	};
	
	class CTFBotMvMEngineerBuildTEDispenser : public IHotplugAction<CTFBotMvMEngineerBuildTEDispenser>
	{
	public:
		CTFBotMvMEngineerBuildTEDispenser(CTFBotHintTeleporterExit *hint) :
			m_hHint(hint)
		{
			TRACE("[this: %08x] [hint: #%d %08x]", (uintptr_t)this, ENTINDEX(hint), (uintptr_t)hint);
		}
		
		virtual const char *GetName() const override { return "MvMEngineerBuildTEDispenser"; }
		
		
		virtual ActionResult<CTFBot> OnStart(CTFBot *actor, Action<CTFBot> *action) override
		{
			TRACE("[this: %08x] [actor: #%d]", (uintptr_t)this, ENTINDEX(actor));
			
			return ActionResult<CTFBot>::Continue();
		}
		
		virtual ActionResult<CTFBot> Update(CTFBot *actor, float dt) override
		{
			TRACE("[this: %08x] [actor: #%d]", (uintptr_t)this, ENTINDEX(actor));
			
			if (this->m_hHint == nullptr) {
				return ActionResult<CTFBot>::Done("No hint entity");
			}
			
			INextBot *nextbot = rtti_cast<INextBot *>(actor);
			
			if (nextbot->IsRangeGreaterThan(this->m_hHint->GetAbsOrigin(), 25.0f)) {
				TRACE_MSG("range_to_hint > 25: pathing\n");
				
				if (this->m_ctRecomputePath.IsElapsed()) {
					TRACE_MSG("recomputing path\n");
					
					this->m_ctRecomputePath.Start(RandomFloat(1.0f, 2.0f));
					
					CTFBotPathCost cost_func(actor, FASTEST_ROUTE);
					this->m_PathFollower.Compute(nextbot, this->m_hHint->GetAbsOrigin(), cost_func, 0.0f, true);
				}
				
				this->m_PathFollower.Update(nextbot);
				if (!this->m_PathFollower.IsValid()) {
					return ActionResult<CTFBot>::Done("Path failed");
				}
				
				return ActionResult<CTFBot>::Continue();
			}
			
			TRACE_MSG("at hint: creating dispenser entity\n");
			
			CBaseEntity *ent = CreateEntityByName("obj_dispenser");
			if (ent == nullptr) {
				return ActionResult<CTFBot>::Done("Couldn't create entity");
			}
			
			auto dispenser = rtti_cast<CObjectDispenser *>(ent);
			dispenser->SetAbsOrigin(this->m_hHint->GetAbsOrigin());
			dispenser->SetAbsAngles(this->m_hHint->GetAbsAngles());
			dispenser->Spawn();
			
			dispenser->StartPlacement(actor);
			dispenser->StartBuilding(actor);
			
			dispenser->SetOwnerEntity(actor);
			
			return ActionResult<CTFBot>::Done("Built a dispenser");
		}
		
		
	private:
		CHandle<CTFBotHintTeleporterExit> m_hHint;
		PathFollower m_PathFollower;
		CountdownTimer m_ctRecomputePath;
	};
	
	
	CTFBotHintSentrygun *hint_sg = nullptr;
	DETOUR_DECL_MEMBER(void, CTFBotMvMEngineerBuildSentryGun_ctor, CTFBotHintSentrygun *hint)
	{
		TRACE();
		
		hint_sg = hint;
		DETOUR_MEMBER_CALL(CTFBotMvMEngineerBuildSentryGun_ctor)(hint);
	}
	
	CTFBotHintTeleporterExit *hint_te = nullptr;
	DETOUR_DECL_MEMBER(void, CTFBotMvMEngineerBuildTeleportExit_ctor, CTFBotHintTeleporterExit *hint)
	{
		TRACE();
		
		hint_te = hint;
		DETOUR_MEMBER_CALL(CTFBotMvMEngineerBuildTeleportExit_ctor)(hint);
	}
	
	
	DETOUR_DECL_MEMBER(ActionResult<CTFBot>, CTFBotMvMEngineerIdle_Update, CTFBot *actor, float dt)
	{
		TRACE();
		
		auto result = DETOUR_MEMBER_CALL(CTFBotMvMEngineerIdle_Update)(actor, dt);
		
		if (result.transition == ActionTransition::SUSPEND_FOR && result.action != nullptr) {
			if (strcmp(result.action->GetName(), "MvMEngineerBuildSentryGun") == 0 && hint_sg != nullptr) {
				delete result.action;
				result.action = new CTFBotMvMEngineerBuildSGDispenser(hint_sg);
				hint_sg = nullptr;
			} else if (strcmp(result.action->GetName(), "MvMEngineerBuildTeleportExit") == 0 && hint_te != nullptr) {
				delete result.action;
				result.action = new CTFBotMvMEngineerBuildTEDispenser(hint_te);
				hint_te = nullptr;
			}
		}
		
		return result;
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("AI:EngieBot_Dispensers")
		{
			MOD_ADD_DETOUR_MEMBER(CTFBotMvMEngineerBuildSentryGun_ctor,    "CTFBotMvMEngineerBuildSentryGun::CTFBotMvMEngineerBuildSentryGun [C1]");
			MOD_ADD_DETOUR_MEMBER(CTFBotMvMEngineerBuildTeleportExit_ctor, "CTFBotMvMEngineerBuildTeleportExit::CTFBotMvMEngineerBuildTeleportExit [C1]");
			
			MOD_ADD_DETOUR_MEMBER(CTFBotMvMEngineerIdle_Update, "CTFBotMvMEngineerIdle::Update");
		}
		
		virtual void OnUnload() override
		{
			CTFBotMvMEngineerBuildSGDispenser::UnloadAll();
			CTFBotMvMEngineerBuildTEDispenser::UnloadAll();
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_ai_engiebot_dispensers", "0", FCVAR_NOTIFY,
		"Mod: make engiebots build dispensers instead of sentries/teles",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
