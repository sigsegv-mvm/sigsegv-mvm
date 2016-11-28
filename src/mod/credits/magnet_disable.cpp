#include "mod.h"


namespace Mod_Credits_Magnet_Disable
{
	constexpr uint8_t s_Buf[] = {
		0xff, 0x90, 0x84, 0x03, 0x00, 0x00,
		0x84, 0xc0,
		0xff, 0x90, 0x84, 0x03, 0x00, 0x00,
		0x84, 0xc0,
		0x0f, 0x84, 0x14, 0xff, 0xff, 0xff,
		0x80, 0x7d, 0xcb, 0x00,
		0x0f, 0x84, 0x6c, 0x03, 0x00, 0x00,
		0x8b, 0x4d, 0xc0
		
	//	0xc7, 0x43, 0x3c, 0x00, 0x00, 0x02, 0x44, // +0000 mov dword ptr [ebx+0x3c],520.0f
	};
	
	struct CPatch_CTFPlayerShared_RadiusCurrencyCollectionCheck : public CPatch
	{
		CPatch_CTFPlayerShared_RadiusCurrencyCollectionCheck() : CPatch(sizeof(s_Buf)) {}
		
		#error need addr
		virtual const char *GetFuncName() const override { return "[client] CTFGameMovement::ProcessMovement"; }
	//	virtual uint32_t GetFuncOffMin() const override { return 0x0000; }
	//	virtual uint32_t GetFuncOffMax() const override { return 0x0100; } // @ 0x0046
		
		virtual bool GetVerifyInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
	//		buf.CopyFrom(s_Buf);
	//		
	//		return true;
		}
		
		virtual bool GetPatchInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
	//		/* NOP out the assignment of pMove->m_flMaxSpeed */
	//		buf.SetRange(0x00, 7, 0x90);
	//		mask.SetRange(0x00, 7, 0xff);
			
			return true;
		}
	};
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Credits:Magnet_Disable")
		{
			this->AddPatch(new CPatch_CTFPlayerShared_RadiusCurrencyCollectionCheck());
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_credits_magnet_disable", "0", FCVAR_NOTIFY,
		"Mod: disable scout's credit pull magnet effect",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
