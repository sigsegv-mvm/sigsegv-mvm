#include "mod.h"
#include "stub/cdebugoverlay.h"
#include "util/scope.h"

#include <overlaytext.h>


enum OverlayType_t
{
	OVERLAY_BOX = 0,
	OVERLAY_SPHERE,
	OVERLAY_LINE,
	OVERLAY_TRIANGLE,
	OVERLAY_SWEPT_BOX,
	OVERLAY_BOX2,
};


struct OverlayBase_t
{
	OverlayType_t m_Type;
	int m_nCreationTick;
	int m_nServerCount;
	float m_flEndTime;
	OverlayBase_t *m_pNextOverlay;
};

struct OverlaySphere_t : public OverlayBase_t
{
	Vector vOrigin;
	float flRadius;
	int nTheta;
	int nPhi;
	int r;
	int g;
	int b;
	int a;
};


namespace Mod_Util_Overlay_Enhancements
{
	// TODO:
	// enhancement to make EntityText(AtPosition)? use actual font tall property for line spacing instead of constant 13
	// upper-left console array is drawn in CConPanel::DrawNotify and does this properly
	// debug text overlays are drawn in CDebugOverlay::Paint and do this wrong
	
	
	ConVar cvar_textspacing("sig_util_overlay_enhancements_textspacing", "0", FCVAR_NOTIFY,
		"Utility: overlay forwarding: client rendering enhancements: use non-hardcoded height for text overlay line spacing");
	
	RefCount rc_CDebugOverlay_Paint;
	DETOUR_DECL_MEMBER(void, CDebugOverlay_Paint)
	{
		SCOPED_INCREMENT(rc_CDebugOverlay_Paint);
		DETOUR_MEMBER_CALL(CDebugOverlay_Paint)();
	}
	
	DETOUR_DECL_MEMBER(int, CMatSystemSurface_DrawColoredText, vgui::HFont font, int x, int y, int r, int g, int b, int a, const char *fmt, va_list argptr)
	{
		if (cvar_textspacing.GetBool() && rc_CDebugOverlay_Paint > 0 && g_pVGuiSurface != nullptr && strcmp(fmt, "%s") == 0) {
			va_list va;
			va_copy(va, argptr);
			
			const char *str = va_arg(va, const char *);
			auto pCurrText = reinterpret_cast<const OverlayText_t *>((uintptr_t)str - offsetof(OverlayText_t, text));
			
			/* undo the stupid line spacing */
			y -= (pCurrText->lineOffset * 13);
			
			/* apply proper line spacing */
			y += (pCurrText->lineOffset * (g_pVGuiSurface->GetFontTall(font) + 1));
			
			va_end(va);
		}
		
		return DETOUR_MEMBER_CALL(CMatSystemSurface_DrawColoredText)(font, x, y, r, g, b, a, fmt, argptr);
	}
	
	
	ConVar cvar_sphere("sig_util_overlay_enhancements_sphere", "0", FCVAR_NOTIFY,
		"Utility: overlay forwarding: client rendering enhancements: use better sphere rendering");
	
	DETOUR_DECL_STATIC(void, CDebugOverlay_DrawOverlay, OverlayBase_t *pOverlay)
	{
		if (cvar_sphere.GetBool()) {
			/* for OVERLAY_SPHERE, CDebugOverlay::DrawOverlay calls the IMaterial *
			 * version of RenderSphere, which is extremely ugly */
			if (pOverlay->m_Type == OVERLAY_SPHERE) {
				OverlaySphere_t *pSphere = static_cast<OverlaySphere_t *>(pOverlay);
				
				if (pSphere->a > 0) {
					RenderSphere(pSphere->vOrigin, pSphere->flRadius, pSphere->nTheta, pSphere->nPhi, Color(pSphere->r, pSphere->g, pSphere->b, pSphere->a), false);
				}
				
				RenderWireframeSphere(pSphere->vOrigin, pSphere->flRadius, pSphere->nTheta, pSphere->nPhi, Color(pSphere->r, pSphere->g, pSphere->b, pSphere->a), true);
				return;
			}
		}
		
		DETOUR_STATIC_CALL(CDebugOverlay_DrawOverlay)(pOverlay);
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Util:Overlay_Enhancements")
		{
			MOD_ADD_DETOUR_MEMBER(CDebugOverlay_Paint,               "[client] CDebugOverlay::Paint");
			MOD_ADD_DETOUR_MEMBER(CMatSystemSurface_DrawColoredText, "[client] CMatSystemSurface::DrawColoredText [va_list]");
			
			MOD_ADD_DETOUR_STATIC(CDebugOverlay_DrawOverlay, "[client] CDebugOverlay::DrawOverlay");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_util_overlay_enhancements", "0", FCVAR_NOTIFY,
		"Utility: overlay forwarding: client rendering enhancements",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
