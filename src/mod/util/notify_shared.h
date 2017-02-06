#ifndef _INCLUDE_SIGSEGV_MOD_UTIL_NOTIFY_SHARED_H_
#define _INCLUDE_SIGSEGV_MOD_UTIL_NOTIFY_SHARED_H_


enum NotifyType : uint8_t
{
	NT_NPRINTF  = 0,
	NT_NXPRINTF = 1,
};


constexpr uint16_t NOTIFY_PORT = 30002;


#endif
