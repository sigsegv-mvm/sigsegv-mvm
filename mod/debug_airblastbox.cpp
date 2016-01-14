#include "mod.h"
#include "sm/detours.h"
#include "stub/stub.h"
#include "util/util.h"


static ConVar cvar_duration("sigsegv_debug_airblastbox_duration", "0.1", FCVAR_NOTIFY,
	"Debug: box draw duration");

static ConVar cvar_color_r("sigsegv_debug_airblastbox_color_r", "255", FCVAR_NOTIFY,
	"Debug: box color (red)");
static ConVar cvar_color_g("sigsegv_debug_airblastbox_color_g", "255", FCVAR_NOTIFY,
	"Debug: box color (green)");
static ConVar cvar_color_b("sigsegv_debug_airblastbox_color_b", "255", FCVAR_NOTIFY,
	"Debug: box color (blue)");
static ConVar cvar_color_a("sigsegv_debug_airblastbox_color_a", "255", FCVAR_NOTIFY,
	"Debug: box color (alpha)");


static RefCount s_rcDeflectProjectiles;
DETOUR_DECL_MEMBER(bool, CTFWeaponBase_DeflectProjectiles, void)
{
	SCOPED_INCREMENT(s_rcDeflectProjectiles);
	return DETOUR_MEMBER_CALL(CTFWeaponBase_DeflectProjectiles)();
}

DETOUR_DECL_STATIC(int, UTIL_EntitiesInBox, const Vector& mins, const Vector& maxs, CFlaggedEntitiesEnum *pEnum)
{
	if (s_rcDeflectProjectiles.NonZero()) {
		NDebugOverlay::Box(vec3_origin, mins, maxs,
			cvar_color_r.GetInt(),
			cvar_color_g.GetInt(),
			cvar_color_b.GetInt(),
			cvar_color_a.GetInt(),
			cvar_duration.GetFloat());
	}
	
	return DETOUR_STATIC_CALL(UTIL_EntitiesInBox)(mins, maxs, pEnum);
}


class CMod_Debug_AirblastBox : public IMod
{
public:
	CMod_Debug_AirblastBox() : IMod("Debug_AirblastBox")
	{
		MOD_ADD_DETOUR_MEMBER(CTFWeaponBase, DeflectProjectiles);
		MOD_ADD_DETOUR_GLOBAL(UTIL_EntitiesInBox);
	}
	
	void SetEnabled(bool enable)
	{
		this->ToggleAllDetours(enable);
	}
};
static CMod_Debug_AirblastBox s_Mod;


static ConVar cvar_enable("sigsegv_debug_airblastbox_enable", "0", FCVAR_NOTIFY,
	"Debug: draw box used for airblast deflection of projectiles",
	[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
		ConVarRef var(pConVar);
		s_Mod.SetEnabled(var.GetBool());
	});
