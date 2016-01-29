#ifndef _INCLUDE_SIGSEGV_UTIL_RTTI_H_
#define _INCLUDE_SIGSEGV_UTIL_RTTI_H_


#include "abi.h"


#if defined __GNUC__
	typedef abi::__class_type_info rtti_t;
#elif defined _MSC_VER
	typedef _TypeDescriptor rtti_t;
#endif


namespace RTTI
{
	void PreLoad();
	
	const rtti_t *GetRTTI(const char *name);
	const void **GetVTable(const char *name);
	
#if defined __GNUC__
	template<class T> inline const rtti_t *GetRTTI()  { return RTTI::GetRTTI(typeid(T).name()); }
	template<class T> inline const void **GetVTable() { return RTTI::GetVTable(typeid(T).name()); }
#elif defined _MSC_VER
	template<class T> inline const rtti_t *GetRTTI()  { return RTTI::GetRTTI(typeid(T).raw_name()); }
	template<class T> inline const void **GetVTable() { return RTTI::GetVTable(typeid(T).raw_name()); }
#endif
}


template<class TO, class FROM>
inline TO rtti_cast(const FROM ptr)
{
	if (ptr == nullptr) {
		return nullptr;
	}
	
	auto rtti_from = RTTI::GetRTTI<typename std::remove_pointer<FROM>::type>();
	auto rtti_to   = RTTI::GetRTTI<typename std::remove_pointer<TO>::type>();
	
	assert(rtti_from != nullptr);
	assert(rtti_to   != nullptr);
	
#if defined __GNUC__
	void *result = abi::__dynamic_cast(ptr, rtti_from, rtti_to, -1);
#elif defined _MSC_VER
	void *result = __RTDynamicCast(ptr, 0, (void *)rtti_from, (void *)rtti_to, false);
#endif
	
	return reinterpret_cast<TO>(result);
}


#endif
