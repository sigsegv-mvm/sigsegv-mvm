#ifndef _INCLUDE_SIGSEGV_MEM_WRAPPER_H_
#define _INCLUDE_SIGSEGV_MEM_WRAPPER_H_


/* global labels from wrapper.s, for usage from C++ */
extern "C"
{
	extern uint8_t Wrapper_begin;
	extern uint8_t Wrapper_end;
	extern uint8_t Wrapper_push_func_addr_1;
	extern uint8_t Wrapper_call_wrapper_pre;
	extern uint8_t Wrapper_call_actual_func;
	extern uint8_t Wrapper_push_func_addr_2;
	extern uint8_t Wrapper_call_wrapper_post;
}


#endif
