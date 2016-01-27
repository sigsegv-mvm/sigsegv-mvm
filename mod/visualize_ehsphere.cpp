#include "mod.h"
#include "sm/detours.h"
#include "stub/stub.h"
#include "util/util.h"


#if defined __GNUC__
#warning TODO: move cvars into class
#endif


#ifdef EXPERIMENTAL

namespace Mod_Visualize_EHSphere
{
	ConVar cvar_duration("sigsegv_visualize_ehsphere_duration", "0.5", FCVAR_NOTIFY,
		"Visualization: EH sphere draw duration");
	
	ConVar cvar_color_r("sigsegv_visualize_ehsphere_color_r", "255", FCVAR_NOTIFY,
		"Visualization: EH sphere color (red)");
	ConVar cvar_color_g("sigsegv_visualize_ehsphere_color_g", "255", FCVAR_NOTIFY,
		"Visualization: EH sphere color (green)");
	ConVar cvar_color_b("sigsegv_visualize_ehsphere_color_b", "255", FCVAR_NOTIFY,
		"Visualization: EH sphere color (blue)");
	ConVar cvar_color_a("sigsegv_visualize_ehsphere_color_a", "0", FCVAR_NOTIFY,
		"Visualization: EH sphere color (alpha)");
	
	
	RefCount rc_CTFSniperRifle_ExplosiveHeadShot;
	DETOUR_DECL_MEMBER(void, CTFSniperRifle_ExplosiveHeadShot, CTFPlayer *player1, CTFPlayer *player2)
	{
		SCOPED_INCREMENT(rc_CTFSniperRifle_ExplosiveHeadShot);
		DETOUR_MEMBER_CALL(CTFSniperRifle_ExplosiveHeadShot)(player1, player2);
	}
	
	/* UTIL_EntitiesInSphere forwards call to partition->EnumerateElementsInSphere */
	RefCount rc_EnumerateElementsInSphere;
	DETOUR_DECL_MEMBER(void, ISpatialPartition_EnumerateElementsInSphere, SpatialPartitionListMask_t listMask, const Vector& origin, float radius, bool coarseTest, IPartitionEnumerator *pIterator)
	{
		SCOPED_INCREMENT(rc_EnumerateElementsInSphere);
		if (rc_CTFSniperRifle_ExplosiveHeadShot > 0 && rc_EnumerateElementsInSphere <= 1) {
			for (int i = 0; i < 8; ++i) {
				NDebugOverlay::Sphere(origin, QAngle(0.0f, (float)i * 22.5f, 0.0f), radius,
					cvar_color_r.GetInt(),
					cvar_color_g.GetInt(),
					cvar_color_b.GetInt(),
					cvar_color_a.GetInt(),
					false,
					cvar_duration.GetFloat());
			}
		}
		
		return DETOUR_MEMBER_CALL(ISpatialPartition_EnumerateElementsInSphere)(listMask, origin, radius, coarseTest, pIterator);
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Visualize_EHSphere")
		{
			MOD_ADD_DETOUR_MEMBER(CTFSniperRifle,    ExplosiveHeadShot);
			MOD_ADD_DETOUR_MEMBER(ISpatialPartition, EnumerateElementsInSphere);
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sigsegv_visualize_ehsphere_enable", "0", FCVAR_NOTIFY,
		"Visualization: draw sphere used for sniper's explosive headshot upgrade",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
	
#endif
