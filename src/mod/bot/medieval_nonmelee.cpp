#include "mod.h"
#include "prop.h"
#include "stub/gamerules.h"


namespace Mod_Bot_Medieval_NonMelee
{
	constexpr uint8_t s_Buf[] = {
		0xa1, 0x00, 0x00, 0x00, 0x00,             // +0000  mov eax,[g_pGameRules]
		0x80, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, // +0005  cmp byte ptr [eax+m_bPlayingMedieval],0
		0x75, 0x00,                               // +000C  jnz +0xXX
	};
	
	struct IPatch_CTFBot_EquipRequiredWeapon : public IPatch
	{
		IPatch_CTFBot_EquipRequiredWeapon() : IPatch(sizeof(s_Buf)) {}
		
		virtual const char *GetFuncName() const override { return "CTFBot::EquipRequiredWeapon"; }
		
		virtual bool GetVerifyInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			buf.CopyFrom(s_Buf);
			
			int off_CTFGameRules_m_bPlayingMedieval;
			if (!Prop::FindOffset(off_CTFGameRules_m_bPlayingMedieval, "CTFGameRules", "m_bPlayingMedieval")) return false;
			
			buf.SetDword(0x00 + 1, (uint32_t)&g_pGameRules.GetRef());
			buf.SetDword(0x05 + 2, (uint32_t)off_CTFGameRules_m_bPlayingMedieval);
			
			mask.SetRange(0x0c + 1, 1, 0x00);
			
			return true;
		}
		
		virtual bool GetPatchInfo(ByteBuf& buf, ByteBuf& mask) const override
		{
			/* NOP out the conditional jump for Medieval mode */
			buf.SetRange(0x0c, 2, 0x90);
			mask.SetRange(0x0c, 2, 0xff);
			
			return true;
		}
	};
	
#if defined _LINUX
	
	struct CPatch_CTFBot_EquipRequiredWeapon : public IPatch_CTFBot_EquipRequiredWeapon
	{
		virtual uint32_t GetFuncOffMin() const override { return 0x0000; }
		virtual uint32_t GetFuncOffMax() const override { return 0x0100; } // @ 0x00b0
	};
	
#elif defined _WINDOWS
	
	struct CPatch_CTFBot_EquipRequiredWeapon : public IPatch_CTFBot_EquipRequiredWeapon
	{
		virtual uint32_t GetFuncOffMin() const override { return 0x0000; }
		virtual uint32_t GetFuncOffMax() const override { return 0x00c0; } // @ 0x0071
	};
	
#endif
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Bot:Medieval_NonMelee")
		{
			this->AddPatch(new CPatch_CTFBot_EquipRequiredWeapon());
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_bot_medieval_nonmelee", "0", FCVAR_NOTIFY,
		"Mod: allow bots in Medieval Mode to use weapons in non-melee slots",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
