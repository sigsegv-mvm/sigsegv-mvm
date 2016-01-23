#include "stub/tfbot.h"


#if defined _LINUX

static constexpr uint8_t s_Buf_CTFBot_m_nMission[] = {
	0x55,                               // +0000  push ebp
	0x89, 0xe5,                         // +0001  mov ebp,esp
	0x53,                               // +0003  push ebx
	0x83, 0xec, 0x00,                   // +0004  sub esp,0xXX
	0x8b, 0x5d, 0x08,                   // +0007  mov ebx,DWORD PTR [ebp+0x8]
	0x8b, 0x45, 0x0c,                   // +000A  mov eax,DWORD PTR [ebp+0xc]
	0x80, 0x7d, 0x10, 0x00,             // +000D  cmp BYTE PTR [ebp+0x10],0x0
	0x8b, 0x93, 0x00, 0x00, 0x00, 0x00, // +0011  mov edx,DWORD PTR [ebx+0xVVVVVVVV]
};

struct CExtract_CTFBot_m_nMission : public IExtract<int>
{
	CExtract_CTFBot_m_nMission() : IExtract<int>(sizeof(s_Buf_CTFBot_m_nMission)) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		buf.CopyFrom(s_Buf_CTFBot_m_nMission);
		
		mask.SetRange(0x04 + 2, 1, 0x00);
		mask.SetRange(0x11 + 2, 4, 0x00);
		
		return true;
	}
	
	virtual const char *GetFuncName() const override   { return "CTFBot::SetMission"; }
	virtual uint32_t GetFuncOffMin() const override    { return 0x0000; }
	virtual uint32_t GetFuncOffMax() const override    { return 0x0000; }
	virtual uint32_t GetExtractOffset() const override { return 0x0011 + 2; }
};

#elif defined _WINDOWS

static constexpr uint8_t s_Buf_CTFBot_m_nMission[] = {
	0x83, 0xbf, 0x00, 0x00, 0x00, 0x00, 0x02, // +0000  cmp dword ptr [edi+0xVVVVVVVV],0x2
	0x75, 0x00,                               // +0007  jnz 0xXX
	0x68, 0x00, 0x00, 0x00, 0x00,             // +0009  push offset "mission_sentry_buster"
};

struct CExtract_CTFBot_m_nMission : public IExtract<int>
{
	CExtract_CTFBot_m_nMission() : IExtract<int>(sizeof(s_Buf_CTFBot_m_nMission)) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		const char *str = Scan::FindUniqueConstStr("mission_sentry_buster");
		if (str == nullptr) return false;
		
		buf.CopyFrom(s_Buf_CTFBot_m_nMission);
		buf.SetDword(0x09 + 1, (uint32_t)str);
		
		mask.SetRange(0x00 + 2, 4, 0x00);
		mask.SetRange(0x07 + 1, 1, 0x00);
		
		return true;
	}
	
	virtual const char *GetFuncName() const override   { return "CFuncNavCost::IsApplicableTo"; }
	virtual uint32_t GetFuncOffMin() const override    { return 0x0000; }
	virtual uint32_t GetFuncOffMax() const override    { return 0x0200; }
	virtual uint32_t GetExtractOffset() const override { return 0x0000 + 2; }
};

#endif


IMPL_EXTRACT(int, CTFBot, m_nMission, new CExtract_CTFBot_m_nMission());

FuncThunk<ILocomotion * (*)(const CTFBot *)> CTFBot::ft_GetLocomotionInterface("CTFBot::GetLocomotionInterface");
FuncThunk<IBody *       (*)(const CTFBot *)> CTFBot::ft_GetBodyInterface      ("CTFBot::GetBodyInterface");
FuncThunk<IVision *     (*)(const CTFBot *)> CTFBot::ft_GetVisionInterface    ("CTFBot::GetVisionInterface");
FuncThunk<IIntention *  (*)(const CTFBot *)> CTFBot::ft_GetIntentionInterface ("CTFBot::GetIntentionInterface");
