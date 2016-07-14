#ifndef _INCLUDE_SIGSEGV_MOD_UTIL_VPROF_SHARED_H_
#define _INCLUDE_SIGSEGV_MOD_UTIL_VPROF_SHARED_H_


// data format:
// begin with 32-bit field containing length of the entire message
// then send bf_write stuff


enum VProfMsg : uint8_t
{
	VP_ROOT,
	VP_NODE,
	VP_DOWN,
	VP_UP,
	
	VP_MAX,
};


constexpr uint16_t VPROF_PORT = 30001;


constexpr size_t VPROF_DATA_LIMIT = 1024 * 1024;


#endif
