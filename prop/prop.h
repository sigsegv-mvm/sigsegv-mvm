#ifndef _INCLUDE_SIGSEGV_PROP_PROP_H_
#define _INCLUDE_SIGSEGV_PROP_PROP_H_


#include "prop/sendprop.h"
#include "prop/datamap.h"
#include "prop/extract.h"


#define PROP_STR(str) static constexpr const char *const _##str = #str

#define PROP_SENDPROP(T, C, P)   PROP_STR(P); CProp_SendProp<T, &_##C, &_##P> P
#define PROP_DATAMAP(T, C, P)    PROP_STR(P); CProp_DataMap<T, &_##C, &_##P> P
#define PROP_EXTRACT(T, C, P, X) PROP_STR(P); CProp_Extract<T, &_##C, &_##P, X> P


#endif
