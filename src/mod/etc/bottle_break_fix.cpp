#include "mod.h"
#include "stub/tfweaponbase.h"


namespace Mod_Etc_Bottle_Break_Fix
{
	// TODO: test with pyrovision to see whether we need to set overrides 1/2/3 for the worldmodel
	
	
	DETOUR_DECL_MEMBER(void, CTFBottle_SwitchBodyGroups)
	{
		auto bottle = reinterpret_cast<CTFBottle *>(this);
		
		bottle->SetModel("models/weapons/w_models/w_bottle.mdl");
		
		int idx_w = modelinfo->GetModelIndex("models/weapons/w_models/w_bottle.mdl");
		bottle->SetModelIndexOverride(VISION_MODE_NONE, idx_w);
		
		auto pVM = UTIL_PlayerByIndex(1)->GetViewModel(0);
		if (pVM != nullptr) {
		//	int idx_v = modelinfo->GetModelIndex("models/weapons/v_models/v_bottle.mdl");
		//	pVM->SetModelIndexOverride(VISION_MODE_NONE, idx_v);
			
			pVM->SetModel("models/weapons/v_models/v_bottle.mdl");
		}
		
		if (bottle->m_bBroken) {
			bottle->SetBodygroup(0, 1);
		//	bottle->SetModel("models/weapons/c_models/c_bottle/c_bottle.mdl");
		//	bottle->SetModel("models/weapons/c_models/c_bottle/c_bottle_broken.mdl");
		//	int idx = modelinfo->GetModelIndex("models/weapons/c_models/c_bottle/c_bottle_broken.mdl");
		//	bottle->SetModelIndexOverride(VISION_MODE_NONE, idx);
		} else {
			bottle->SetBodygroup(0, 0);
		//	bottle->SetModel("models/weapons/c_models/c_bottle/c_bottle.mdl");
		//	int idx = modelinfo->GetModelIndex("models/weapons/c_models/c_bottle/c_bottle.mdl");
		//	bottle->SetModelIndexOverride(VISION_MODE_NONE, idx);
		}
		
	//	bottle->SetViewModel();
	}
	
//	DETOUR_DECL_MEMBER(const char *, CTFWeaponBase_GetViewModel, int viewmodelindex)
//	{
//		auto weapon = reinterpret_cast<CTFWeaponBase *>(this);
//		
//		if (weapon->ClassMatches("tf_weapon_bottle") && viewmodelindex == 0) {
//			return "models/weapons/v_models/v_bottle.mdl";
//		}
//		
//		return DETOUR_MEMBER_CALL(CTFWeaponBase_GetViewModel)(viewmodelindex);
//	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Etc:Bottle_Break_Fix")
		{
			MOD_ADD_DETOUR_MEMBER(CTFBottle_SwitchBodyGroups, "CTFBottle::SwitchBodyGroups");
		//	MOD_ADD_DETOUR_MEMBER(CTFWeaponBase_GetViewModel, "CTFWeaponBase::GetViewModel");
			
			
			// void __cdecl CTFBottle::SwitchBodyGroups(CTFBottle *this)
			// const char *__cdecl CTFWeaponBase::GetWorldModel(CTFWeaponBase *this)
			// 
			
			
			// override GetWorldModel      to work like (client) CTFStickBomb::GetWorldModel
			// override GetWorldModelIndex to work like (client) CTFStickBomb::GetWorldModelIndex
			// override SwitchBodyGroups   to work like (client) CTFStickBomb::SwitchBodyGroups
		}
		
		virtual bool OnLoad() override
		{
			CBaseEntity::PrecacheModel("models/weapons/w_models/w_bottle.mdl");
			CBaseEntity::PrecacheModel("models/weapons/v_models/v_bottle.mdl");
			CBaseEntity::PrecacheModel("models/weapons/c_models/c_bottle/c_bottle.mdl");
			CBaseEntity::PrecacheModel("models/weapons/c_models/c_bottle/c_bottle_broken.mdl");
			
			return true;
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_etc_bottle_break_fix", "0", FCVAR_NOTIFY,
		"Mod: fix post-Love-and-War-Update demoman bottle break-on-crit breakage",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
