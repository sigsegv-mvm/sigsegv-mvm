#ifndef _INCLUDE_SIGSEGV_MOD_DEBUG_MELEE_TRACE_NORMAL_H_
#define _INCLUDE_SIGSEGV_MOD_DEBUG_MELEE_TRACE_NORMAL_H_


/* should work identically to Valve's code, at least as of 20151007a */
namespace Normal
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
		
		CTraceFilterIgnoreFriendlyCombatItems filter_humans(pPlayer, COLLISION_GROUP_NONE);
		CTraceFilterIgnoreTeammates           filter_robots(pPlayer, COLLISION_GROUP_NONE);
		
		ITraceFilter *pFilter = (bMvMRobot ? (ITraceFilter *)&filter_robots : (ITraceFilter *)&filter_humans);
		
		UTIL_TraceLine(vecSwingStart, vecSwingEnd, MASK_SOLID, pFilter, &trace);
		if (trace.fraction < 1.0f) return true;
		
		UTIL_TraceHull(vecSwingStart, vecSwingEnd, vecSwingMins, vecSwingMaxs, MASK_SOLID, pFilter, &trace);
		if (trace.fraction < 1.0f) {
			/* we hit something via the hull trace; fix up the intersection point */
			if (trace.m_pEnt != nullptr && trace.m_pEnt->IsBSPModel()) {
				FindHullIntersection(vecSwingStart, trace, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, pPlayer);
			}
			
			return true;
		}
		
		/* didn't hit anything */
		return false;
	}
}


#if 0

/* still not 100% sure what the purpose of this is... needs a closer look */

void FindHullIntersection(const Vector& vecWeaponShootPos, CGameTrace& trace, const Vector& vecDuckMins, const Vector& vecDuckMaxs, CBaseEntity *pOwner)
{
	CTraceFilterSimple filter(pOwner);
	trace_t tr;
	
	UTIL_TraceLine(vecWeaponShootPos, vecWeaponShootPos + (2.0f * (trace.endpos - vecWeaponShootPos)), MASK_SOLID, &filter, &tr);
	if (tr.fraction < 1.0f) {
		trace = tr;
		return;
	}
	
	Vector vecMinsMaxs[2] {
		vecDuckMins,
		vecDuckMaxs,
	};
	
	float flBestSqr = Square(1000000.0f);
	for (int i = 0; i <= 1; ++i) {
		for (int j = 0; j <= 1; ++j) {
			for (int k = 0; k <= 1; ++k) {
				Vector vecStart = vecWeaponShootPos;
				Vector vecDelta = Vector(vecMinsMaxs[i].x, vecMinsMaxs[j].y, vecMinsMaxs[k].z);
				
				UTIL_TraceLine(vecStart, vecStart + vecDelta, MASK_SOLID, &filter, &tr);
				if (tr.traction < 1.0f) {
					float flDistSqr = vecWeaponShootPos.DistToSqr(tr.endpos);
					if (flDistSqr < flBestSqr) {
						flBestSqr = flDistSqr;
						trace = tr;
					}
				}
			}
		}
	}
}

#endif


#endif
