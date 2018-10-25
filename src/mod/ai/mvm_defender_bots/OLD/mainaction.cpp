#include "mod/ai/mvm_defender_bots/mainaction.h"
#include "mod/ai/mvm_defender_bots/defendgate.h"
#include "mod/ai/mvm_defender_bots/attacktank.h"
#include "mod/ai/mvm_defender_bots/collectmoney.h"
#include "stub/tfbot_behavior.h"


namespace Mod::AI::MvM_Defender_Bots
{
	QueryResponse CTFBotMvMDefender::ShouldHurry(const INextBot *nextbot) const
	{
		// TODO
		return QueryResponse::DONTCARE;
	}
	
	QueryResponse CTFBotMvMDefender::ShouldRetreat(const INextBot *nextbot) const
	{
		// TODO
		return QueryResponse::DONTCARE;
	}
	
	QueryResponse CTFBotMvMDefender::ShouldAttack(const INextBot *nextbot, const CKnownEntity *threat) const
	{
		// TODO
		return QueryResponse::DONTCARE;
	}
	
	
	ActionResult<CTFBot> CTFBotMvMDefender::OnStart(CTFBot *actor, Action<CTFBot> *action)
	{
		// TODO
		return ActionResult<CTFBot>::Continue();
	}
	
	ActionResult<CTFBot> CTFBotMvMDefender::Update(CTFBot *actor, float dt)
	{
		if (actor->IsPlayerClass(TF_CLASS_SCOUT) && CTFBotCollectMoney::IsPossible(actor)) {
			return ActionResult<CTFBot>::SuspendFor(new CTFBotCollectMoney(), "Collecting money");
		}
		
		// TODO
		return ActionResult<CTFBot>::Continue();
	}
	
	void CTFBotMvMDefender::OnEnd(CTFBot *actor, Action<CTFBot> *action)
	{
		// TODO
	}
	
	
	Action<CTFBot> *CTFBotMvMDefender::InitialContainedAction(CTFBot *actor)
	{
		if (actor->IsPlayerClass(TF_CLASS_ENGINEER)) {
			return CTFBotEngineerBuild::New();
		}
		
		if (actor->IsPlayerClass(TF_CLASS_MEDIC)) {
			return CTFBotMedicHeal::New();
		}
		
		if (actor->IsPlayerClass(TF_CLASS_SPY)) {
			return CTFBotSpyInfiltrate::New();
		}
		
		if (actor->IsPlayerClass(TF_CLASS_SNIPER)) {
			auto weapon = actor->GetActiveTFWeapon();
			if (weapon != nullptr && WeaponID_IsSniperRifle(weapon->GetWeaponID())) {
				return CTFBotSniperLurk::New();
			}
		}
		
		return CTFBotSeekAndDestroy::New();
	}
	
	/*
	Action<CTFBot> *CTFBotMvMDefender::GetNextAction()
	{
		if (!this->m_ctReevaluate.IsElapsed() && this->m_LastObjective != Objective::NONE) {
			return nullptr;
		}
		// TODO: randomize the interval a bit
		this->m_ctReevaluate.Start(OBJECTIVE_REEVAL_INTERVAL);
		
		Objective next = DecideBestObjective();
		if (next == this->m_LastObjective) {
			return nullptr;
		}
		this->m_LastObjective = next;
		
		switch (next) {
		case Objective::DEFEND_FLAG:
			return new CTFBotMvMDefendFlag();
		case Objective::DEFEND_GATE:
			return new CTFBotMvMDefendGate();
		case Objective::ATTACK_TANK:
			return new CTFBotMvMAttackTank();
		}
		
		assert(false);
		return nullptr;
	}
	
	
	Objective CTFBotMvMDefender::DecideBestObjective()
	{
		if (this->IsFlagDangerouslyClose()) {
			return Objective::DEFEND_FLAG;
		}
		
		if (this->IsGateBeingAttacked()) {
			return Objective::DEFEND_GATE;
		}
		
		if (this->ShouldAttackTank()) {
			return Objective::ATTACK_TANK;
		}
		
		return Objective::DEFEND_FLAG;
	}
	
	
	bool CTFBotMvMDefender::IsFlagDangerouslyClose()
	{
		// TODO
		return false;
	}
	
	bool CTFBotMvMDefender::IsGateBeingAttacked()
	{
		for (const auto& master : g_hControlPointMasters) {
			// if not active, continue
			
			// loop over control points in master
				// if not active, continue
				// if !HasBeenContested, continue
				// if LastContestedAt is more recent than the last time we checked
		}
		
		// TODO
		return false;
	}
	
	bool CTFBotMvMDefender::ShouldAttackTank()
	{
		// TODO: check that there IS a tank at all, return false if not
		
		switch (this->GetActor()->GetPlayerClass()->GetClassIndex()) {
		case TF_CLASS_SCOUT:        return true;
		case TF_CLASS_SNIPER:       return false;
		case TF_CLASS_SOLDIER:      return true;
		case TF_CLASS_DEMOMAN:      return true;
		case TF_CLASS_MEDIC:        return false;
		case TF_CLASS_HEAVYWEAPONS: return true;
		case TF_CLASS_PYRO:         return true;
		case TF_CLASS_SPY:          return false;
		case TF_CLASS_ENGINEER:     return false;
		}
		
		assert(false);
		return false;
	}*/
}
