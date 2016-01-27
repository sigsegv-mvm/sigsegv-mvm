#include "mod.h"


namespace Mod_SniperChargeUncap
{
#if defined _LINUX
	
	constexpr uint8_t s_Buf[] = {
		0xf3, 0x0f, 0x10, 0x45, 0x00,                   // +0000  movss xmm0,DWORD PTR [ebp-0xXX]
		0xf3, 0x0f, 0x58, 0x45, 0x00,                   // +0005  addss xmm0,DWORD PTR [ebp-0xXX]
		0xf3, 0x0f, 0x10, 0xc8,                         // +000A  movss xmm1,xmm0
		0x0f, 0x28, 0xc1,                               // +000E  movaps xmm0,xmm1
		0x0f, 0x57, 0xc9,                               // +0011  xorps xmm1,xmm1
		0xf3, 0x0f, 0x5f, 0xc1,                         // +0014  maxss xmm0,xmm1
		0xf3, 0x0f, 0x5d, 0x05, 0x00, 0x00, 0x00, 0x00, // +0018  minss xmm0,DWORD PTR [XXXXXXXX]
		0xf3, 0x0f, 0x11, 0x45, 0x00,                   // +0020  movss DWORD PTR [ebp-0xXX],xmm0
		0xf3, 0x0f, 0x59, 0x40, 0x10,                   // +0025  mulss xmm0,DWORD PTR [eax+0x10]
	};
	
	struct CPatch_UncapChargeRate_Common : public IPatch
	{
		CPatch_UncapChargeRate_Common() : IPatch(sizeof(s_Buf)) {}
		
		virtual void GetVerifyInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			buf.CopyFrom(s_Buf);
			
			mask.SetRange(0x05 + 4, 1, 0x00);
			mask.SetRange(0x00 + 4, 1, 0x00);
			mask.SetRange(0x18 + 4, 4, 0x00);
			mask.SetRange(0x20 + 4, 1, 0x00);
		}
		
		virtual void GetPatchInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			/* NOP out the MINSS instruction */
			buf.SetRange(0x18, 8, 0x90);
			mask.SetRange(0x18, 8, 0xff);
		}
	};
	
	struct CPatch_UncapChargeRate_CTFSniperRifle : public CPatch_UncapChargeRate_Common
	{
		virtual const char *GetFuncName() const override { return "CTFSniperRifle::ItemPostFrame"; }
		virtual uint32_t GetFuncOffMin() const override  { return 0x0000; }
		virtual uint32_t GetFuncOffMax() const override  { return 0x0380; } // @ 0x026a
	};
	
	struct CPatch_UncapChargeRate_CTFSniperRifleClassic : CPatch_UncapChargeRate_Common
	{
		virtual const char *GetFuncName() const override { return "CTFSniperRifleClassic::ItemPostFrame"; }
		virtual uint32_t GetFuncOffMin() const override  { return 0x0000; }
		virtual uint32_t GetFuncOffMax() const override  { return 0x0280; } // @ 0x0146
	};
	
#elif defined _WINDOWS
	
	constexpr uint8_t s_Buf[] = {
		0xa1, 0x00, 0x00, 0x00, 0x00,                   // +0000  mov eax,DWORD PTR [xxxxxxxx]
		0xf3, 0x0f, 0x10, 0x05, 0x00, 0x00, 0x00, 0x00, // +0005  movss xmm0,DWORD PTR [xxxxxxxx]
		0xf3, 0x0f, 0x10, 0x50, 0x10,                   // +000D  movss xmm2,DWORD PTR [eax+0x10]
		0xd9, 0x5d, 0x00,                               // +0012  fstp [ebp-0xXX]
		0xf3, 0x0f, 0x10, 0x4d, 0x00,                   // +0015  movss xmm1,[ebp-0xXX]
		0xf3, 0x0f, 0x5f, 0xc8,                         // +001A  maxss xmm1,xmm0
		0xf3, 0x0f, 0x10, 0x05, 0x00, 0x00, 0x00, 0x00, // +001E  movss xmm0,DWORD PTR [xxxxxxxx]
		0xf3, 0x0f, 0x5d, 0xc8,                         // +0026  minss xmm1,xmm0
		0xf3, 0x0f, 0x10, 0x05, 0x00, 0x00, 0x00, 0x00, // +002A  movss xmm0,DWORD PTR [xxxxxxxx]
	};
	
	struct CPatch_UncapChargeRate_Common : public IPatch
	{
		CPatch_UncapChargeRate_Common() : IPatch(sizeof(s_Buf)) {}
		
		virtual void GetVerifyInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			buf.CopyFrom(s_Buf);
			
			mask.SetRange(0x00 + 1, 4, 0x00);
			mask.SetRange(0x05 + 4, 4, 0x00);
			mask.SetRange(0x12 + 2, 1, 0x00);
			mask.SetRange(0x15 + 4, 1, 0x00);
			mask.SetRange(0x1e + 4, 4, 0x00);
			mask.SetRange(0x2a + 4, 4, 0x00);
		}
		
		virtual void GetPatchInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			/* NOP out the MINSS instruction */
			buf.SetRange(0x26, 4, 0x90);
			mask.SetRange(0x26, 4, 0xff);
		}
	};
	
	struct CPatch_UncapChargeRate_CTFSniperRifle : public CPatch_UncapChargeRate_Common
	{
		virtual const char *GetFuncName() const override { return "CTFSniperRifle::ItemPostFrame"; }
		virtual uint32_t GetFuncOffMin() const override  { return 0x0000; }
		virtual uint32_t GetFuncOffMax() const override  { return 0x0200; } // @ 0x16e
	};
	
	struct CPatch_UncapChargeRate_CTFSniperRifleClassic : CPatch_UncapChargeRate_Common
	{
		virtual const char *GetFuncName() const override { return "CTFSniperRifleClassic::ItemPostFrame"; }
		virtual uint32_t GetFuncOffMin() const override  { return 0x0000; }
		virtual uint32_t GetFuncOffMax() const override  { return 0x0200; } // @ 0x162
	};
	
#endif
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("SniperChargeUncap")
		{
			this->AddPatch(new CPatch_UncapChargeRate_CTFSniperRifle());
			this->AddPatch(new CPatch_UncapChargeRate_CTFSniperRifleClassic());
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllPatches(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sigsegv_sniperchargeuncap_enable", "0", FCVAR_NOTIFY,
		"Mod: remove the 200 percent upper limit on sniper rifle charge rate",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
