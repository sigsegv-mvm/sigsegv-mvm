#include "mod.h"
#include "stub/cdebugoverlay.h"


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
	/* for OVERLAY_SPHERE, CDebugOverlay::DrawOverlay calls the IMaterial *
	 * version of RenderSphere, which is extremely ugly */
	DETOUR_DECL_STATIC(void, CDebugOverlay_DrawOverlay, OverlayBase_t *pOverlay)
	{
		if (pOverlay->m_Type == OVERLAY_SPHERE) {
			OverlaySphere_t *pSphere = static_cast<OverlaySphere_t *>(pOverlay);
			
			if (pSphere->a > 0) {
				RenderSphere(pSphere->vOrigin, pSphere->flRadius, pSphere->nTheta, pSphere->nPhi,
					Color(pSphere->r, pSphere->g, pSphere->b, pSphere->a), false);
			}
			
			RenderWireframeSphere(pSphere->vOrigin, pSphere->flRadius, pSphere->nTheta, pSphere->nPhi,
				Color(pSphere->r, pSphere->g, pSphere->b, pSphere->a), true);
		} else {
			DETOUR_STATIC_CALL(CDebugOverlay_DrawOverlay)(pOverlay);
		}
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Util:Overlay_Enhancements")
		{
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
