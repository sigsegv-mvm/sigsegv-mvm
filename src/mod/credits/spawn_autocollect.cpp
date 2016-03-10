#include "mod.h"
#include "re/nextbot.h"
#include "util/scope.h"


namespace Mod_Credits_Spawn_AutoCollect
{
	RefCount rc_CCurrencyPack_ComeToRest;
	DETOUR_DECL_MEMBER(void, CCurrencyPack_ComeToRest)
	{
		SCOPED_INCREMENT(rc_CCurrencyPack_ComeToRest);
		return DETOUR_MEMBER_CALL(CCurrencyPack_ComeToRest)();
	}
	
	DETOUR_DECL_MEMBER(CNavArea *, CNavMesh_GetNavArea, const Vector& v1, float f1)
	{
		CNavArea *area = DETOUR_MEMBER_CALL(CNavMesh_GetNavArea)(v1, f1);
		
		if (area != nullptr && rc_CCurrencyPack_ComeToRest > 0) {
			TFNavAttributeType attr = reinterpret_cast<CTFNavArea *>(area)->GetTFAttributes();
			
			if ((attr & BLUE_SPAWN_ROOM) != 0) {
	//			DevMsg("CCurrencyPack landed in BLUE_SPAWN_ROOM area; auto-collecting\n");
				return nullptr;
			}
		}
		
		return area;
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Credits:Spawn_AutoCollect")
		{
			MOD_ADD_DETOUR_MEMBER(CCurrencyPack_ComeToRest, "CCurrencyPack::ComeToRest");
			MOD_ADD_DETOUR_MEMBER(CNavMesh_GetNavArea,      "CNavMesh::GetNavArea");
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_credits_spawn_autocollect", "0", FCVAR_NOTIFY,
		"Mod: auto-collect credits that land in the bots' spawn area",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
