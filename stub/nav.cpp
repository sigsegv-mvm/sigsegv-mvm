#include "stub/nav.h"


// CTFNavArea::m_nAttributes
// * CTFNavArea::IsValidForWanderingPopulation
// * CTFNavArea::IsBlocked
// * CTFNavArea::IsSentryGunHere
//   CTFNavMesh::ResetMeshAttributes
//   CTFNavMesh::RemoveAllMeshDecoration
//   CTFNavMesh::ComputeLegalBombDropAreas
//   CTFNavMesh::CollectAndMaskSpawnRoomExits
//   TF_EditClearAllAttributes
//   GetBombInfo


#if defined _LINUX

static constexpr uint8_t s_Buf[] = {
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
	CExtract_CTFNavArea_m_nAttributes() : IExtract<TFNavAttributeType>(sizeof(s_Buf)) {}
	
	virtual void GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		buf.CopyFrom(s_Buf);
		
		mask.SetRange(0x07 + 2, 4, 0x00);
	}
	
	virtual const char *GetFuncName() const override   { return "CTFNavArea::IsValidForWanderingPopulation"; }
	virtual uint32_t GetFuncOffset() const override    { return 0x0000; }
	virtual uint32_t GetExtractOffset() const override { return 0x0007 + 2; }
};

#elif defined _WIN32



#endif


CProp_Extract<CTFNavArea, TFNavAttributeType> CTFNavArea::m_nAttributes("CTFNavArea", "m_nAttributes", new CExtract_CTFNavArea_m_nAttributes());

FuncThunk<void (*)(CTFNavMesh *, CUtlVector<CBaseObject *> *, int)> CTFNavMesh::ft_CollectBuiltObjects("CTFNavMesh::CollectBuiltObjects");

GlobalThunk<CTFNavMesh *> TheNavMesh("TheNavMesh");
