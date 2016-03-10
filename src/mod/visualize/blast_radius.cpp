#include "mod.h"
#include "util/scope.h"
#include "stub/baseentity.h"
#include "stub/tfplayer.h"


// TODO: make video with this:
// stock RL vs direct hit
// stock GL vs loch n load
// stock RL: RS 0/1/2/3/4


// blast radius works for:
// RL/GL/SL
// detonator/scorch

// TODO:
// jarate/milk


struct CTFRadiusDamageInfo
{
	CTakeDamageInfo *m_DmgInfo;   // +0x00
	Vector m_vecOrigin;           // +0x04: blast origin
	float m_flRadius;             // +0x10: blast radius
	CBaseEntity *m_pEntityIgnore; // +0x14
	float m_unknown_18;           // +0x18, default 0.0f, ?
	float m_unknown_1c;           // +0x1c, default 1.0f, scales damage force or something
	int m_unknown_20;             // +0x20, default 0, ?
	float m_flFalloff;            // +0x24: how much the damage is decreased by at the edge of the blast
};


namespace Mod_Visualize_Blast_Radius
{
	// draw multiple concentric semi-transparent spheres representing each 10% damage threshold, colored based on distance
	// draw a number over each sphere showing the ~damage at that distance
	
	// for each entity hit:
	// draw their absbox and color it in based on the closeness
	// draw text: the damage amount, and the percentage distance
	
	
	ConVar cvar_duration("sig_visualize_blast_radius_duration", "5.00", FCVAR_NOTIFY,
		"Visualization: duration");
	ConVar cvar_clear("sig_visualize_blast_radius_clear", "1", FCVAR_NOTIFY,
		"Visualization: clear overlays each time?");
	
	
	CBasePlayer *GetLocalPlayer()
	{
		static ConVarRef index("sig_debug_listenserverhost_index");
		return UTIL_PlayerByIndex(index.GetInt());
	}
	
	
	Color RatioToColor(float ratio)
	{
		ratio = Clamp(ratio, 0.0f, 1.0f);
		
		float r;
		float g;
		
		if (ratio < 0.50f) {
			r = 1.00f;
			g = ratio * 2;
		} else {
			r = 1.00f - (ratio * 2);
			g = 1.00f;
		}
		
		r *= 255.0f;
		g *= 255.0f;
		
		return Color(lrint(r), lrint(g), 0x00, 0xff);
	}
	
	
	CTFRadiusDamageInfo *radiusinfo = nullptr;
	RefCount rc_CTFGameRules_RadiusDamage;
	DETOUR_DECL_MEMBER(void, CTFGameRules_RadiusDamage, CTFRadiusDamageInfo& info)
	{
		radiusinfo = &info;
		
		SCOPED_INCREMENT(rc_CTFGameRules_RadiusDamage);
		DETOUR_MEMBER_CALL(CTFGameRules_RadiusDamage)(info);
	}
	
	DETOUR_DECL_MEMBER(void, CEntitySphereQuery_ctor, const Vector& center, float radius, int flagMask)
	{
		if (rc_CTFGameRules_RadiusDamage > 0) {
			if (cvar_clear.GetBool()) {
				NDebugOverlay::Clear();
			}
			
			QAngle angles = vec3_angle;
			CBasePlayer *player = GetLocalPlayer();
			if (player != nullptr) {
				angles = player->EyeAngles();
			//	angles.x = 0.0f;
			//	angles.z = 0.0f;
			}
			
			Vector fwd, rt, up;
			AngleVectors(angles, &fwd, &rt, &up);
			
			char buf[1024];
			
			float damage_center = radiusinfo->m_DmgInfo->GetDamage();
			float damage_edge   = damage_center * radiusinfo->m_flFalloff;
			
			float ratio_center = 1.00f;
			float ratio_edge   = (damage_edge / damage_center);
			
			for (float f = 1.0f; f >= radiusinfo->m_flFalloff - 0.01f; f -= 0.1f) {
				float dist = RemapValClamped(f, 1.0f, radiusinfo->m_flFalloff, 0.0f, radiusinfo->m_flRadius);
				
				Vector where_r = radiusinfo->m_vecOrigin + (dist * rt);
				Vector where_l = radiusinfo->m_vecOrigin - (dist * rt);
				
				snprintf(buf, sizeof(buf), "%.0f%%%%", f * 100.0f);
				NDebugOverlay::EntityTextAtPosition(where_r, -1, buf, cvar_duration.GetFloat(), 0xff, 0xff, 0xff, 0xff);
				NDebugOverlay::EntityTextAtPosition(where_l, -1, buf, cvar_duration.GetFloat(), 0xff, 0xff, 0xff, 0xff);
				snprintf(buf, sizeof(buf), "%.0f", dist);
				NDebugOverlay::EntityTextAtPosition(where_r, 1, buf, cvar_duration.GetFloat(), 0xff, 0xff, 0xff, 0xff);
				NDebugOverlay::EntityTextAtPosition(where_l, 1, buf, cvar_duration.GetFloat(), 0xff, 0xff, 0xff, 0xff);
				
				NDebugOverlay::EntityTextAtPosition(where_r, 2, "HU", cvar_duration.GetFloat(), 0xff, 0xff, 0xff, 0xff);
				NDebugOverlay::EntityTextAtPosition(where_l, 2, "HU", cvar_duration.GetFloat(), 0xff, 0xff, 0xff, 0xff);
				
				Color c = RatioToColor(RemapValClamped(f, 0.9f, radiusinfo->m_flFalloff, 1.0f, 0.0f));
				NDebugOverlay::Circle(radiusinfo->m_vecOrigin, angles, dist, c.r(), c.g(), c.b(), 0x40, false, cvar_duration.GetFloat());
			}
			
			//NDebugOverlay::Sphere(radiusinfo->m_vecOrigin, angles, radiusinfo->m_flRadius, r, g, b, a, false, cvar_duration.GetFloat());
			//NDebugOverlay::Circle(radiusinfo->m_vecOrigin, angles, radiusinfo->m_flRadius, r, g, b, a, false, cvar_duration.GetFloat());
			
			//snprintf(buf, sizeof(buf), "radius: %.1f HU", radiusinfo->m_flRadius);
			//NDebugOverlay::EntityTextAtPosition(radiusinfo->m_vecOrigin, 1, buf, cvar_duration.GetFloat(), 0xff, 0xff, 0xff, 0xff);
		}
		
		DETOUR_MEMBER_CALL(CEntitySphereQuery_ctor)(center, radius, flagMask);
	}
	
	RefCount rc_CTFRadiusDamageInfo_ApplyToEntity;
	DETOUR_DECL_MEMBER(int, CTFRadiusDamageInfo_ApplyToEntity, CBaseEntity *ent)
	{
		if (rc_CTFGameRules_RadiusDamage > 0) ++rc_CTFRadiusDamageInfo_ApplyToEntity;
		auto result = DETOUR_MEMBER_CALL(CTFRadiusDamageInfo_ApplyToEntity)(ent);
		if (rc_CTFGameRules_RadiusDamage > 0) --rc_CTFRadiusDamageInfo_ApplyToEntity;
		return result;
	}
	
	DETOUR_DECL_MEMBER(int, CBaseEntity_TakeDamage, const CTakeDamageInfo& info)
	{
		if (rc_CTFRadiusDamageInfo_ApplyToEntity > 0 && radiusinfo != nullptr) {
			auto ent = reinterpret_cast<CBaseEntity *>(this);
			if (rtti_cast<CBaseCombatCharacter *>(ent) != nullptr) {
				float damage_center = radiusinfo->m_DmgInfo->GetDamage();
				float damage_edge   = damage_center * radiusinfo->m_flFalloff;
				float damage_actual = info.GetDamage();
				
			//	DevMsg("damage_center: %f\n", damage_center);
			//	DevMsg("damage_edge:   %f\n", damage_edge);
			//	DevMsg("damage_actual: %f\n", damage_actual);
				
				float ratio_center = 1.00f;
				float ratio_edge   = (damage_edge   / damage_center);
				float ratio_actual = (damage_actual / damage_center);
				
			//	DevMsg("ratio_center: %f\n", ratio_center);
			//	DevMsg("ratio_edge:   %f\n", ratio_edge);
			//	DevMsg("ratio_actual: %f\n", ratio_actual);
				
				Color c = RatioToColor(RemapValClamped(ratio_actual, ratio_edge, ratio_center, 0.00f, 1.00f));
				NDebugOverlay::EntityBounds(ent, c.r(), c.g(), c.b(), 0x20, cvar_duration.GetFloat());
				
				char buf[1024];
				
				NDebugOverlay::EntityText(ENTINDEX(ent), -9, "Distance:", cvar_duration.GetFloat(), 0xff, 0xff, 0xff, 0xff);
				snprintf(buf, sizeof(buf), "%.0f HU", RemapValClamped(ratio_actual, 1.0f, radiusinfo->m_flFalloff, 0.0f, radiusinfo->m_flRadius));
				NDebugOverlay::EntityText(ENTINDEX(ent), -8, buf, cvar_duration.GetFloat(), 0xff, 0xff, 0xff, 0xff);
				NDebugOverlay::EntityText(ENTINDEX(ent), -6, "Splash multiplier:", cvar_duration.GetFloat(), 0xff, 0xff, 0xff, 0xff);
				snprintf(buf, sizeof(buf), "%.0f%%%%", ratio_actual * 100.0f);
				NDebugOverlay::EntityText(ENTINDEX(ent), -5, buf, cvar_duration.GetFloat(), 0xff, 0xff, 0xff, 0xff);
			//	snprintf(buf, sizeof(buf), "%.0f", info.GetDamage());
			//	NDebugOverlay::EntityText(ENTINDEX(ent), -5, buf, cvar_duration.GetFloat(), 0xff, 0xff, 0xff, 0xff);
			}
		}
		
		return DETOUR_MEMBER_CALL(CBaseEntity_TakeDamage)(info);
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Visualize:Blast_Radius")
		{
			MOD_ADD_DETOUR_MEMBER(CTFGameRules_RadiusDamage,         "CTFGameRules::RadiusDamage");
			MOD_ADD_DETOUR_MEMBER(CEntitySphereQuery_ctor,           "CEntitySphereQuery::CEntitySphereQuery [C1]");
			MOD_ADD_DETOUR_MEMBER(CTFRadiusDamageInfo_ApplyToEntity, "CTFRadiusDamageInfo::ApplyToEntity");
			MOD_ADD_DETOUR_MEMBER(CBaseEntity_TakeDamage,            "CBaseEntity::TakeDamage");
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_visualize_blast_radius", "0", FCVAR_NOTIFY,
		"Visualization: draw sphere representing blast radius",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
