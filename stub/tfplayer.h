#ifndef _INCLUDE_SIGSEGV_STUB_TFPLAYER_H_
#define _INCLUDE_SIGSEGV_STUB_TFPLAYER_H_


#include "stub/baseanimating.h"


class CBaseCombatCharacter : public CBaseFlex {};
class CBasePlayer : public CBaseCombatCharacter {};
class CBaseMultiplayerPlayer : public CBasePlayer {};

class CTFPlayer : public CBaseMultiplayerPlayer
{
public:
	// TODO
	
private:
	// TODO
};


#endif
