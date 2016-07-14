#include "mod.h"
#include "stub/entities.h"
#include "stub/tfweaponbase.h"
#include "stub/tfplayer.h"
#include "util/scope.h"


// FINDINGS:
// - the problem is specifically that flame entities exist for too long (thereby going farther)
// - high latency (ping) in itself is not the problem
// - short-term changes in ping, especially jitter, is *part*, but not all, of the problem
// - the ultimate culprit is sv_clockcorrection_msecs (server-side clock correction)
//   - the default value of this convar is 60
//   - setting it to 0 completely solves the flame distance variance problem
//     (but obviously might have other deleterious effects on who-knows-what)
// 
// Laggy, jittery clients plus server-side clock correction leads to
// - flames lasting longer and therefore traveling farther before being removed
// - greatly increased overall variance in flame duration/distance


// STUFF THAT I HAVE RULED OUT:
// - random number generation (for tf_flamethrower_vecrand or for tf_flamethrower_flametime)
// - anything related to tf_flamethrower_float or tf_flamethrower_drag
// - client side prediction
// - insufficient CPU priority on dedicated server (RT prio had no effect)
// - skipping ticks or thinks
// - client side interp
// - lag compensation
// - client-side clock drift correction
// - server gpGlobals->curtime not agreeing with reality (Plat_FloatTime etc)


namespace Mod_Debug_Flamethrower_Mojo
{
	RefCount rc_CTFFlameEntity_Create;
	RefCount rc_CTFFlameEntity_FlameThink;
	
	
	std::map<CHandle<CBaseEntity>, Vector> flame_pos_start;
	std::map<CHandle<CBaseEntity>, float>  flame_realtime_start;
	std::map<CHandle<CBaseEntity>, int>    flame_tick_start;
	std::map<CHandle<CBaseEntity>, Color>  flame_color;
	std::map<CHandle<CBaseEntity>, int>    flame_thinks;
	std::map<CHandle<CBaseEntity>, bool>   flame_thought_this_tick;
	std::map<CHandle<CBaseEntity>, bool>   flame_spawned_this_tick;
	
	std::deque<double> recent_dists;
	
	
	Color ChooseColor()
	{
		static long i = 0;
		
		switch ((i++) % 8) {
		case 0: return Color(0xff, 0x00, 0x00, 0xff);
		case 1: return Color(0xff, 0x80, 0x00, 0xff);
		case 2: return Color(0xff, 0xff, 0x00, 0xff);
		case 3: return Color(0x00, 0xff, 0x00, 0xff);
		case 4: return Color(0x00, 0xff, 0xff, 0xff);
		case 5: return Color(0x00, 0x00, 0xff, 0xff);
		case 6: return Color(0x80, 0x00, 0xff, 0xff);
		case 7: return Color(0xff, 0x00, 0xff, 0xff);
		default: return Color(0x00, 0x00, 0x00, 0x00);
		}
	}
	
	
	DETOUR_DECL_MEMBER(Vector, CTFFlameThrower_GetFlameOriginPos)
	{
		auto result = DETOUR_MEMBER_CALL(CTFFlameThrower_GetFlameOriginPos)();
		
	//	DevMsg("[CTFFlameThrower::GetFlameOriginPos]  [%+6.1f %+6.1f %+6.1f]\n",
	//		result.x, result.y, result.z);
		
		return result;
	}
	
	
	DETOUR_DECL_STATIC(CTFFlameEntity *, CTFFlameEntity_Create, const Vector& origin, const QAngle& angles, CBaseEntity *owner, float f1, int i1, float f2, bool b1, bool b2)
	{
		SCOPED_INCREMENT(rc_CTFFlameEntity_Create);
		
		auto result = DETOUR_STATIC_CALL(CTFFlameEntity_Create)(origin, angles, owner, f1, i1, f2, b1, b2);
		
		if (result != nullptr) {
		//	DevMsg("[CTFFlameEntity::Create]  [%+6.1f %+6.1f %+6.1f]\n",
		//		result->GetAbsOrigin().x, result->GetAbsOrigin().y, result->GetAbsOrigin().z);
			
			flame_pos_start[result]         = result->GetAbsOrigin();
			flame_realtime_start[result]    = Plat_FloatTime();
			flame_tick_start[result]        = gpGlobals->tickcount;
			flame_color[result]             = ChooseColor();
			flame_thinks[result]            = 0;
			flame_thought_this_tick[result] = false;
			flame_spawned_this_tick[result] = true;
			
			Vector velocity;
			result->GetVelocity(&velocity, nullptr);
			
		//	DevMsg("New flame entity speed: %.0f\n", velocity.Length());
		//	NDebugOverlay::EntityTextAtPosition(result->GetAbsOrigin(), 0, CFmtStrN<64>("%.0f", velocity.Length()), 1.0f, 0xff, 0xff, 0xff, 0xff);
		}
		
		return result;
	}
	
	
	ConVar cvar_stats_qty("sig_debug_flamethrower_mojo_stats_qty", "22", FCVAR_NOTIFY,
		"Debug: number of recent flame distances to keep track of for statistics");
	
	
	ConVar cvar_dead_flame_duration("sig_debug_flamethrower_mojo_dead_flame_duration", "1.0", FCVAR_NOTIFY,
		"Debug: How long to show boxes for flame entities after they've been removed");
	
	
	// reasons a flame may be removed:
	// - in OnCollide, if (enginetrace->GetPointContents(flame->GetAbsOrigin()) & MASK_WATER) != 0
	// - in FlameThink, if the time-to-live is exceeded
	// - in FlameThink, if forward trace from m_vecPrevPos to current pos with MASK_SOLID hits something (e.g. a wall)
	DETOUR_DECL_MEMBER(void, CTFFlameEntity_RemoveFlame)
	{
		auto flame = reinterpret_cast<CTFFlameEntity *>(this);
		
		auto it = flame_pos_start.find(flame);
		if (it != flame_pos_start.end()) {
			const Vector& pos_start = (*it).second;
			const Vector& pos_end   = flame->GetAbsOrigin();
			float dist = (pos_end - pos_start).Length();
			
			int dTick = gpGlobals->tickcount - flame_tick_start[flame];
			float dRealTime = Plat_FloatTime() - flame_realtime_start[flame];
			
		//	NDebugOverlay::EntityTextAtPosition(pos_end, 0, CFmtStrN<64>("%.0f", dist), cvar_dead_flame_duration.GetFloat(), 0xff, 0xff, 0xff, 0xff);
		//	NDebugOverlay::EntityTextAtPosition(pos_end, 0, CFmtStrN<64>("%dt", dTick), cvar_dead_flame_duration.GetFloat(), 0xff, 0xff, 0xff, 0xff);
		//	NDebugOverlay::EntityTextAtPosition(pos_end, 1, CFmtStrN<64>("%.3fs", dRealTime), cvar_dead_flame_duration.GetFloat(), 0xff, 0xff, 0xff, 0xff);
			
			NDebugOverlay::EntityBounds(flame, 0xff, 0xff, 0xff, 0x00, cvar_dead_flame_duration.GetFloat());
		//	NDebugOverlay::Cross3D(flame->WorldSpaceCenter(), 3.0f, 0xff, 0xff, 0xff, false, cvar_dead_flame_duration.GetFloat());
			
			recent_dists.push_back(dist);
			while (recent_dists.size() > cvar_stats_qty.GetInt()) {
				recent_dists.pop_front();
			}
			
			flame_pos_start.erase(it);
			flame_tick_start.erase(flame_tick_start.find(flame));
			flame_realtime_start.erase(flame_realtime_start.find(flame));
			flame_color.erase(flame_color.find(flame));
			flame_thinks.erase(flame_thinks.find(flame));
			flame_thought_this_tick.erase(flame_thought_this_tick.find(flame));
			flame_spawned_this_tick.erase(flame_spawned_this_tick.find(flame));
		}
			
		//	DevMsg("[CTFFlameEntity::RemoveFlame]  [%+6.1f %+6.1f %+6.1f]\n",
		//		flame->GetAbsOrigin().x, flame->GetAbsOrigin().y, flame->GetAbsOrigin().z);
		
		DETOUR_MEMBER_CALL(CTFFlameEntity_RemoveFlame)();
	}
	
	
	DETOUR_DECL_MEMBER(void, CTFFlameEntity_FlameThink)
	{
		SCOPED_INCREMENT(rc_CTFFlameEntity_FlameThink);
		
		auto flame = reinterpret_cast<CTFFlameEntity *>(this);
		
		flame_thought_this_tick[flame] = true;
		
		int& thinks = flame_thinks[flame];
		++thinks;
		
	//	Color c = flame_color[flame];
	//	c[0] += (0xff - c[0]) / 2;
	//	c[1] += (0xff - c[1]) / 2;
	//	c[2] += (0xff - c[2]) / 2;
		
	//	NDebugOverlay::Cross3D(flame->WorldSpaceCenter(), 3.0f, c.r(), c.g(), c.b(), true, gpGlobals->interval_per_tick);
	//	NDebugOverlay::EntityTextAtPosition(flame->WorldSpaceCenter(), 0, CFmtStrN<256>("THINK#%d", thinks), gpGlobals->interval_per_tick, c.r(), c.g(), c.b(), 0xff);
		
		DETOUR_MEMBER_CALL(CTFFlameEntity_FlameThink)();
	}
	
	
	DETOUR_DECL_MEMBER(void, IEngineTrace_TraceRay, const Ray_t& ray, unsigned int fMask, ITraceFilter *pTraceFilter, trace_t *pTrace)
	{
		DETOUR_MEMBER_CALL(IEngineTrace_TraceRay)(ray, fMask, pTraceFilter, pTrace);
		
		if (rc_CTFFlameEntity_FlameThink > 0 && fMask == MASK_SOLID) {
			auto m_pPassEnt = *reinterpret_cast<IHandleEntity **>((uintptr_t)pTraceFilter + 0x4);
			
			auto flame = rtti_cast<CTFFlameEntity *>(m_pPassEnt);
			if (flame != nullptr) {
				Color c = flame_color[flame];
				
				NDebugOverlay::Line(pTrace->startpos, pTrace->endpos, c.r(), c.g(), c.b(), true, gpGlobals->interval_per_tick);
			}
		}
	}
	
	
	ConVar cvar_stats_interval("sig_debug_flamethrower_mojo_stats_interval", "0.25", FCVAR_NOTIFY,
		"Debug: how often to refresh the flame distance statistics overlay");
	ConVar cvar_stats_screenpos_x("sig_debug_flamethrower_mojo_stats_screenpos_x", "0.77", FCVAR_NOTIFY,
		"Debug: where to put the statistics overlay");
	ConVar cvar_stats_screenpos_y("sig_debug_flamethrower_mojo_stats_screenpos_y", "0.05", FCVAR_NOTIFY,
		"Debug: where to put the statistics overlay");
	
	void DrawStatTextLine(int line, const char *text)
	{
		NDebugOverlay::ScreenText(cvar_stats_screenpos_x.GetFloat(), cvar_stats_screenpos_y.GetFloat() + (line * 0.0175f),
			text, 0xff, 0xff, 0xff, 0xff, cvar_stats_interval.GetFloat());
	}
	
	
	ConVar cvar_net_interval("sig_debug_flamethrower_mojo_net_interval", "0.25", FCVAR_NOTIFY,
		"Debug: how often to refresh the network overlay");
	ConVar cvar_net_screenpos_x("sig_debug_flamethrower_mojo_net_screenpos_x", "0.55", FCVAR_NOTIFY,
		"Debug: where to put the network overlay");
	ConVar cvar_net_screenpos_y("sig_debug_flamethrower_mojo_net_screenpos_y", "0.05", FCVAR_NOTIFY,
		"Debug: where to put the network overlay");
	
	void DrawNetTextLine(int line, const char *text)
	{
		NDebugOverlay::ScreenText(cvar_net_screenpos_x.GetFloat(), cvar_net_screenpos_y.GetFloat() + (line * 0.0175f),
			text, 0xff, 0xff, 0xff, 0xff, cvar_net_interval.GetFloat());
	}
	
	
	ConVar cvar_param_interval("sig_debug_flamethrower_mojo_param_interval", "0.25", FCVAR_NOTIFY,
		"Debug: how often to refresh the flamethrower parameter overlay");
	ConVar cvar_param_screenpos_x("sig_debug_flamethrower_mojo_param_screenpos_x", "0.03", FCVAR_NOTIFY,
		"Debug: where to put the flamethrower parameter overlay");
	ConVar cvar_param_screenpos_y("sig_debug_flamethrower_mojo_param_screenpos_y", "0.05", FCVAR_NOTIFY,
		"Debug: where to put the flamethrower parameter overlay");
	
	void DrawParamTextLine(int line, const char *text, int r = 0xff, int g = 0xff, int b = 0xff, int a = 0xff)
	{
		NDebugOverlay::ScreenText(cvar_param_screenpos_x.GetFloat(), cvar_param_screenpos_y.GetFloat() + (line * 0.0175f),
			text, r, g, b, a, cvar_param_interval.GetFloat());
	}
	void DrawParamConVarLine(int line, ConVarRef& ref)
	{
		const char *help_text = "";
		ConVar *pConVar = dynamic_cast<ConVar *>(ref.GetLinkedConVar());
		if (pConVar != nullptr) {
			help_text = pConVar->GetHelpText();
		}
		
		CFmtStrN<256> str("%-33s = %7.2f  (\"%s\")", ref.GetName(), ref.GetFloat(), help_text);
		
		if (strcmp(ref.GetDefault(), ref.GetString()) != 0) {
			DrawParamTextLine(line, str, 0xff, 0xff, 0x00, 0xff);
		} else {
			DrawParamTextLine(line, str);
		}
	}
	
	
	double ComputeStdDev(double mean)
	{
		if (recent_dists.size() < 2) return 0.0;
		
		double accum = 0.0;
		
		std::for_each(recent_dists.begin(), recent_dists.end(), [&](const double dist){
			accum += Square(dist - mean);
		});
		
		return sqrt(accum / (recent_dists.size() - 1));
	}
	
	
	void ComputeQuartiles(double& min, double& q1, double& median, double& q3, double& max)
	{
		if (recent_dists.empty()) {
			min    = 0.0;
			q1     = 0.0;
			median = 0.0;
			q3     = 0.0;
			max    = 0.0;
			return;
		}
		
		auto n_q1 = recent_dists.size() / 4;
		auto n_q2 = recent_dists.size() / 2;
		auto n_q3 = n_q1 + n_q2;
		
		n_q1 = Clamp(n_q1, 0U, recent_dists.size());
		n_q2 = Clamp(n_q2, 0U, recent_dists.size());
		n_q3 = Clamp(n_q3, 0U, recent_dists.size());
		
		std::vector<double> sorted(recent_dists.begin(), recent_dists.end());
		std::sort(sorted.begin(), sorted.end());
		
		q1     = sorted[n_q1];
		median = sorted[n_q2];
		q3     = sorted[n_q3];
		
		min = sorted.front();
		max = sorted.back();
	}
	
	
	DETOUR_DECL_STATIC(int, D_RandomInt, int iMinVal, int iMaxVal)
	{
		auto result = DETOUR_STATIC_CALL(D_RandomInt)(iMinVal, iMaxVal);
		
		if (rc_CTFFlameEntity_Create > 0) {
			DevMsg("RandomInt(%d, %d) = %d\n", iMinVal, iMaxVal, result);
		}
		
		return result;
	}
	
	
	DETOUR_DECL_MEMBER(void, CLagCompensationManager_StartLagCompensation, CBasePlayer *player, CUserCmd *cmd)
	{
		/* do nothing */
	}
	
	DETOUR_DECL_MEMBER(void, CLagCompensationManager_FinishLagCompensation, CBasePlayer *player)
	{
		/* do nothing */
	}
	
	
	void PostThink_DrawFlames()
	{
		for (int i = 0; i < ITFFlameEntityAutoList::AutoList().Count(); ++i) {
			auto flame = rtti_cast<CTFFlameEntity *>(ITFFlameEntityAutoList::AutoList()[i]);
			if (flame == nullptr) continue;
			
			Color c = flame_color[flame];
			
			NDebugOverlay::EntityBounds(flame, c.r(), c.g(), c.b(), 0x10, gpGlobals->interval_per_tick);
		//	NDebugOverlay::Cross3D(flame->WorldSpaceCenter(), 3.0f, 0xff, 0xff, 0xff, false, gpGlobals->interval_per_tick);
			
			Color c_light = c;
			c_light[0] += (0xff - c_light[0]) / 2;
			c_light[1] += (0xff - c_light[1]) / 2;
			c_light[2] += (0xff - c_light[2]) / 2;
			
			bool thought;
			bool think_missed_initial = false;
			bool think_missed_removed = false;
			
			if (flame_thought_this_tick.find(flame) != flame_thought_this_tick.end()) {
				thought = flame_thought_this_tick[flame];
				
				if (flame_spawned_this_tick[flame]) {
					think_missed_initial = true;
				}
			} else {
				thought = false;
				think_missed_removed = true;
			}
			
			if (thought) {
				int think_num = flame_thinks[flame];
				
				NDebugOverlay::EntityTextAtPosition(flame->WorldSpaceCenter(), -1, "THINK", gpGlobals->interval_per_tick, c_light.r(), c_light.g(), c_light.b(), 0xff);
				NDebugOverlay::EntityTextAtPosition(flame->WorldSpaceCenter(),  0, CFmtStrN<64>("#%d", think_num), gpGlobals->interval_per_tick, c_light.r(), c_light.g(), c_light.b(), 0xff);
			} else {
				if (think_missed_initial) {
					NDebugOverlay::EntityTextAtPosition(flame->WorldSpaceCenter(), -1, "SPAWN", gpGlobals->interval_per_tick, c_light.r(), c_light.g(), c_light.b(), 0xff);
				} else if (think_missed_removed) {
					NDebugOverlay::EntityTextAtPosition(flame->WorldSpaceCenter(), -1, "DEAD", gpGlobals->interval_per_tick, c_light.r(), c_light.g(), c_light.b(), 0xff);
				} else {
					NDebugOverlay::EntityTextAtPosition(flame->WorldSpaceCenter(), -1, "MISSED", gpGlobals->interval_per_tick, c_light.r(), c_light.g(), c_light.b(), 0xff);
					NDebugOverlay::EntityTextAtPosition(flame->WorldSpaceCenter(),  0, "THINK!", gpGlobals->interval_per_tick, c_light.r(), c_light.g(), c_light.b(), 0xff);
				}
			}
		}
	}
	
	void PostThink_PyroOverlay()
	{
		static CountdownTimer ctPyroOverlay;
		if (ctPyroOverlay.IsElapsed()) {
			ctPyroOverlay.Start(1.00f);
			
			for (int i = gpGlobals->maxClients + 1; i < 2048; ++i) {
				auto flamer = rtti_cast<CTFFlameThrower *>(UTIL_EntityByIndex(i));
				if (flamer == nullptr) continue;
				
				CTFPlayer *owner = ToTFPlayer(flamer->GetOwner());
				if (owner != nullptr) {
					NDebugOverlay::EntityBounds(owner, 0xff, 0xff, 0xff, 0x00, 1.00f);
					
					if (!owner->IsBot()) {
						Vector vecFwd;
						owner->EyeVectors(&vecFwd);
						Vector vecStart = owner->EyePosition() + Vector(0.0f, 0.0f, 50.0f);
						
					//	NDebugOverlay::Line(vecStart, vecStart + (500.0f * vecFwd), 0x80, 0x80, 0x80, true, 1.00f);
						
						NDebugOverlay::EntityTextAtPosition(vecStart + (-30.0f * vecFwd) + Vector(0.0f, 0.0f, 15.0f), 0, "HAMMER", 1.00f, 0xc0, 0xc0, 0xc0, 0xff);
						NDebugOverlay::EntityTextAtPosition(vecStart + (-30.0f * vecFwd) + Vector(0.0f, 0.0f, 15.0f), 1, " UNITS", 1.00f, 0xc0, 0xc0, 0xc0, 0xff);
					
						for (float x = 0.0f; x <= 500.0f; x += 20.0f) {
							Vector vecHash1 = vecStart + (x * vecFwd) + Vector(0.0f, 0.0f,  10.0f);
							Vector vecHash2 = vecStart + (x * vecFwd) + Vector(0.0f, 0.0f, -50.0f);
							Vector vecHash3 = vecStart + ((x - 3.0f) * vecFwd) + Vector(0.0f, 0.0f,  15.0f);
							
							NDebugOverlay::Line(vecHash1, vecHash2, 0x80, 0x80, 0x80, true, 1.00f);
							NDebugOverlay::EntityTextAtPosition(vecHash3, 0, CFmtStrN<64>("%.0f", x), 1.00f, 0xc0, 0xc0, 0xc0, 0xff);
						}
					}
				}
			}
		}
	}
	
	void PostThink_StatOverlay()
	{
		static CountdownTimer ctStatOverlay;
		if (ctStatOverlay.IsElapsed()) {
			ctStatOverlay.Start(cvar_stats_interval.GetFloat());
			
			double sum    = std::accumulate(recent_dists.begin(), recent_dists.end(), 0.0);
			double mean   = sum / (double)recent_dists.size();
			double stddev = ComputeStdDev(mean);
			
			DrawStatTextLine( 0, CFmtStrN<256>("DISTANCE STATISTICS FOR THE LAST %u FLAMES:", recent_dists.size()));
			
			DrawStatTextLine( 2, CFmtStrN<256>("  Mean: %4.0f HU", mean));
			DrawStatTextLine( 3, CFmtStrN<256>("StdDev: %4.0f HU", stddev));
			
			double min, q1, median, q3, max;
			ComputeQuartiles(min, q1, median, q3, max);
			
			DrawStatTextLine( 5, CFmtStrN<256>("   Max: %4.0f HU", max));
			DrawStatTextLine( 6, CFmtStrN<256>("    Q3: %4.0f HU", q3));
			DrawStatTextLine( 7, CFmtStrN<256>("Median: %4.0f HU", median));
			DrawStatTextLine( 8, CFmtStrN<256>("    Q1: %4.0f HU", q1));
			DrawStatTextLine( 9, CFmtStrN<256>("   Min: %4.0f HU", min));
			
			float range = max - min;
			
			DrawStatTextLine(11, CFmtStrN<256>(" Range: %4.0f HU  (max-min)", range));
		}
	}
	
	void PostThink_NetOverlay()
	{
		static CountdownTimer ctNetOverlay;
		if (ctNetOverlay.IsElapsed()) {
			ctNetOverlay.Start(cvar_net_interval.GetFloat());
			
			static ConVarRef net_fakelag             ("net_fakelag");
			static ConVarRef net_fakejitter          ("net_fakejitter");
			static ConVarRef sv_clockcorrection_msecs("sv_clockcorrection_msecs");
			static ConVarRef host_timescale          ("host_timescale");
			
			DrawNetTextLine(0, "SERVER NETWORK SETTINGS:");
			DrawNetTextLine(1, "(this is NOT a listen server!)");
			
			DrawNetTextLine(3, CFmtStrN<256>("net_fakelag    = %3.0f", net_fakelag.GetFloat()));
			DrawNetTextLine(4, CFmtStrN<256>("net_fakejitter = %3.0f", net_fakejitter.GetFloat()));
			
			float lag    = net_fakelag.GetFloat();
			float jitter = Min(lag * 0.5f, net_fakejitter.GetFloat());
			
			DrawNetTextLine(6, "EFFECTIVE LAG:");
			DrawNetTextLine(7, CFmtStrN<256>("%.0f +/- %.0f milliseconds", lag, jitter));
			
			DrawNetTextLine(10, CFmtStrN<256>("sv_clockcorrection_msecs = %3d", sv_clockcorrection_msecs.GetInt()));
			
			DrawNetTextLine(13, CFmtStrN<256>("host_timescale = %.3f", host_timescale.GetFloat()));
		}
	}
	
	void PostThink_ParamOverlay()
	{
		static CountdownTimer ctParamOverlay;
		if (ctParamOverlay.IsElapsed()) {
			ctParamOverlay.Start(cvar_param_interval.GetFloat());
			
			static ConVarRef tf_flamethrower_boxsize          ("tf_flamethrower_boxsize");
			static ConVarRef tf_flamethrower_flametime        ("tf_flamethrower_flametime");
			
			static ConVarRef tf_flamethrower_drag             ("tf_flamethrower_drag");
			static ConVarRef tf_flamethrower_float            ("tf_flamethrower_float");
			
			static ConVarRef tf_flamethrower_velocity         ("tf_flamethrower_velocity");
			static ConVarRef tf_flamethrower_vecrand          ("tf_flamethrower_vecrand");
			static ConVarRef tf_flamethrower_velocityfadestart("tf_flamethrower_velocityfadestart");
			static ConVarRef tf_flamethrower_velocityfadeend  ("tf_flamethrower_velocityfadeend");
			
			DrawParamTextLine(0, "FLAMETHROWER CONVAR SETTINGS:");
			
			DrawParamConVarLine( 2, tf_flamethrower_boxsize);
			DrawParamConVarLine( 3, tf_flamethrower_flametime);
			
			DrawParamConVarLine( 5, tf_flamethrower_drag);
			DrawParamConVarLine( 6, tf_flamethrower_float);
			
			DrawParamConVarLine( 8, tf_flamethrower_velocity);
			DrawParamConVarLine( 9, tf_flamethrower_vecrand);
			DrawParamConVarLine(10, tf_flamethrower_velocityfadestart);
			DrawParamConVarLine(11, tf_flamethrower_velocityfadeend);
		}
	}
	
	void PostThink_RefillAmmo()
	{
		static CountdownTimer ctRefillAmmo;
		if (ctRefillAmmo.IsElapsed()) {
			ctRefillAmmo.Start(1.0f);
			
			for (int i = 1; i <= gpGlobals->maxClients; ++i) {
				CBasePlayer *player = UTIL_PlayerByIndex(i);
				if (player == nullptr) continue;
				
				for (int j = 0; j < MAX_AMMO_SLOTS; ++j) {
					player->GiveAmmo(1000, j, true);
				}
			}
		}
	}

	
	
	class CMod : public IMod, public IFrameUpdateListener
	{
	public:
		CMod() : IMod("Debug:Flamethrower_Mojo")
		{
			MOD_ADD_DETOUR_MEMBER(CTFFlameThrower_GetFlameOriginPos, "CTFFlameThrower::GetFlameOriginPos");
			
			MOD_ADD_DETOUR_STATIC(CTFFlameEntity_Create, "CTFFlameEntity::Create");
			
			MOD_ADD_DETOUR_MEMBER(CTFFlameEntity_RemoveFlame, "CTFFlameEntity::RemoveFlame");
			
			MOD_ADD_DETOUR_MEMBER(CTFFlameEntity_FlameThink, "CTFFlameEntity::FlameThink");
			
			MOD_ADD_DETOUR_MEMBER(IEngineTrace_TraceRay, "IEngineTrace::TraceRay");
			
		//	this->AddDetour(new CDetour("RandomInt", reinterpret_cast<void *>(&RandomInt), GET_STATIC_CALLBACK(D_RandomInt), GET_STATIC_INNERPTR(D_RandomInt)));
			
		//	MOD_ADD_DETOUR_MEMBER(CLagCompensationManager_StartLagCompensation,  "CLagCompensationManager::StartLagCompensation");
		//	MOD_ADD_DETOUR_MEMBER(CLagCompensationManager_FinishLagCompensation, "CLagCompensationManager::FinishLagCompensation");
		}
		
		virtual bool ShouldReceiveFrameEvents() const override { return this->IsEnabled(); }
		
		virtual void FrameUpdatePreEntityThink() override
		{
			for (auto& pair : flame_thought_this_tick) {
				pair.second = false;
			}
			for (auto& pair : flame_spawned_this_tick) {
				pair.second = false;
			}
		}
		
		virtual void FrameUpdatePostEntityThink() override
		{
			PostThink_DrawFlames();
			PostThink_PyroOverlay();
			PostThink_StatOverlay();
			PostThink_NetOverlay();
			PostThink_ParamOverlay();
			PostThink_RefillAmmo();
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_debug_flamethrower_mojo", "0", FCVAR_NOTIFY,
		"Debug: investigate the 'flamethrower mojo' phenomenon",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
