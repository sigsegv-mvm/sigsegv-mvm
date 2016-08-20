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


class StandardMaterials
{
public:
	StandardMaterials()
	{
		{
			KeyValues *kv = new KeyValues("wireframe");
			kv->SetInt("$vertexcolor", 1);
			this->m_pWireframe = g_pMaterialSystem->CreateMaterial("__utilWireframe", kv);
			this->m_pWireframe->IncrementReferenceCount();
		}
		{
			KeyValues *kv = new KeyValues("wireframe");
			kv->SetInt("$vertexcolor", 1);
			kv->SetInt("$vertexalpha", 1);
			kv->SetInt("$ignorez",     1);
			this->m_pWireframeIgnoreZ = g_pMaterialSystem->CreateMaterial("__utilWireframeIgnoreZ", kv);
			this->m_pWireframeIgnoreZ->IncrementReferenceCount();
		}
		{
			KeyValues *kv = new KeyValues("unlitgeneric");
			kv->SetInt("$vertexcolor", 1);
			kv->SetInt("$vertexalpha", 1);
			this->m_pVertexColor = g_pMaterialSystem->CreateMaterial("__utilVertexColor", kv);
			this->m_pVertexColor->IncrementReferenceCount();
		}
		{
			KeyValues *kv = new KeyValues("unlitgeneric");
			kv->SetInt("$vertexcolor", 1);
			kv->SetInt("$vertexalpha", 1);
			kv->SetInt("$ignorez",     1);
			this->m_pVertexColorIgnoreZ = g_pMaterialSystem->CreateMaterial("__utilVertexColorIgnoreZ", kv);
			this->m_pVertexColorIgnoreZ->IncrementReferenceCount();
		}
		
		for (auto handle = g_pMaterialSystem->FirstMaterial(); handle != g_pMaterialSystem->InvalidMaterial();
			handle = g_pMaterialSystem->NextMaterial(handle)) {
			IMaterial *mat = g_pMaterialSystem->GetMaterial(handle);
			
			DevMsg("Material: \"%s\" \"%s\"\n", mat->GetName(), mat->GetTextureGroupName());
		}
	}
	~StandardMaterials()
	{
		if (this->m_pWireframe != nullptr) {
			this->m_pWireframe->DecrementReferenceCount();
			this->m_pWireframe = nullptr;
		}
		if (this->m_pWireframeIgnoreZ != nullptr) {
			this->m_pWireframeIgnoreZ->DecrementReferenceCount();
			this->m_pWireframeIgnoreZ = nullptr;
		}
		if (this->m_pVertexColor != nullptr) {
			this->m_pVertexColor->DecrementReferenceCount();
			this->m_pVertexColor = nullptr;
		}
		if (this->m_pVertexColorIgnoreZ != nullptr) {
			this->m_pVertexColorIgnoreZ->DecrementReferenceCount();
			this->m_pVertexColorIgnoreZ = nullptr;
		}
	}
	
	IMaterial *Wireframe() const
	{
		assert(this->m_pWireframe != nullptr);
		return this->m_pWireframe;
	}
	IMaterial *WireframeIgnoreZ() const
	{
		assert(this->m_pWireframeIgnoreZ != nullptr);
		return this->m_pWireframeIgnoreZ;
	}
	IMaterial *VertexColor() const
	{
		assert(this->m_pVertexColor != nullptr);
		return this->m_pVertexColor;
	}
	IMaterial *VertexColorIgnoreZ() const
	{
		assert(this->m_pVertexColorIgnoreZ != nullptr);
		return this->m_pVertexColorIgnoreZ;
	}
	
	static StandardMaterials& Get()
	{
		static StandardMaterials std;
		return std;
	}
	
private:
	IMaterial *m_pWireframe          = nullptr;
	IMaterial *m_pWireframeIgnoreZ   = nullptr;
	IMaterial *m_pVertexColor        = nullptr;
	IMaterial *m_pVertexColorIgnoreZ = nullptr;
};


IMaterial *GetWireframe()
{
	return StandardMaterials::Get().Wireframe();
}
IMaterial *GetWireframeIgnoreZ()
{
	return StandardMaterials::Get().WireframeIgnoreZ();
}
IMaterial *GetVertexColor()
{
	return StandardMaterials::Get().VertexColor();
}
IMaterial *GetVertexColorIgnoreZ()
{
	return StandardMaterials::Get().VertexColorIgnoreZ();
}


static StaticFuncThunk<void, const Vector&, float, int, int, Color, bool> ft_RenderWireframeSphere("[client] RenderWireframeSphere");
void RenderWireframeSphere(const Vector& vCenter, float flRadius, int nTheta, int nPhi, Color c, bool bZBuffer)
{
	ft_RenderWireframeSphere(vCenter, flRadius, nTheta, nPhi, c, bZBuffer);
}

static StaticFuncThunk<void, const Vector&, float, int, int, Color, IMaterial *> ft_RenderSphere("[client] RenderSphere [material]");
void RenderSphere(const Vector& vCenter, float flRadius, int nTheta, int nPhi, Color c, IMaterial *pMaterial)
{
	ft_RenderSphere(vCenter, flRadius, nTheta, nPhi, c, pMaterial);
}

void RenderSphere(const Vector& vCenter, float flRadius, int nTheta, int nPhi, Color c, bool bZBuffer)
{
	IMaterial *mat = (bZBuffer ? GetVertexColor() : GetVertexColorIgnoreZ());
	if (mat == nullptr) return;
	
	RenderSphere(vCenter, flRadius, nTheta, nPhi, c, mat);
}
