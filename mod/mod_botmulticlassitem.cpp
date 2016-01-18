#include "mod.h"
//#include "sm/detours.h"
//#include "stub/stub.h"
//#include "util/util.h"
//#include "re/nextbot.h"


#if 0
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
			DevMsg("CCurrencyPack landed in BLUE_SPAWN_ROOM area; auto-collecting\n");
			return nullptr;
		}
	}
	
	return area;
}
#endif

static RefCount s_rcCTFBot_AddItem;
DETOUR_DECL_MEMBER(void, CTFBot_AddItem, const char *item)
{
	SCOPED_INCREMENT(s_rcCTFBot_AddItem);
	DETOUR_MEMBER_CALL(CTFBot_AddItem)(item);
}

DETOUR_DECL_STATIC(CBaseEntity *, CreateEntityByName, const char *className, int iForceEdictIndex)
{
	// TODO
	
	return DETOUR_STATIC_CALL(CreateEntityByName)(className, iForceEdictIndex);
}


// void CTFBot::AddItem(const char *name)
// - CBaseEntity *CItemGeneration::GenerateRandomItem(CItemSelectionCriteria *criteria, const Vector& pos, const QAngle& ang)
//   - CBaseEntity *CItemGeneration::SpawnItem(int itemdefidx, const Vector& pos, const QAngle& ang, int, int, const char *class_or_null)
//     - CBaseEntity *CreateEntityByName(const char *className, int iForceEdictIndex)

// const char *TranslateWeaponEntForClass(const char *name, int classnum)



class CMod_BotMultiClassItem : public IMod
{
public:
	CMod_BotMultiClassItem() : IMod("BotMultiClassItem")
	{
		//MOD_ADD_DETOUR_MEMBER(CCurrencyPack, ComeToRest);
		//MOD_ADD_DETOUR_MEMBER(CNavMesh, GetNavArea);
	}
	
	void SetEnabled(bool enable)
	{
		//this->ToggleAllDetours(enable);
	}
};
static CMod_BotMultiClassItem s_Mod;


static ConVar cvar_enable("sigsegv_botmulticlassitem_enable", "0", FCVAR_NOTIFY,
	"Mod: remap item entity names so bots can be given multi-class weapons",
	[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
		ConVarRef var(pConVar);
		s_Mod.SetEnabled(var.GetBool());
	});
