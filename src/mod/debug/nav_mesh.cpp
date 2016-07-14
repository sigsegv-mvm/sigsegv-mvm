#include "mod.h"
#include "stub/nav.h"


void ForEachArea(const std::function<void(CTFNavArea *)>& functor)
{
	CUtlVector<CTFNavArea *>& vec = TheNavAreas;
	
	FOR_EACH_VEC(vec, i) {
		functor(vec[i]);
	}
}


namespace Mod_Debug_Nav_Mesh
{
	constexpr float INTERVAL = 0.25f;
	
	
	ConVar cvar_mode("sig_debug_nav_mesh_mode", "", FCVAR_NOTIFY,
		"Debug: nav mesh debug mode");
	
	
	void UpdateOverlays()
	{
		const char *mode = cvar_mode.GetString();
		
		if (V_stricmp(mode, "spawn") == 0) {
			ForEachArea([](CTFNavArea *area){
				Extent ext;
				area->GetExtent(&ext);
				
				TFNavAttributeType attr = area->GetTFAttributes();
				
				bool red_spawn  = ((attr & RED_SPAWN_ROOM)  != 0);
				bool blu_spawn  = ((attr & BLUE_SPAWN_ROOM) != 0);
				bool spawn_exit = ((attr & SPAWN_ROOM_EXIT) != 0);
				
				if (red_spawn) {
					if (spawn_exit) {
						NDebugOverlay::Box(ext.lo, vec3_origin, (ext.hi - ext.lo), 0xff, 0x80, 0x80, 0x40, INTERVAL);
					} else {
						NDebugOverlay::Box(ext.lo, vec3_origin, (ext.hi - ext.lo), 0xff, 0x00, 0x00, 0x40, INTERVAL);
					}
				}
				if (blu_spawn) {
					if (spawn_exit) {
						NDebugOverlay::Box(ext.lo, vec3_origin, (ext.hi - ext.lo), 0x80, 0x80, 0xff, 0x40, INTERVAL);
					} else {
						NDebugOverlay::Box(ext.lo, vec3_origin, (ext.hi - ext.lo), 0x00, 0x00, 0xff, 0x40, INTERVAL);
					}
				}
			});
		}
	}
	
	
	class CMod : public IMod, public IFrameUpdateListener
	{
	public:
		CMod() : IMod("Debug:Nav_Mesh") {}
		
		virtual bool ShouldReceiveFrameEvents() const override { return this->IsEnabled(); }
		
		virtual void FrameUpdatePostEntityThink() override
		{
			if (this->m_ctUpdate.IsElapsed()) {
				this->m_ctUpdate.Start(INTERVAL);
				
				UpdateOverlays();
			}
		}
		
	private:
		CountdownTimer m_ctUpdate;
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_debug_nav_mesh", "0", FCVAR_NOTIFY,
		"Debug: nav mesh stuff",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
