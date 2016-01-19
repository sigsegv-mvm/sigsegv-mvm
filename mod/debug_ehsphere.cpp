#include "mod.h"
#include "sm/detours.h"
#include "stub/stub.h"
#include "util/util.h"


static ConVar cvar_duration("sigsegv_debug_ehsphere_duration", "0.5", FCVAR_NOTIFY,
	"Debug: EH sphere draw duration");

static ConVar cvar_color_r("sigsegv_debug_ehsphere_color_r", "255", FCVAR_NOTIFY,
	"Debug: EH sphere color (red)");
static ConVar cvar_color_g("sigsegv_debug_ehsphere_color_g", "255", FCVAR_NOTIFY,
	"Debug: EH sphere color (green)");
static ConVar cvar_color_b("sigsegv_debug_ehsphere_color_b", "255", FCVAR_NOTIFY,
	"Debug: EH sphere color (blue)");
static ConVar cvar_color_a("sigsegv_debug_ehsphere_color_a", "0", FCVAR_NOTIFY,
	"Debug: EH sphere color (alpha)");


static RefCount rc_CTFSniperRifle_ExplosiveHeadShot;
DETOUR_DECL_MEMBER(void, CTFSniperRifle_ExplosiveHeadShot, CTFPlayer *player1, CTFPlayer *player2)
{
	SCOPED_INCREMENT(rc_CTFSniperRifle_ExplosiveHeadShot);
	DETOUR_MEMBER_CALL(CTFSniperRifle_ExplosiveHeadShot)(player1, player2);
}

DETOUR_DECL_STATIC(int, UTIL_EntitiesInSphere, const Vector& center, float radius, CFlaggedEntitiesEnum *pEnum)
{
	if (rc_CTFSniperRifle_ExplosiveHeadShot.NonZero()) {
		for (int i = 0; i < 8; ++i) {
			NDebugOverlay::Sphere(center, QAngle(0.0f, (float)i * 22.5f, 0.0f), radius,
				cvar_color_r.GetInt(),
				cvar_color_g.GetInt(),
				cvar_color_b.GetInt(),
				cvar_color_a.GetInt(),
				false,
				cvar_duration.GetFloat());
		}
	}
	
	return DETOUR_STATIC_CALL(UTIL_EntitiesInSphere)(center, radius, pEnum);
}


class CMod_Debug_EHSphere : public IMod
{
public:
	CMod_Debug_EHSphere() : IMod("Debug_EHSphere")
	{
		MOD_ADD_DETOUR_MEMBER(CTFSniperRifle, ExplosiveHeadShot);
		MOD_ADD_DETOUR_GLOBAL(UTIL_EntitiesInSphere);
	}
	
	void SetEnabled(bool enable)
	{
		this->ToggleAllDetours(enable);
	}
};
static CMod_Debug_EHSphere s_Mod;


static ConVar cvar_enable("sigsegv_debug_ehsphere_enable", "0", FCVAR_NOTIFY,
	"Debug: draw sphere used for sniper's explosive headshot upgrade",
	[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
		ConVarRef var(pConVar);
		s_Mod.SetEnabled(var.GetBool());
	});
	
