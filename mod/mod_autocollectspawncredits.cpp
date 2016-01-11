#include "mod.h"
#include "sdk/detours.h"
#include "stub/stub.h"
#include "util/util.h"
#include "re/nextbot.h"


static RefCount s_rcCCurrencyPack_ComeToRest;
DETOUR_DECL_MEMBER(void, CCurrencyPack_ComeToRest, void)
{
	SCOPED_INCREMENT(s_rcCCurrencyPack_ComeToRest);
	return DETOUR_MEMBER_CALL(CCurrencyPack_ComeToRest)();
}

DETOUR_DECL_MEMBER(CNavArea *, CNavMesh_GetNavArea, const Vector& v1, float f1)
{
	CNavArea *area = DETOUR_MEMBER_CALL(CNavMesh_GetNavArea)(v1, f1);
	
	if (area != nullptr && s_rcCCurrencyPack_ComeToRest.NonZero()) {
		TFNavAttributeType attr = reinterpret_cast<CTFNavArea *>(area)->GetTFAttributes();
		
		if ((attr & BLUE_SPAWN_ROOM) != 0) {
			DevMsg("CCurrencyPack landed in BLUE_SPAWN_ROOM area; auto-collecting");
			return nullptr;
		}
	}
	
	return area;
}


class CMod_AutoCollectSpawnCredits : public IMod
{
public:
	CMod_AutoCollectSpawnCredits() : IMod("AutoCollectSpawnCredits")
	{
		MOD_ADD_DETOUR_MEMBER(CCurrencyPack, ComeToRest);
		MOD_ADD_DETOUR_MEMBER(CNavMesh, GetNavArea);
	}
	
	void SetEnabled(bool enable)
	{
		this->ToggleAllDetours(enable);
	}
};
static CMod_AutoCollectSpawnCredits s_Mod;


static ConVar cvar_enable("sigsegv_autocollectspawncredits_enable", "0", FCVAR_NOTIFY,
	"Mod: auto-collect credits that land in the bots' spawn area",
	[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
		ConVarRef var(pConVar);
		s_Mod.SetEnabled(var.GetBool());
	});
