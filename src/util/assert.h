#ifndef _INCLUDE_SIGSEGV_UTIL_ASSERT_H_
#define _INCLUDE_SIGSEGV_UTIL_ASSERT_H_


// TODO: non-fatal assert
// TODO: fatal assert

// dump backtrace in assert failure message

// in fatal assert:
// if not in main thread, abort()
// if in main thread, try to emergency-unload all mods (maybe on next frame?)
// make extension IsRunning return false thereafter with an error message


#endif
