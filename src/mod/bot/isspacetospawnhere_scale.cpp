#include "mod.h"
#include "util/scope.h"
#include "stub/populators.h"
#include "stub/gamerules.h"


namespace Mod_Bot_IsSpaceToSpawnHere_Scale
{
	/* real implementation in 20151007a */
#if 0
	bool IsSpaceToSpawnHere(const Vector& pos)
	{
		Vector vecMins = VEC_HULL_MIN + Vector(-5.0f, -5.0f, 0.0f);
		Vector vecMaxs = VEC_HULL_MAX + Vector( 5.0f,  5.0f, 5.0f);
		
		trace_t tr;
		UTIL_TraceHull(pos, pos, vecMins, vecMaxs, MASK_PLAYERSOLID, nullptr, COLLISION_GROUP_PLAYER_MOVEMENT, &tr);
		
		static ConVarRef tf_debug_placement_failure("tf_debug_placement_failure");
		if (tf_debug_placement_failure.GetBool() && tr.fraction < 1.0f) {
			NDebugOverlay::Cross3D(pos, 5.0f, 0xff, 0x64, 0x00, true, 1.0e+5f);
		}
		
		return (tr.fraction >= 1.0f);
	}
#endif
	
	// 1. take model scale into account
	// 2. do a sanity check here... are they doing the trace properly
	// 3. take this example and build a proof-of-concept for an engiebot push force fix!
	
	
	ConVar cvar_debug("sig_bot_isspacetospawnhere_scale_debug", "0", FCVAR_NOTIFY,
		"Mod: Spew debug information when doing spawn trace");
	
	
	CTFBotSpawner *the_spawner = nullptr;
	float the_bot_scale = 1.0f;
	RefCount rc_CTFBotSpawner_Spawn;
	DETOUR_DECL_MEMBER(int, CTFBotSpawner_Spawn, const Vector& where, CUtlVector<CHandle<CBaseEntity>> *ents)
	{
		static ConVarRef tf_mvm_miniboss_scale("tf_mvm_miniboss_scale");
		
		auto spawner = reinterpret_cast<CTFBotSpawner *>(this);
		
		the_spawner   = spawner;
		the_bot_scale = (spawner->IsMiniBoss(-1) ? tf_mvm_miniboss_scale.GetFloat() : 1.0f);
		if (spawner->m_flScale != -1.0f) the_bot_scale = spawner->m_flScale;
		
		SCOPED_INCREMENT_IF(rc_CTFBotSpawner_Spawn, the_bot_scale != 1.0f);
		
		return DETOUR_MEMBER_CALL(CTFBotSpawner_Spawn)(where, ents);
	}
	
	
	CTFBot *the_spy = nullptr;
	float the_spy_scale = 1.0f;
	RefCount rc_TeleportNearVictim;
	DETOUR_DECL_STATIC(bool, TeleportNearVictim, CTFBot *spy, CTFPlayer *victim, int dist)
	{
		the_spy       = spy;
		the_spy_scale = spy->GetModelScale();
		
		SCOPED_INCREMENT_IF(rc_TeleportNearVictim, the_spy_scale != 1.0f);
		return DETOUR_STATIC_CALL(TeleportNearVictim)(spy, victim, dist);
	}
	
	
	const Vector *the_pos;
	RefCount rc_IsSpaceToSpawnHere;
	DETOUR_DECL_STATIC(bool, IsSpaceToSpawnHere, const Vector& pos)
	{
		SCOPED_INCREMENT(rc_IsSpaceToSpawnHere);
		the_pos = &pos;
		
		if (cvar_debug.GetBool()) DevMsg("\nIsSpaceToSpawnHere BEGIN @ %.7f\n", Plat_FloatTime());
		auto result = DETOUR_STATIC_CALL(IsSpaceToSpawnHere)(pos);
		if (cvar_debug.GetBool()) DevMsg("IsSpaceToSpawnHere END   @ %.7f\n\n", Plat_FloatTime());
		return result;
	}
	
	
	DETOUR_DECL_MEMBER(void, IEngineTrace_TraceRay, const Ray_t& ray, unsigned int fMask, ITraceFilter *pTraceFilter, trace_t *pTrace)
	{
		if (rc_IsSpaceToSpawnHere > 0 && the_pos != nullptr) {
			if (rc_CTFBotSpawner_Spawn > 0 && the_spawner != nullptr) {
				Vector vecMins = (VEC_HULL_MIN * the_bot_scale) + Vector(-5.0f, -5.0f, 0.0f);
				Vector vecMaxs = (VEC_HULL_MAX * the_bot_scale) + Vector( 5.0f,  5.0f, 5.0f);
				
				Ray_t ray_alt;
				ray_alt.Init(*the_pos, *the_pos, vecMins, vecMaxs);
				
				if (cvar_debug.GetBool()) {
					DevMsg("IsSpaceToSpawnHere via CTFBotSpawner::Spawn\n");
					DevMsg("  from spawner @ 0x%08x with icon \"%s\" and scale %f\n", (uintptr_t)the_spawner, STRING(the_spawner->m_strClassIcon), the_spawner->m_flScale);
					DevMsg("    and the_pos [ %+6.1f %+6.1f %+6.1f ]\n", the_pos->x, the_pos->y, the_pos->z);
					
					DevMsg("  fixed-up mins and maxs:\n");
					DevMsg("    vecMins: [ %+6.1f %6.1f %6.1f ]\n", vecMins.x, vecMins.y, vecMins.z);
					DevMsg("    vecMaxs: [ %+6.1f %6.1f %6.1f ]\n", vecMaxs.x, vecMaxs.y, vecMaxs.z);
					
					DevMsg("  would have used this ray:\n");
					DevMsg("    m_Start:       [ %+6.1f %6.1f %6.1f ]\n", ray.m_Start.x,       ray.m_Start.y,       ray.m_Start.z);
					DevMsg("    m_Delta:       [ %+6.1f %6.1f %6.1f ]\n", ray.m_Delta.x,       ray.m_Delta.y,       ray.m_Delta.z);
					DevMsg("    m_StartOffset: [ %+6.1f %6.1f %6.1f ]\n", ray.m_StartOffset.x, ray.m_StartOffset.y, ray.m_StartOffset.z);
					DevMsg("    m_Extents:     [ %+6.1f %6.1f %6.1f ]\n", ray.m_Extents.x,     ray.m_Extents.y,     ray.m_Extents.z);
					
					DevMsg("  actually using this ray:\n");
					DevMsg("    m_Start:       [ %+6.1f %6.1f %6.1f ]\n", ray_alt.m_Start.x,       ray_alt.m_Start.y,       ray_alt.m_Start.z);
					DevMsg("    m_Delta:       [ %+6.1f %6.1f %6.1f ]\n", ray_alt.m_Delta.x,       ray_alt.m_Delta.y,       ray_alt.m_Delta.z);
					DevMsg("    m_StartOffset: [ %+6.1f %6.1f %6.1f ]\n", ray_alt.m_StartOffset.x, ray_alt.m_StartOffset.y, ray_alt.m_StartOffset.z);
					DevMsg("    m_Extents:     [ %+6.1f %6.1f %6.1f ]\n", ray_alt.m_Extents.x,     ray_alt.m_Extents.y,     ray_alt.m_Extents.z);
					
					NDebugOverlay::Box(*the_pos, vecMins, vecMaxs, 0xff, 0xff, 0xff, 0x40, 10.0f);
				}
				
				DETOUR_MEMBER_CALL(IEngineTrace_TraceRay)(ray_alt, fMask, pTraceFilter, pTrace);
				return;
			}
			
			if (rc_TeleportNearVictim > 0 && the_spy != nullptr) {
				Vector vecMins = (VEC_HULL_MIN * the_spy_scale) + Vector(-5.0f, -5.0f, 0.0f);
				Vector vecMaxs = (VEC_HULL_MAX * the_spy_scale) + Vector( 5.0f,  5.0f, 5.0f);
				
				Ray_t ray_alt;
				ray_alt.Init(*the_pos, *the_pos, vecMins, vecMaxs);
				
				if (cvar_debug.GetBool()) {
					DevMsg("\nIsSpaceToSpawnHere via TeleportNearVictim\n");
					DevMsg("  from spy #%d with name \"%s\" and scale %f\n", ENTINDEX(the_spy), the_spy->GetPlayerName(), the_spy->GetModelScale());
					DevMsg("    and the_pos [ %+6.1f %+6.1f %+6.1f ]\n", the_pos->x, the_pos->y, the_pos->z);
					
					DevMsg("  fixed-up mins and maxs:\n");
					DevMsg("    vecMins: [ %+6.1f %6.1f %6.1f ]\n", vecMins.x, vecMins.y, vecMins.z);
					DevMsg("    vecMaxs: [ %+6.1f %6.1f %6.1f ]\n", vecMaxs.x, vecMaxs.y, vecMaxs.z);
					
					DevMsg("  would have used this ray:\n");
					DevMsg("    m_Start:       [ %+6.1f %6.1f %6.1f ]\n", ray.m_Start.x,       ray.m_Start.y,       ray.m_Start.z);
					DevMsg("    m_Delta:       [ %+6.1f %6.1f %6.1f ]\n", ray.m_Delta.x,       ray.m_Delta.y,       ray.m_Delta.z);
					DevMsg("    m_StartOffset: [ %+6.1f %6.1f %6.1f ]\n", ray.m_StartOffset.x, ray.m_StartOffset.y, ray.m_StartOffset.z);
					DevMsg("    m_Extents:     [ %+6.1f %6.1f %6.1f ]\n", ray.m_Extents.x,     ray.m_Extents.y,     ray.m_Extents.z);
					
					DevMsg("  actually using this ray:\n");
					DevMsg("    m_Start:       [ %+6.1f %6.1f %6.1f ]\n", ray_alt.m_Start.x,       ray_alt.m_Start.y,       ray_alt.m_Start.z);
					DevMsg("    m_Delta:       [ %+6.1f %6.1f %6.1f ]\n", ray_alt.m_Delta.x,       ray_alt.m_Delta.y,       ray_alt.m_Delta.z);
					DevMsg("    m_StartOffset: [ %+6.1f %6.1f %6.1f ]\n", ray_alt.m_StartOffset.x, ray_alt.m_StartOffset.y, ray_alt.m_StartOffset.z);
					DevMsg("    m_Extents:     [ %+6.1f %6.1f %6.1f ]\n", ray_alt.m_Extents.x,     ray_alt.m_Extents.y,     ray_alt.m_Extents.z);
					
					NDebugOverlay::Box(*the_pos, vecMins, vecMaxs, 0xff, 0xff, 0xff, 0x40, 10.0f);
				}
				
				DETOUR_MEMBER_CALL(IEngineTrace_TraceRay)(ray_alt, fMask, pTraceFilter, pTrace);
				return;
			}
		}
		
		DETOUR_MEMBER_CALL(IEngineTrace_TraceRay)(ray, fMask, pTraceFilter, pTrace);
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Bot:IsSpaceToSpawnHere_Scale")
		{
			MOD_ADD_DETOUR_MEMBER(CTFBotSpawner_Spawn, "CTFBotSpawner::Spawn");
			
			MOD_ADD_DETOUR_STATIC(TeleportNearVictim, "TeleportNearVictim");
			
			MOD_ADD_DETOUR_STATIC(IsSpaceToSpawnHere, "IsSpaceToSpawnHere");
			
			MOD_ADD_DETOUR_MEMBER(IEngineTrace_TraceRay, "IEngineTrace::TraceRay");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_bot_isspacetospawnhere_scale", "0", FCVAR_NOTIFY,
		"Mod: make IsSpaceToSpawnHere take bots' model scale into account",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
