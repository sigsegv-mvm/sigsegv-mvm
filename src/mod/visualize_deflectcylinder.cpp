#include "mod.h"
#include "sm/detours.h"
#include "stub/stub.h"
#include "util/util.h"


#if defined __GNUC__
#warning TODO: move cvars into class
#endif


#ifdef EXPERIMENTAL

namespace Mod_Visualize_DeflectCylinder
{
	ConVar cvar_duration("sigsegv_visualize_deflectcylinder_duration", "0.1", FCVAR_NOTIFY,
		"Visualization: cylinder draw duration");
	
	ConVar cvar_color_r("sigsegv_visualize_deflectcylinder_color_r", "255", FCVAR_NOTIFY,
		"Visualization: cylinder color (red)");
	ConVar cvar_color_g("sigsegv_visualize_deflectcylinder_color_g", "255", FCVAR_NOTIFY,
		"Visualization: cylinder color (green)");
	ConVar cvar_color_b("sigsegv_visualize_deflectcylinder_color_b", "255", FCVAR_NOTIFY,
		"Visualization: cylinder color (blue)");
	ConVar cvar_color_a("sigsegv_visualize_deflectcylinder_color_a", "0", FCVAR_NOTIFY,
		"Visualization: cylinder color (alpha)");
	
	ConVar cvar_sphere_enable("sigsegv_visualize_deflectcylinder_sphere_enable", "0", FCVAR_NOTIFY,
		"Visualization: sphere enable");
	ConVar cvar_sphere_color_r("sigsegv_visualize_deflectcylinder_sphere_color_r", "255", FCVAR_NOTIFY,
		"Visualization: sphere color (red)");
	ConVar cvar_sphere_color_g("sigsegv_visualize_deflectcylinder_sphere_color_g", "255", FCVAR_NOTIFY,
		"Visualization: sphere color (green)");
	ConVar cvar_sphere_color_b("sigsegv_visualize_deflectcylinder_sphere_color_b", "255", FCVAR_NOTIFY,
		"Visualization: sphere color (blue)");
	ConVar cvar_sphere_color_a("sigsegv_visualize_deflectcylinder_sphere_color_a", "0", FCVAR_NOTIFY,
		"Visualization: sphere color (alpha)");
	
	
	RefCount rc_CTFMinigun_AttackProjectiles;
	DETOUR_DECL_MEMBER(void, CTFMinigun_AttackEnemyProjectiles)
	{
		SCOPED_INCREMENT(rc_CTFMinigun_AttackProjectiles);
		DETOUR_MEMBER_CALL(CTFMinigun_AttackEnemyProjectiles)();
	}
	
	bool is_MiniBoss;
	DETOUR_DECL_MEMBER(bool, CTFPlayer_IsMiniBoss)
	{
		bool result = DETOUR_MEMBER_CALL(CTFPlayer_IsMiniBoss)();
		is_MiniBoss = result;
		return result;
	}
	
	/* UTIL_EntitiesInSphere forwards call to partition->EnumerateElementsInSphere */
	RefCount rc_EnumerateElementsInSphere;
	DETOUR_DECL_MEMBER(void, ISpatialPartition_EnumerateElementsInSphere, SpatialPartitionListMask_t listMask, const Vector& origin, float radius, bool coarseTest, IPartitionEnumerator *pIterator)
	{
		SCOPED_INCREMENT(rc_EnumerateElementsInSphere);
		if (rc_CTFMinigun_AttackProjectiles > 0 && rc_EnumerateElementsInSphere <= 1 && cvar_sphere_enable.GetBool()) {
			NDebugOverlay::Sphere(origin, QAngle(0.0f, 0.0f, 0.0f), radius,
				cvar_sphere_color_r.GetInt(),
				cvar_sphere_color_g.GetInt(),
				cvar_sphere_color_b.GetInt(),
				cvar_sphere_color_a.GetInt(),
				false,
				cvar_duration.GetFloat());
			
			NDebugOverlay::Sphere(origin, QAngle(0.0f, 30.0f, 0.0f), radius,
				cvar_sphere_color_r.GetInt(),
				cvar_sphere_color_g.GetInt(),
				cvar_sphere_color_b.GetInt(),
				cvar_sphere_color_a.GetInt(),
				false,
				cvar_duration.GetFloat());
			
			NDebugOverlay::Sphere(origin, QAngle(0.0f, 60.0f, 0.0f), radius,
				cvar_sphere_color_r.GetInt(),
				cvar_sphere_color_g.GetInt(),
				cvar_sphere_color_b.GetInt(),
				cvar_sphere_color_a.GetInt(),
				false,
				cvar_duration.GetFloat());
		}
		
		return DETOUR_MEMBER_CALL(ISpatialPartition_EnumerateElementsInSphere)(listMask, origin, radius, coarseTest, pIterator);
	}
	
	DETOUR_DECL_STATIC(float, CalcDistanceToLineSegment, const Vector& P, const Vector& vLineA, const Vector& vLineB, float *t)
	{
		float dist = DETOUR_STATIC_CALL(CalcDistanceToLineSegment)(P, vLineA, vLineB, t);
		
		if (rc_CTFMinigun_AttackProjectiles > 0) {
			float radius = (is_MiniBoss ? 56.0f : 38.0f);
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
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Visualize_DeflectCylinder")
		{
			MOD_ADD_DETOUR_MEMBER(CTFMinigun,        AttackEnemyProjectiles);
			MOD_ADD_DETOUR_MEMBER(CTFPlayer,         IsMiniBoss);
			MOD_ADD_DETOUR_MEMBER(ISpatialPartition, EnumerateElementsInSphere);
			MOD_ADD_DETOUR_GLOBAL(CalcDistanceToLineSegment);
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sigsegv_visualize_deflectcylinder_enable", "0", FCVAR_NOTIFY,
		"Visualization: draw cylinder used for heavy's projectile deflection upgrade",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
	
#endif
