#include "mod.h"
#include "stub/debugoverlay.h"


namespace Mod_Debug_Overlay_Send
{
	ConVar cvar_player_index("sig_debug_overlay_send_player_index", "0", FCVAR_NOTIFY,
		"Entity index of the \"local player\"");
	
	
	class MyDebugOverlay : public IVDebugOverlay
	{
	public:
		MyDebugOverlay() {}
		virtual ~MyDebugOverlay() {}
		
		virtual void AddEntityTextOverlay(int ent_index, int line_offset, float duration, int r, int g, int b, int a, const char *format, ...) override
		{
			DevMsg("MyDebugOverlay::%s\n", "AddEntityTextOverlay");
			
			char buf[1024];
			va_list va;
			
			va_start(va, format);
			Q_vsnprintf(buf, sizeof(buf), format, va);
			va_end(va);
			
			IGameEvent *event = gameeventmanager->CreateEvent("overlay_AddEntityTextOverlay");
			if (event != nullptr) {
				event->SetInt   ("ent_index",   ent_index);
				event->SetInt   ("line_offset", line_offset);
				event->SetFloat ("duration",    duration);
				event->SetInt   ("r",           r);
				event->SetInt   ("g",           g);
				event->SetInt   ("b",           b);
				event->SetInt   ("a",           a);
				event->SetString("text",        buf);
				
				gameeventmanager->FireEvent(event);
			}
		}
		
		virtual void AddBoxOverlay(const Vector& origin, const Vector& mins, const Vector& max, const QAngle& orientation, int r, int g, int b, int a, float duration) override
		{
			DevMsg("MyDebugOverlay::%s\n", "AddBoxOverlay");
		}
		
		virtual void AddTriangleOverlay(const Vector& p1, const Vector& p2, const Vector& p3, int r, int g, int b, int a, bool noDepthTest, float duration) override
		{
			DevMsg("MyDebugOverlay::%s\n", "AddTriangleOverlay");
		}
		
		virtual void AddLineOverlay(const Vector& origin, const Vector& dest, int r, int g, int b, bool noDepthTest, float duration) override
		{
			DevMsg("MyDebugOverlay::%s\n", "AddLineOverlay");
		}
		
		virtual void AddTextOverlay(const Vector& origin, float duration, const char *format, ...) override
		{
			DevMsg("MyDebugOverlay::%s\n", "AddTextOverlay");
		}
		
		virtual void AddTextOverlay(const Vector& origin, int line_offset, float duration, const char *format, ...) override
		{
			DevMsg("MyDebugOverlay::%s\n", "AddTextOverlay");
		}
		
		virtual void AddScreenTextOverlay(float flXPos, float flYPos, float flDuration, int r, int g, int b, int a, const char *text) override
		{
			DevMsg("MyDebugOverlay::%s\n", "AddScreenTextOverlay");
		}
		
		virtual void AddSweptBoxOverlay(const Vector& start, const Vector& end, const Vector& mins, const Vector& max, const QAngle& angles, int r, int g, int b, int a, float flDuration) override
		{
			DevMsg("MyDebugOverlay::%s\n", "AddSweptBoxOverlay");
		}
		
		virtual void AddGridOverlay(const Vector& origin) override
		{
			DevMsg("MyDebugOverlay::%s\n", "AddGridOverlay");
		}
		
		virtual int ScreenPosition(const Vector& point, Vector& screen) override
		{
			DevMsg("MyDebugOverlay::%s\n", "ScreenPosition");
			return 0;
		}
		
		virtual int ScreenPosition(float flXPos, float flYPos, Vector& screen) override
		{
			DevMsg("MyDebugOverlay::%s\n", "ScreenPosition");
			return 0;
		}
		
		
		virtual OverlayText_t *GetFirst() override
		{
			DevMsg("MyDebugOverlay::%s\n", "GetFirst");
			return nullptr;
		}
		
		virtual OverlayText_t *GetNext(OverlayText_t *current) override
		{
			DevMsg("MyDebugOverlay::%s\n", "GetNext");
			return nullptr;
		}
		
		virtual void ClearDeadOverlays() override
		{
			DevMsg("MyDebugOverlay::%s\n", "ClearDeadOverlays");
		}
		
		virtual void ClearAllOverlays() override
		{
			DevMsg("MyDebugOverlay::%s\n", "ClearAllOverlays");
		}
		
		
		virtual void AddTextOverlayRGB(const Vector& origin, int line_offset, float duration, float r, float g, float b, float alpha, const char *format, ...) override
		{
			DevMsg("MyDebugOverlay::%s\n", "AddTextOverlayRGB");
		}
		
		virtual void AddTextOverlayRGB(const Vector& origin, int line_offset, float duration, int r, int g, int b, int a, const char *format, ...) override
		{
			DevMsg("MyDebugOverlay::%s\n", "AddTextOverlayRGB");
		}
		
		
		virtual void AddLineOverlayAlpha(const Vector& origin, const Vector& dest, int r, int g, int b, int a, bool noDepthTest, float duration) override
		{
			DevMsg("MyDebugOverlay::%s\n", "AddLineOverlayAlpha");
		}
		
		virtual void AddBoxOverlay2(const Vector& origin, const Vector& mins, const Vector& max, const QAngle& orientation, const Color& faceColor, const Color& edgeColor, float duration) override
		{
			DevMsg("MyDebugOverlay::%s\n", "AddBoxOverlay2");
		}
	};
	
	
	DETOUR_DECL_STATIC(CBasePlayer *, UTIL_GetListenServerHost)
	{
		return UTIL_PlayerByIndex(cvar_player_index.GetInt());
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Debug:Overlay_Send")
		{
			MOD_ADD_DETOUR_STATIC(UTIL_GetListenServerHost, "UTIL_GetListenServerHost");
		}
		
		virtual bool OnLoad() override
		{
			return true;
		}
		virtual void OnUnload() override
		{
			this->SetEnabled(false);
		}
		
		void SetEnabled(bool enable)
		{
			if (this->m_bEnabled != enable) {
				if (enable) {
					this->Enable();
				} else {
					this->Disable();
				}
				
				this->m_bEnabled = enable;
			}
		}
		
	private:
		void Enable()
		{
			assert(!this->m_bEnabled);
			
			IVDebugOverlay *do_real = debugoverlay;
			IVDebugOverlay *do_mine = new MyDebugOverlay();
			DevMsg("overriding debugoverlay: %08x -> %08x\n", (uintptr_t)do_real, (uintptr_t)do_mine);
			
			this->m_pSaved = do_real;
			debugoverlay   = do_mine;
			
			this->ToggleAllDetours(true);
		}
		void Disable()
		{
			assert(this->m_bEnabled);
			
			this->ToggleAllDetours(false);
			
			IVDebugOverlay *do_mine = debugoverlay;
			IVDebugOverlay *do_real = this->m_pSaved;
			DevMsg("restoring debugoverlay: %08x -> %08x\n", (uintptr_t)do_mine, (uintptr_t)do_real);
			
			debugoverlay = do_real;
			this->m_pSaved = nullptr;
			delete do_mine;
		}
		
		bool m_bEnabled = false;
		
		IVDebugOverlay *m_pSaved = nullptr;
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_debug_overlay_send", "0", FCVAR_NOTIFY,
		"Debug: overlay forwarding: server send",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
