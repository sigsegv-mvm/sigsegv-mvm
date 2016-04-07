#ifndef _INCLUDE_SIGSEGV_STUB_TFBOT_BEHAVIOR_H_
#define _INCLUDE_SIGSEGV_STUB_TFBOT_BEHAVIOR_H_


#include "re/nextbot.h"
#include "re/path.h"
#include "stub/tfbot.h"


class ActionStub : public Action<CTFBot>
{
public:
	virtual const char *GetName() const override { return nullptr; }
	
protected:
	ActionStub() {}
};


class CTFBotSeekAndDestroy : public ActionStub
{
public:
	CTFBotSeekAndDestroy() = delete;
	static CTFBotSeekAndDestroy *New(float duration = -1.0f);
	
private:
	uint32_t m_Dword0034;        // +0x0034
	uint32_t m_Dword0038;        // +0x0038
	PathFollower m_PathFollower; // +0x003c
	CountdownTimer m_ctUnknown1; // +0x4810
	CountdownTimer m_ctUnknown2; // +0x481c
};
SIZE_CHECK(CTFBotSeekAndDestroy, 0x4828);


class CTFBotMedicHeal : public ActionStub
{
public:
	CTFBotMedicHeal() = delete;
	static CTFBotMedicHeal *New();
	
private:
	ChasePath m_ChasePath;         // +0x0034
	CountdownTimer m_ctUnknown1;   // +0x4834
	CountdownTimer m_ctUnknown2;   // +0x4840
	CHandle<CTFPlayer> m_hPatient; // +0x484c
	Vector m_vecUnknown1;          // +0x4850
	CountdownTimer m_ctUnknown3;   // +0x485c
	uint32_t m_Dword4868;          // +0x4868
	CountdownTimer m_ctUnknown4;   // +0x486c
	PathFollower m_PathFollower;   // +0x4878
	Vector m_vecFollowPosition;    // +0x904c
};
SIZE_CHECK(CTFBotMedicHeal, 0x9058);


class CTFBotSniperLurk : public ActionStub
{
public:
	CTFBotSniperLurk() = delete;
	static CTFBotSniperLurk *New();
	
private:
	CountdownTimer m_ctUnknown1; // +0x0034
	CountdownTimer m_ctUnknown2; // +0x0040
	PathFollower m_PathFollower; // +0x004c
	uint8_t pad_4820[0x14];      // +0x4820
	CountdownTimer m_ctUnknown3; // +0x4834
	uint8_t pad_4840[0x1c];      // +0x4840
};
SIZE_CHECK(CTFBotSniperLurk, 0x485c);


class CTFBotSpyInfiltrate : public ActionStub
{
public:
	CTFBotSpyInfiltrate() = default;
	static CTFBotSpyInfiltrate *New();
	
private:
	CountdownTimer m_ctUnknown1; // +0x0034
	PathFollower m_PathFollower; // +0x0040
	uint32_t m_Dword4814;        // +0x4814
	CountdownTimer m_ctUnknown2; // +0x4818
	CountdownTimer m_ctUnknown3; // +0x4824
	uint32_t m_Dword4830;        // +0x4830
};
SIZE_CHECK(CTFBotSpyInfiltrate, 0x4834);


class CTFBotEngineerBuild : public ActionStub
{
public:
	CTFBotEngineerBuild() = default;
	static CTFBotEngineerBuild *New();
};
SIZE_CHECK(CTFBotEngineerBuild, 0x0034);


class CTFBotDead : public ActionStub
{
public:
	CTFBotDead() = default;
	static CTFBotDead *New();
	
private:
	IntervalTimer m_itDeathEpoch; // +0x0034
};
SIZE_CHECK(CTFBotDead, 0x0038);


class CTFBotMainAction : public ActionStub
{
	/* TODO */
};


#endif
