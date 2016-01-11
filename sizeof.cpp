#include "common.h"
#include "re/nextbot.h"


#undef SIZE_CHECK
#define SIZE_CHECK(_type, _ideal) \
	printf("%s", #_type); \
	goto_col(50); printf("%04x", _ideal); \
	goto_col(60); printf("%04x\n", sizeof(_type))
#define OFFSET_CHECK(_type, _member, _ideal) \
	printf("%s::%s", #_type, #_member); \
	goto_col(50); printf("%04x", _ideal); \
	goto_col(60); printf("%04x\n", offsetof(_type, _member))


void goto_col(int col)
{
	printf("\r\e[%dC", col);
}

extern "C"
{
	void check_sizeof()
	{
		printf("TYPE");
		goto_col(50); printf("IDEAL");
		goto_col(60); printf("ACTUAL\n");
		
		SIZE_CHECK(CKnownEntity,               0x30); // 0x2d
		SIZE_CHECK(INextBotEventResponder,     0x04);
		SIZE_CHECK(IContextualQuery,           0x04);
		SIZE_CHECK(INextBotComponent,          0x14);
		SIZE_CHECK(IIntention,                 0x18);
		SIZE_CHECK(IBody,                      0x14);
		SIZE_CHECK(ILocomotion,                0x58);
		SIZE_CHECK(IVision,                    0xc4);
		SIZE_CHECK(INextBot,                   0x60);
		SIZE_CHECK(Behavior<CTFBot>,           0x50);
		SIZE_CHECK(Action<CTFBot>,             0x34); // 0x32
		SIZE_CHECK(ActionResult<CTFBot>,       0x0c);
		SIZE_CHECK(EventDesiredResult<CTFBot>, 0x10);
	}
}
