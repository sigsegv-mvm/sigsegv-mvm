#include "mod.h"
#include "sm/detours.h"
#include "stub/stub.h"
#include "util/util.h"


static ConVar cvar_duration("sigsegv_debug_deflectcylinder_duration", "0.1", FCVAR_NOTIFY,
	"Debug: cylinder draw duration");

static ConVar cvar_color_r("sigsegv_debug_deflectcylinder_color_r", "255", FCVAR_NOTIFY,
	"Debug: cylinder color (red)");
static ConVar cvar_color_g("sigsegv_debug_deflectcylinder_color_g", "255", FCVAR_NOTIFY,
	"Debug: cylinder color (green)");
static ConVar cvar_color_b("sigsegv_debug_deflectcylinder_color_b", "255", FCVAR_NOTIFY,
	"Debug: cylinder color (blue)");
static ConVar cvar_color_a("sigsegv_debug_deflectcylinder_color_a", "0", FCVAR_NOTIFY,
	"Debug: cylinder color (alpha)");

static ConVar cvar_sphere_enable("sigsegv_debug_deflectcylinder_sphere_enable", "0", FCVAR_NOTIFY,
	"Debug: sphere enable");
static ConVar cvar_sphere_color_r("sigsegv_debug_deflectcylinder_sphere_color_r", "255", FCVAR_NOTIFY,
	"Debug: sphere color (red)");
static ConVar cvar_sphere_color_g("sigsegv_debug_deflectcylinder_sphere_color_g", "255", FCVAR_NOTIFY,
	"Debug: sphere color (green)");
static ConVar cvar_sphere_color_b("sigsegv_debug_deflectcylinder_sphere_color_b", "255", FCVAR_NOTIFY,
	"Debug: sphere color (blue)");
static ConVar cvar_sphere_color_a("sigsegv_debug_deflectcylinder_sphere_color_a", "0", FCVAR_NOTIFY,
	"Debug: sphere color (alpha)");


static RefCount s_rcAttackEnemyProjectiles;
DETOUR_DECL_MEMBER(void, CTFMinigun_AttackEnemyProjectiles, void)
{
	SCOPED_INCREMENT(s_rcAttackEnemyProjectiles);
	DETOUR_MEMBER_CALL(CTFMinigun_AttackEnemyProjectiles)();
}

static bool s_IsMiniBoss;
DETOUR_DECL_MEMBER(bool, CTFPlayer_IsMiniBoss, void)
{
	bool result = DETOUR_MEMBER_CALL(CTFPlayer_IsMiniBoss)();
	s_IsMiniBoss = result;
	return result;
}

DETOUR_DECL_STATIC(int, UTIL_EntitiesInSphere, const Vector& center, float radius, CFlaggedEntitiesEnum *pEnum)
{
	if (s_rcAttackEnemyProjectiles.NonZero() && cvar_sphere_enable.GetBool()) {
		NDebugOverlay::Sphere(center, QAngle(0.0f, 0.0f, 0.0f), radius,
			cvar_sphere_color_r.GetInt(),
			cvar_sphere_color_g.GetInt(),
			cvar_sphere_color_b.GetInt(),
			cvar_sphere_color_a.GetInt(),
			false,
			cvar_duration.GetFloat());
		
		NDebugOverlay::Sphere(center, QAngle(0.0f, 30.0f, 0.0f), radius,
			cvar_sphere_color_r.GetInt(),
			cvar_sphere_color_g.GetInt(),
			cvar_sphere_color_b.GetInt(),
			cvar_sphere_color_a.GetInt(),
			false,
			cvar_duration.GetFloat());
		
		NDebugOverlay::Sphere(center, QAngle(0.0f, 60.0f, 0.0f), radius,
			cvar_sphere_color_r.GetInt(),
			cvar_sphere_color_g.GetInt(),
			cvar_sphere_color_b.GetInt(),
			cvar_sphere_color_a.GetInt(),
			false,
			cvar_duration.GetFloat());
	}
	
	return DETOUR_STATIC_CALL(UTIL_EntitiesInSphere)(center, radius, pEnum);
}

DETOUR_DECL_STATIC(float, CalcDistanceToLineSegment, const Vector& P, const Vector& vLineA, const Vector& vLineB, float *t)
{
	float dist = DETOUR_STATIC_CALL(CalcDistanceToLineSegment)(P, vLineA, vLineB, t);
	
	if (s_rcAttackEnemyProjectiles.NonZero()) {
		float radius = (s_IsMiniBoss ? 56.0f : 38.0f);
		bool is_inside = (dist <= radius);
		
		NDebugOverlay::Cross3D(P, 10.0f,
			(is_inside ? 0 : 255),
			(is_inside ? 255 : 0),
			0,
			true,
			cvar_duration.GetFloat());
		
		static int lastframe = -1;
		if (gpGlobals->framecount != lastframe) {
			lastframe = gpGlobals->framecount;
			
			/*NDebugOverlay::Line(vLineA, vLineB,
				cvar_color_r.GetFloat(),
				cvar_color_g.GetFloat(),
				cvar_color_b.GetFloat(),
				true,
				cvar_duration.GetFloat());*/
			
			QAngle ang;
			VectorAngles(vLineA - vLineB, ang);
			
			for (int i = 0; i <= 10; ++i) {
				float pA = ((float)i / 10.0f);
				float pB = (1.0f - pA);
				
				NDebugOverlay::Circle((pA * vLineA) + (pB * vLineB), ang, radius,
					cvar_color_r.GetFloat(),
					cvar_color_g.GetFloat(),
					cvar_color_b.GetFloat(),
					cvar_color_a.GetFloat(),
					false,
					cvar_duration.GetFloat());
			}
		}
	}
	
	return dist;
}


class CMod_Debug_DeflectCylinder : public IMod
{
public:
	CMod_Debug_DeflectCylinder() : IMod("Debug_DeflectCylinder")
	{
		MOD_ADD_DETOUR_MEMBER(CTFMinigun, AttackEnemyProjectiles);
		MOD_ADD_DETOUR_MEMBER(CTFPlayer, IsMiniBoss);
		MOD_ADD_DETOUR_GLOBAL(UTIL_EntitiesInSphere);
		MOD_ADD_DETOUR_GLOBAL(CalcDistanceToLineSegment);
	}
	
	void SetEnabled(bool enable)
	{
		this->ToggleAllDetours(enable);
	}
};
static CMod_Debug_DeflectCylinder s_Mod;


static ConVar cvar_enable("sigsegv_debug_deflectcylinder_enable", "0", FCVAR_NOTIFY,
	"Debug: draw cylinder used for heavy's projectile deflection upgrade",
	[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
		ConVarRef var(pConVar);
		s_Mod.SetEnabled(var.GetBool());
	});
	
