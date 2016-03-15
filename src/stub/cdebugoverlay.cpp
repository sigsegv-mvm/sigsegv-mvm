#include "stub/cdebugoverlay.h"
#include "link/link.h"


namespace CDebugOverlay
{
	static StaticFuncThunk<void, const Vector&, float, int, int, int, int, int, int, float> ft_AddSphereOverlay("[client] CDebugOverlay::AddSphereOverlay");
	void AddSphereOverlay(const Vector& vOrigin, float flRadius, int nTheta, int nPhi, int r, int g, int b, int a, float flDuration)
	{
		ft_AddSphereOverlay(vOrigin, flRadius, nTheta, nPhi, r, g, b, a, flDuration);
	}
}


#if 0
static IMaterial *GetWireframe()
{
	ft_InitializeStandardMaterials();
	static IMaterial *s_pWireframe = g_pMaterialSystem->FindMaterial("__utilWireframe", TEXTURE_GROUP_OTHER);
	return s_pWireframe;
}
static IMaterial *GetWireframeIgnoreZ()
{
	ft_InitializeStandardMaterials();
	static IMaterial *s_pWireframeIgnoreZ = g_pMaterialSystem->FindMaterial("__utilWireframeIgnoreZ", TEXTURE_GROUP_OTHER);
	return s_pWireframeIgnoreZ;
}
#endif


static StaticFuncThunk<void, const Vector&, float, int, int, Color, bool> ft_RenderWireframeSphere("[client] RenderWireframeSphere");
void RenderWireframeSphere(const Vector& vCenter, float flRadius, int nTheta, int nPhi, Color c, bool bZBuffer)
{
	ft_RenderWireframeSphere(vCenter, flRadius, nTheta, nPhi, c, bZBuffer);
}

static StaticFuncThunk<void, const Vector&, float, int, int, Color, IMaterial *> ft_RenderSphere("[client] RenderSphere [IMaterial *]");
void RenderSphere(const Vector& vCenter, float flRadius, int nTheta, int nPhi, Color c, IMaterial *pMaterial)
{
	ft_RenderSphere(vCenter, flRadius, nTheta, nPhi, c, pMaterial);
}

static GlobalThunk<IMaterial *> s_pVertexColor("[client] s_pVertexColor");
static GlobalThunk<IMaterial *> s_pVertexColorIgnoreZ("[client] s_pVertexColorIgnoreZ");
void RenderSphere(const Vector& vCenter, float flRadius, int nTheta, int nPhi, Color c, bool bZBuffer)
{
	RenderSphere(vCenter, flRadius, nTheta, nPhi, c, (bZBuffer ? s_pVertexColor : s_pVertexColorIgnoreZ));
//	RenderSphere(vCenter, flRadius, nTheta, nPhi, c, (bZBuffer ? GetWireframe() : GetWireframeIgnoreZ()));
}
