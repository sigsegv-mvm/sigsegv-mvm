#ifndef _INCLUDE_SIGSEGV_MOD_DEBUG_MELEE_TRACE_IMPROVED_H_
#define _INCLUDE_SIGSEGV_MOD_DEBUG_MELEE_TRACE_IMPROVED_H_


/* improved version with modifications by me */
namespace Improved
{
	class CTFWeaponBaseMeleeExt : public CTFWeaponBaseMelee
	{
	public:
		bool DoSwingTraceInternal(trace_t& trace, bool bCleaveAttack, CUtlVector<trace_t> *pTraces);
	};
	
	
	inline bool CTFWeaponBaseMeleeExt::DoSwingTraceInternal(trace_t& trace, bool bCleaveAttack, CUtlVector<trace_t> *pTraces)
	{
		static Vector vecSwingMins(-18.0f, -18.0f, -18.0f);
		static Vector vecSwingMaxs( 18.0f,  18.0f,  18.0f);
		
		float flMultBounds = 1.0f;
		CALL_ATTRIB_HOOK_FLOAT(flMultBounds, melee_bounds_multiplier);
		
		vecSwingMins *= flMultBounds;
		vecSwingMaxs *= flMultBounds;
		
		CTFPlayer *pPlayer = GetTFPlayerOwner();
		if (pPlayer == nullptr) return false;
		
		float flSwingRange = GetSwingRange();
		
		float flModelScale = pPlayer->GetModelScale();
		if (flModelScale > 1.0f) {
			flSwingRange *= flModelScale;
			vecSwingMins *= flModelScale;
			vecSwingMaxs *= flModelScale;
		}
		
		CALL_ATTRIB_HOOK_FLOAT(flSwingRange, melee_range_multiplier);
		
		Vector vecForward;
		AngleVectors(pPlayer->EyeAngles(), &vecForward);
		Vector vecSwingStart = pPlayer->Weapon_ShootPosition();
		Vector vecSwingEnd   = vecSwingStart + (flSwingRange * vecForward);
		
		bool bMvMRobot = (pPlayer->GetTeamNumber() == TF_TEAM_PVE_INVADERS && TFGameRules()->IsMannVsMachineMode());
		
		/* special case for functional-but-undocumented attribute class 'melee_cleave_attack' */
		if (bCleaveAttack) {
			Ray_t ray;
			ray.Init(vecSwingStart, vecSwingEnd, vecSwingMins, vecSwingMaxs);
			
			CBaseEntity *ents[0x100];
			int count = UTIL_EntitiesAlongRay(ents, 0x100, ray, (FL_CLIENT | FL_OBJECT));
			
			int nHit = 0;
			for (int i = 0; i < count; ++i) {
				/* ignore ourself */
				if (ents[i] == pPlayer) continue;
				
				/* MvM robots ignore teammate hits */
				if (bMvMRobot && ents[i]->GetTeamNumber() != pPlayer->GetTeamNumber()) continue;
				
				if (pTraces != nullptr) {
					trace_t temp;
					UTIL_TraceModel(vecSwingStart, vecSwingEnd, vecSwingMins, vecSwingMaxs, ents[i], COLLISION_GROUP_NONE, &temp);
					(*pTraces)[pTraces->AddToTail()] = temp;
				}
				
				++nHit;
			}
			
			return (nHit > 0);
		}
		
		/* special case for the Homewrecker's attribute */
		int iSetDmgApplyToSapper = 0;
		CALL_ATTRIB_HOOK_INT(iSetDmgApplyToSapper, set_dmg_apply_to_sapper);
		if (iSetDmgApplyToSapper != 0) {
			CTraceFilterIgnorePlayers filter(nullptr, COLLISION_GROUP_NONE);
			
			if ((UTIL_TraceLine(vecSwingStart, vecSwingEnd,                             MASK_SOLID, &filter, &trace), trace.fraction < 1.0f) ||
				(UTIL_TraceHull(vecSwingStart, vecSwingEnd, vecSwingMins, vecSwingMaxs, MASK_SOLID, &filter, &trace), trace.fraction < 1.0f)) {
				/* we hit a non-player entity */
				
				if (trace.m_pEnt != nullptr && trace.m_pEnt->IsBaseObject() &&
					trace.m_pEnt->GetTeamNumber() == pPlayer->GetTeamNumber() &&
					static_cast<CBaseObject *>(trace.m_pEnt)->HasSapper()) {
					/* we hit a friendly building with a sapper */
					return true;
				}
			}
		}
		
		/* normal case */
		
		{
			NDebugOverlay::Clear();
			
			Ray_t ray;
			ray.Init(vecSwingStart, vecSwingEnd);
			
			NDebugOverlay::Line(vecSwingStart, vecSwingEnd, 0x00, 0xff, 0x00, true, 3600.0f);
			
			CBaseEntity *ents[1024];
			int count = UTIL_EntitiesAlongRay(ents, 1024, ray, 0);
			
			bool hit = false;
			int hitIdx = -1;
			
			DevMsg("UTIL_EntitiesAlongRay: hit %d entities\n", count);
			for (int i = 0; i < count; ++i) {
				if (ents[i] == pPlayer) continue;
				
				trace_t temp;
				enginetrace->ClipRayToEntity(ray, MASK_SOLID, ents[i], &temp);
				
				DevMsg("%3d | %5.3f | #%d \"%s\"\n", i, temp.fraction, ENTINDEX(ents[i]), ents[i]->GetClassname());
				
				if (temp.fraction < 1.0f) {
					NDebugOverlay::EntityText(ENTINDEX(ents[i]), 0, CFmtStrN<64>("%d", i), 3600.0f, 0xff, 0xff, 0xff, 0xff);
					
					if (!hit && temp.fraction < 1.0f) {
						hit = true;
						hitIdx = i;
						NDebugOverlay::EntityBounds(ents[i], 0xff, 0x00, 0x00, 0x40, 3600.0f);
					} else {
						NDebugOverlay::EntityBounds(ents[i], 0xff, 0xff, 0xff, 0x40, 3600.0f);
					}
				}
			}
		}
		// TODO: do a ray first with no extent
		// if that fails, THEN do a ray with the mins and maxs
		
		// are we still able to hit the world with this code?
		
		/* didn't hit anything */
		return false;
	}
}


#endif
