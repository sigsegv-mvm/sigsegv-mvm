#include "mod.h"


namespace Mod_Sniper_Head_Uncap
{
#if defined _LINUX
	
	constexpr uint8_t s_Buf[] = {
		0x83, 0xf8, 0x05, // +0000  cmp eax,5
		0x7f, 0x00,       // +0003  jg +0xXX
	};
	
	struct CPatch_CTFSniperRifleDecap_SniperRifleChargeRateMod : public IPatch
	{
		CPatch_CTFSniperRifleDecap_SniperRifleChargeRateMod() : IPatch(sizeof(s_Buf)) {}
		
		virtual const char *GetFuncName() const override { return "CTFSniperRifleDecap::SniperRifleChargeRateMod"; }
		virtual uint32_t GetFuncOffMin() const override  { return 0x0000; }
		virtual uint32_t GetFuncOffMax() const override  { return 0x0030; } // @ 0x001a
		
		virtual bool GetVerifyInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			buf.CopyFrom(s_Buf);
			
			mask.SetRange(0x03 + 1, 1, 0x00);
			
			return true;
		}
		
		virtual bool GetPatchInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			/* NOP out the conditional jump for heads > 5 */
			buf.SetRange(0x03, 2, 0x90);
			mask.SetRange(0x03, 2, 0xff);
			
			return true;
		}
	};
	
#elif defined _WINDOWS
	
	constexpr uint8_t s_Buf[] = {
		0x83, 0xbe, 0x00, 0x00, 0x00, 0x00, 0x06, // +0000  cmp dword ptr [esi+0xXXXXXXXX],6
		0x7c, 0x00,                               // +0007  jl +0xXX
		0xb8, 0x06, 0x00, 0x00, 0x00,             // +0009  mov eax,6
		0xeb, 0x00,                               // +000E  jmp +0xXX
	};
	
	struct CPatch_CTFSniperRifleDecap_SniperRifleChargeRateMod : public IPatch
	{
		CPatch_CTFSniperRifleDecap_SniperRifleChargeRateMod() : IPatch(sizeof(s_Buf)) {}
		
		virtual const char *GetFuncName() const override { return "CTFSniperRifleDecap::SniperRifleChargeRateMod"; }
		virtual uint32_t GetFuncOffMin() const override  { return 0x0000; }
		virtual uint32_t GetFuncOffMax() const override  { return 0x0060; } // @ 0x0043
		
		virtual bool GetVerifyInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			buf.CopyFrom(s_Buf);
			
			mask.SetRange(0x00 + 2, 4, 0x00);
			mask.SetRange(0x0e + 1, 1, 0x00);
			
			return true;
		}
		
		virtual bool GetPatchInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			/* always take the jump for heads < 6 */
			buf[0x07] = 0xeb;
			mask[0x07] = 0xff;
			
			return true;
		}
	};
	
#endif
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Sniper:Head_Uncap")
		{
			this->AddPatch(new CPatch_CTFSniperRifleDecap_SniperRifleChargeRateMod());
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_sniper_head_uncap", "0", FCVAR_NOTIFY,
		"Mod: remove the 6-head cap on the Bazaar Bargain",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
