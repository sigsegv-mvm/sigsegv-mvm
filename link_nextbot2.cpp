#include "common.h"


struct vtable
{
	uint32_t offset;
	abi::__class_type_info *typeinfo;
	void *vfuncs[0x1000];
};


extern "C"
{
	/* the definition of the vtable/RTTI symbols can't be in a translation unit
	 * that includes the actual class definition */
	
	
	extern void _ZN22INextBotEventResponderD0Ev(void *);
	extern void _ZN22INextBotEventResponderD2Ev(void *);
	extern void _ZN16IContextualQueryD0Ev      (void *);
	extern void _ZN16IContextualQueryD2Ev      (void *);
	
	/* typeinfo */
	abi::__class_type_info _ZTI22INextBotEventResponder("22INextBotEventResponder");
	abi::__class_type_info _ZTI16IContextualQuery("16IIContextualQuery");
	
	
	/* vtable */
	vtable _ZTV22INextBotEventResponder = {
		0x00000000,
		&_ZTI22INextBotEventResponder,
		{
			(void *)&_ZN22INextBotEventResponderD2Ev,
			(void *)&_ZN22INextBotEventResponderD0Ev,
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
		}
	};
	
	vtable _ZTV16IContextualQuery = {
		0x00000000,
		&_ZTI16IContextualQuery,
		{
			(void *)&_ZN16IContextualQueryD2Ev,
			(void *)&_ZN16IContextualQueryD0Ev,
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
			0x00000000, // TODO
		}
	};
}
