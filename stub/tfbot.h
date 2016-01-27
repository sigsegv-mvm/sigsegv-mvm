#ifndef _INCLUDE_SIGSEGV_STUB_TFBOT_H_
#define _INCLUDE_SIGSEGV_STUB_TFBOT_H_


#include "prop.h"
#include "stub/tfplayer.h"


class ILocomotion;
class IBody;
class IVision;
class IIntention;


template<typename T>
class NextBotPlayer : public T {};

class CTFBot : public NextBotPlayer<CTFPlayer>
{
public:
	static const char *GetRTTIAddrName() { return "[RTTI] CTFBot"; }
	
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
	DEF_EXTRACT(int, m_nMission);
	
	static FuncThunk<ILocomotion * (*)(const CTFBot *)> ft_GetLocomotionInterface;
	static FuncThunk<IBody * (*)(const CTFBot *)>       ft_GetBodyInterface;
	static FuncThunk<IVision * (*)(const CTFBot *)>     ft_GetVisionInterface;
	static FuncThunk<IIntention * (*)(const CTFBot *)>  ft_GetIntentionInterface;
};


// TODO: ToTFBot
// (WARNING: DON'T ASSUME THAT DYNAMIC CASTS ARE SAFE!)


#endif
