#ifndef _INCLUDE_SIGSEGV_STUB_TFBOT_H_
#define _INCLUDE_SIGSEGV_STUB_TFBOT_H_


#include "prop.h"
#include "stub/tfplayer.h"


template<typename T>
class NextBotPlayer : public T {};

class CTFBot : public NextBotPlayer<CTFPlayer>
{
public:
	int GetMission() const { return m_nMission.Get(this); }
	
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
	static CProp_Extract<CTFBot, int> m_nMission;
};


#endif
