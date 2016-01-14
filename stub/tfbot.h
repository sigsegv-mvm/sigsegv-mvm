#ifndef _INCLUDE_SIGSEGV_STUB_TFBOT_H_
#define _INCLUDE_SIGSEGV_STUB_TFBOT_H_


#include "prop/prop.h"
#include "stub/tfplayer.h"
#include "mem/extract.h"


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
	
	virtual void GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		buf.CopyFrom(s_Buf_CTFBot_m_nMission);
		
		mask.SetRange(0x04 + 2, 1, 0x00);
		mask.SetRange(0x11 + 2, 4, 0x00);
	}
	
	virtual const char *GetFuncName() const override   { return "CTFBot::SetMission"; }
	virtual uint32_t GetFuncOffset() const override    { return 0x0000; }
	virtual uint32_t GetExtractOffset() const override { return 0x0011 + 2; }
};

#elif defined _WINDOWS

static constexpr uint8_t s_Buf_CTFBot_m_nMission[] = {
	0x00,
	// TODO
};

struct CExtract_CTFBot_m_nMission : public IExtract<int>
{
	CExtract_CTFBot_m_nMission() : IExtract<int>(sizeof(s_Buf_CTFBot_m_nMission)) {}
	
	virtual void GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		// TODO
	}
	
	virtual const char *GetFuncName() const override   { return "CTFBot::SetMission"; }
	virtual uint32_t GetFuncOffset() const override    { /* TODO */ return 0x0000; }
	virtual uint32_t GetExtractOffset() const override { /* TODO */ return 0x0011 + 2; }
};

#endif


class ILocomotion;
class IBody;
class IVision;
class IIntention;


template<typename T>
class NextBotPlayer : public T {};

class CTFBot : public NextBotPlayer<CTFPlayer>
{
public:
	int GetMission() const { return this->m_nMission; }
	
	/* thunk */
	ILocomotion *GetLocomotionInterface() const { return (*ft_GetLocomotionInterface)(this); }
	IBody *GetBodyInterface() const             { return (*ft_GetBodyInterface)      (this); }
	IVision *GetVisionInterface() const         { return (*ft_GetVisionInterface)    (this); }
	IIntention *GetIntentionInterface() const   { return (*ft_GetIntentionInterface) (this); }
	void PressFireButton(float duration = -1.0f);
	void ReleaseFireButton();
	void PressAltFireButton(float duration = -1.0f);
	void ReleaseAltFireButton();
	void PressMeleeButton(float duration = -1.0f);
	void ReleaseMeleeButton();
	void PressSpecialFireButton(float duration = -1.0f);
	void ReleaseSpecialFireButton();
	void PressUseButton(float duration = -1.0f);
	void ReleaseUseButton();
	void PressReloadButton(float duration = -1.0f);
	void ReleaseReloadButton();
	void PressForwardButton(float duration = -1.0f);
	void ReleaseForwardButton();
	void PressBackwardButton(float duration = -1.0f);
	void ReleaseBackwardButton();
	void PressLeftButton(float duration = -1.0f);
	void ReleaseLeftButton();
	void PressRightButton(float duration = -1.0f);
	void ReleaseRightButton();
	void PressJumpButton(float duration = -1.0f);
	void ReleaseJumpButton();
	void PressCrouchButton(float duration = -1.0f);
	void ReleaseCrouchButton();
	void PressWalkButton(float duration = -1.0f);
	void ReleaseWalkButton();
	void SetButtonScale(float forward, float side);
	
private:
	PROP_STR(CTFBot);
	
	PROP_EXTRACT(int, CTFBot, m_nMission, CExtract_CTFBot_m_nMission);
	
	static FuncThunk<ILocomotion * (*)(const CTFBot *)> ft_GetLocomotionInterface;
	static FuncThunk<IBody * (*)(const CTFBot *)>       ft_GetBodyInterface;
	static FuncThunk<IVision * (*)(const CTFBot *)>     ft_GetVisionInterface;
	static FuncThunk<IIntention * (*)(const CTFBot *)>  ft_GetIntentionInterface;
};


#endif
