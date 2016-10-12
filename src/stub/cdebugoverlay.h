#ifndef _INCLUDE_SIGSEGV_STUB_CDEBUGOVERLAY_H_
#define _INCLUDE_SIGSEGV_STUB_CDEBUGOVERLAY_H_


/* from engine/debugoverlay.cpp; only in the client version of the engine DLL */
namespace CDebugOverlay
{
	void AddSphereOverlay(const Vector& vOrigin, float flRadius, int nTheta, int nPhi, int r, int g, int b, int a, float flDuration);
}


IMaterial *GetMatWireframe();
IMaterial *GetMatWireframeIgnoreZ();
IMaterial *GetMatVertexColor();
IMaterial *GetMatVertexColorIgnoreZ();


/* from tier2/renderutils.cpp; in client DLL */
void RenderWireframeSphere(const Vector& vCenter, float flRadius, int nTheta, int nPhi, Color c, bool bZBuffer);

void RenderSphere(const Vector& vCenter, float flRadius, int nTheta, int nPhi, Color c, IMaterial *pMaterial);
void RenderSphere(const Vector& vCenter, float flRadius, int nTheta, int nPhi, Color c, bool bZBuffer);

//void DrawScreenSpaceRectangle(IMaterial *pMaterial, int nDestX, int nDestY, int nWidth, int nHeight,
//	float flSrcTexture0, float flSrcTextureY0, float flSrcTextureX1, float flSrcTextureY1,
//	int nSrcTextureWidth, int nSrcTextureHeight, void *pClientRenderable, int nXDice, int nYDice);


#endif
