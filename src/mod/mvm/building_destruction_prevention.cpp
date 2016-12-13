#include "mod.h"


namespace Mod_MvM_Building_Destruction_Prevention
{
	constexpr uint8_t s_Buf_CBaseObject_UpdateOnRemove[] = {
		0xa1, 0x40, 0x34, 0x6b, 0x01,       // +0000  mov eax,ds:IBaseObjectAutoList::m_IBaseObjectAutoListAutoList
		0x8b, 0x04, 0xb0,                   // +0005  mov eax,[eax+esi*4]
		0x8d, 0xb8, 0x9c, 0xf7, 0xff, 0xff, // +0008  lea edi,[eax-0x864]
		0x85, 0xc0,                         // +000E  test eax,eax
		0xb8, 0x00, 0x00, 0x00, 0x00,       // +0010  mov eax,0x00000000
		0x0f, 0x44, 0xf8,                   // +0015  cmovz edi,eax
		0x39, 0xdf,                         // +0018  cmp edi,ebx
		0x74, 0xd9,                         // +001A  jz -0x27
		
		0x8b, 0x03,                         // +001C  mov eax,[ebx]
		0x89, 0x1c, 0x24,                   // +001E  mov [esp],ebx
		0xff, 0x90, 0x7c, 0x05, 0x00, 0x00, // +0021  call dword ptr [eax+GetType]
		
		0x89, 0x45, 0xe4,                   // +0027  mov [ebp-0x1c],eax
		0x8b, 0x07,                         // +002A  mov eax,[edi]
		0x89, 0x3c, 0x24,                   // +002C  mov [esp],edi
		0xff, 0x90, 0x7c, 0x05, 0x00, 0x00, // +002F  call dword ptr [eax+GetType]
		
		0x39, 0x45, 0xe4,                   // +0035  cmp [ebp-0x1c],eax
		0x75, 0xbb,                         // +0038  jnz -0x45
		0x8b, 0x07,                         // +003A  mov eax,[edi]
		0x89, 0x3c, 0x24,                   // +003C  mov [esp],edi
		0xff, 0x90, 0x78, 0x05, 0x00, 0x00, // +003F  call dword ptr [eax+IsDying]
		
		0x84, 0xc0,                         // +0045  test al,al
		0x75, 0xac,                         // +0047  jnz -0x54
		0x8b, 0x07,                         // +0049  mov eax,[edi]
		0x89, 0x3c, 0x24,                   // +004B  mov [esp],edi
		0xff, 0x90, 0x80, 0x05, 0x00, 0x00, // +004E  call dword ptr [eax+GetBuilder]
		
		0x85, 0xc0,                         // +0054  test eax,eax
		0x75, 0x9d,                         // +0056  jnz -0x63
		0x8b, 0x07,                         // +0058  mov eax,[edi]
		0x89, 0x3c, 0x24,                   // +005A  mov [esp],edi
		0xff, 0x90, 0x64, 0x05, 0x00, 0x00, // +005D  call dword ptr [eax+DetonateObject]
		
		0xeb, 0x90,                         // +0063  jmp -0x70
	};
	
	struct CPatch_CBaseObject_UpdateOnRemove : public CPatch
	{
		CPatch_CBaseObject_UpdateOnRemove() : CPatch(sizeof(s_Buf_CBaseObject_UpdateOnRemove)) {}
		
		virtual const char *GetFuncName() const override { return "CBaseObject::UpdateOnRemove"; }
		virtual uint32_t GetFuncOffMin() const override  { return 0x0000; }
		virtual uint32_t GetFuncOffMax() const override  { return 0x0130; } // ServerLinux 20161122b: +0x00c3
		
		virtual bool GetVerifyInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			buf.CopyFrom(s_Buf_CBaseObject_UpdateOnRemove);
			
			void *addr__IBaseObjectAutoList_m_IBaseObjectAutoListAutoList = AddrManager::GetAddr("IBaseObjectAutoList::m_IBaseObjectAutoListAutoList");
			if (addr__IBaseObjectAutoList_m_IBaseObjectAutoListAutoList == nullptr) return false;
			
			buf.SetDword(0x00 + 1, (uintptr_t)addr__IBaseObjectAutoList_m_IBaseObjectAutoListAutoList);
			
			mask.SetRange(0x08 + 2, 4, 0x00);
			mask.SetRange(0x1a + 0, 1, 0x00);
			
			mask.SetRange(0x21 + 2, 4, 0x00);
			
			mask.SetRange(0x27 + 2, 1, 0x00);
			mask.SetRange(0x2f + 2, 4, 0x00);
			
			mask.SetRange(0x35 + 2, 1, 0x00);
			mask.SetRange(0x3f + 2, 4, 0x00);
			
			mask.SetRange(0x47 + 1, 1, 0x00);
			mask.SetRange(0x4e + 2, 4, 0x00);
			
			mask.SetRange(0x56 + 1, 1, 0x00);
			mask.SetRange(0x5d + 2, 4, 0x00);
			
			mask.SetRange(0x63 + 1, 1, 0x00);
			
			return true;
		}
		
		virtual bool GetPatchInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			/* unconditionally skip the unwanted section of code (as if every
			 * CBaseObject in the autolist is nullptr) */
			buf [0x1a] = 0xeb;
			mask[0x1a] = 0xff;
			
			return true;
		}
	};
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("MvM:Building_Destruction_Prevention")
		{
			this->AddPatch(new CPatch_CBaseObject_UpdateOnRemove());
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_mvm_building_destruction_prevention", "0", FCVAR_NOTIFY,
		"Mod: disable some questionable code that causes unowned-building-destruction chain reactions in MvM mode",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
