#include "stub/nav.h"


// CTFNavArea::m_nAttributes
// * CTFNavArea::IsValidForWanderingPopulation
// * CTFNavArea::IsBlocked
// * CTFNavMesh::IsSentryGunHere
//   CTFNavMesh::ResetMeshAttributes
//   CTFNavMesh::RemoveAllMeshDecoration
//   CTFNavMesh::ComputeLegalBombDropAreas
//   CTFNavMesh::CollectAndMaskSpawnRoomExits
//   TF_EditClearAllAttributes
//   GetBombInfo

#if defined _LINUX

static constexpr uint8_t s_Buf_CTFNavArea_m_nAttributes[] = {
	0x55,                                                       // +0000  push ebp
	0x89, 0xe5,                                                 // +0001  mov ebp,esp
	0x8b, 0x45, 0x08,                                           // +0003  mov eax,[ebp+this]
	0x5d,                                                       // +0006  pop ebp
	0xf7, 0x80, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x06, // +0007  test DWORD PTR [eax+0xVVVVVVVV],0x06000007
	0x0f, 0x94, 0xc0,                                           // +0011  setz al
	0xc3,                                                       // +0014  ret
};

struct CExtract_CTFNavArea_m_nAttributes : public IExtract<TFNavAttributeType>
{
	CExtract_CTFNavArea_m_nAttributes() : IExtract<TFNavAttributeType>(sizeof(s_Buf_CTFNavArea_m_nAttributes)) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		buf.CopyFrom(s_Buf_CTFNavArea_m_nAttributes);
		
		mask.SetRange(0x07 + 2, 4, 0x00);
		
		return true;
	}
	
	virtual const char *GetFuncName() const override   { return "CTFNavArea::IsValidForWanderingPopulation"; }
	virtual uint32_t GetFuncOffMin() const override    { return 0x0000; }
	virtual uint32_t GetFuncOffMax() const override    { return 0x0030; }
	virtual uint32_t GetExtractOffset() const override { return 0x0007 + 2; }
};

#elif defined _WINDOWS

static constexpr uint8_t s_Buf_CTFNavArea_m_nAttributes[] = {
	0x55,                               // +0000  push ebp
	0x8b, 0xec,                         // +0001  mov ebp,esp
	0x8b, 0x91, 0x00, 0x00, 0x00, 0x00, // +0003  mov edx,[ecx+0xVVVVVVVV]
};

struct CExtract_CTFNavArea_m_nAttributes : public IExtract<TFNavAttributeType>
{
	CExtract_CTFNavArea_m_nAttributes() : IExtract<TFNavAttributeType>(sizeof(s_Buf_CTFNavArea_m_nAttributes)) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		buf.CopyFrom(s_Buf_CTFNavArea_m_nAttributes);
		
		mask.SetRange(0x03 + 2, 4, 0x00);
		
		return true;
	}
	
	virtual const char *GetFuncName() const override   { return "CTFNavArea::IsBlocked"; }
	virtual uint32_t GetFuncOffMin() const override    { return 0x0000; }
	virtual uint32_t GetFuncOffMax() const override    { return 0x0030; }
	virtual uint32_t GetExtractOffset() const override { return 0x0003 + 2; }
};

#endif


MemberFuncThunk<const CNavArea *, void, Extent *>                 CNavArea::ft_GetExtent            ("CNavArea::GetExtent");
MemberFuncThunk<const CNavArea *, void, const Vector *, Vector *> CNavArea::ft_GetClosestPointOnArea("CNavArea::GetClosestPointOnArea");


IMPL_EXTRACT(TFNavAttributeType, CTFNavArea, m_nAttributes, new CExtract_CTFNavArea_m_nAttributes());


MemberFuncThunk<const CNavMesh *, CNavArea *, const Vector&, bool, float, bool, bool, int> CNavMesh::ft_GetNearestNavArea_vec                   ("CNavMesh::GetNearestNavArea [vec]");
MemberFuncThunk<const CNavMesh *, CNavArea *, CBaseEntity *, int, float>                   CNavMesh::ft_GetNearestNavArea_ent                   ("CNavMesh::GetNearestNavArea [ent]");
MemberFuncThunk<CNavMesh *, void, const Extent&, CUtlVector<CTFNavArea *> *>               CNavMesh::ft_CollectAreasOverlappingExtent_CTFNavArea("CNavMesh::CollectAreasOverlappingExtent<CTFNavArea>");


MemberFuncThunk<CTFNavMesh *, void, CUtlVector<CBaseObject *> *, int> CTFNavMesh::ft_CollectBuiltObjects("CTFNavMesh::CollectBuiltObjects");


GlobalThunk<CTFNavMesh *> TheNavMesh("TheNavMesh");


StaticFuncThunk<float, CNavArea *, CNavArea *, CTFBotPathCost&, float> ft_NavAreaTravelDistance_CTFBotPathCost("NavAreaTravelDistance<CTFBotPathCost>");
