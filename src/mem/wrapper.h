#ifndef _INCLUDE_SIGSEGV_MEM_WRAPPER_H_
#define _INCLUDE_SIGSEGV_MEM_WRAPPER_H_


/* global symbols from wrapper.s, for access from C++ */
extern "C"
{
	extern uint8_t __wrapper_Begin;
	extern uint8_t __wrapper_MOV_FuncAddr_1;
	extern uint8_t __wrapper_CALL_Pre;
	extern uint8_t __wrapper_CALL_Inner;
	extern uint8_t __wrapper_MOV_FuncAddr_2;
	extern uint8_t __wrapper_CALL_Post;
	extern uint8_t __wrapper_End;
}


namespace Wrapper
{
	inline uint8_t *Base() { return &__wrapper_Begin; }
	
	inline ptrdiff_t Offset_MOV_FuncAddr_1() { return (&__wrapper_MOV_FuncAddr_1 - &__wrapper_Begin); }
	inline ptrdiff_t Offset_CALL_Pre()       { return (&__wrapper_CALL_Pre       - &__wrapper_Begin); }
	inline ptrdiff_t Offset_CALL_Inner()     { return (&__wrapper_CALL_Inner     - &__wrapper_Begin); }
	inline ptrdiff_t Offset_MOV_FuncAddr_2() { return (&__wrapper_MOV_FuncAddr_2 - &__wrapper_Begin); }
	inline ptrdiff_t Offset_CALL_Post()      { return (&__wrapper_CALL_Post      - &__wrapper_Begin); }
	
	inline size_t Size() { return (&__wrapper_End - &__wrapper_Begin); }
}


#endif
