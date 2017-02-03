#include "mod.h"
#include "stub/nav.h"
#include "stub/entities.h"
#include "stub/gamerules.h"
#include "stub/tf_objective_resource.h"
#include "util/scope.h"


namespace Mod_Credits_Spawn_AutoCollect
{
	DETOUR_DECL_MEMBER(void, CCurrencyPack_ComeToRest)
	{
		auto pack = reinterpret_cast<CCurrencyPack *>(this);
		
		DETOUR_MEMBER_CALL(CCurrencyPack_ComeToRest)();
		
		if (!pack->IsMarkedForDeletion() && !pack->IsBeingPulled() && !pack->IsDistributed()) {
			auto area = reinterpret_cast<CTFNavArea *>(TheNavMesh->GetNavArea(pack->GetAbsOrigin()));
			if (area != nullptr && area->HasTFAttributes(BLUE_SPAWN_ROOM)) {
				DevMsg("Non-distributed CCurrencyPack (#%d / $%d) landed in BLUE_SPAWN_ROOM area; distributing now.\n",
					ENTINDEX(pack), pack->GetAmount());
				
				pack->SetDistributed(true);
				
				TFGameRules()->DistributeCurrencyAmount(pack->GetAmount(), nullptr, true, false, false);
				TFObjectiveResource()->m_nMvMWorldMoney -= pack->GetAmount();
				
				StopParticleEffects(pack);
				DispatchParticleEffect("mvm_cash_embers_red", PATTACH_ABSORIGIN_FOLLOW, pack);
			}
		}
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Credits:Spawn_AutoCollect")
		{
			MOD_ADD_DETOUR_MEMBER(CCurrencyPack_ComeToRest, "CCurrencyPack::ComeToRest");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_credits_spawn_autocollect", "0", FCVAR_NOTIFY,
		"Mod: auto-collect credits that land in the bots' spawn area",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
