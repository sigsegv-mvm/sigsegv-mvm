#ifndef _INCLUDE_SIGSEGV_UTIL_RTTI_H_
#define _INCLUDE_SIGSEGV_UTIL_RTTI_H_


#include "abi.h"


#if defined __GNUC__
typedef abi::__class_type_info rtti_t;
#elif defined _MSC_VER
typedef _TypeDescriptor rtti_t;
#endif


#if defined __GNUC__
template<class T> inline const char *TypeName() { return typeid(T).name(); }
#elif defined _MSC_VER
template<class T> inline const char *TypeName() { return typeid(T).raw_name(); }
#endif


namespace RTTI
{
	void PreLoad();
	
	const rtti_t *GetRTTI(const char *name);
	const void **GetVTable(const char *name);
	
	template<class T> const rtti_t *GetRTTI()  { return GetRTTI(TypeName<T>()); }
	template<class T> const void **GetVTable() { return GetVTable(TypeName<T>()); }
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
	/* GCC's __dynamic_cast is grumpy and won't do up-casts at runtime, so we
	 * have to manually take care of up-casting ourselves */
	void *result = (void *)ptr;
	if (!static_cast<const std::type_info *>(rtti_from)->__do_upcast(rtti_to, &result)) {
		result = abi::__dynamic_cast(result, rtti_from, rtti_to, -1);
	}
#elif defined _MSC_VER
	/* MSVC's __RTDynamicCast will happily do runtime up-casts and down-casts */
	void *result = __RTDynamicCast((void *)ptr, 0, (void *)rtti_from, (void *)rtti_to, false);
#endif
	
	return reinterpret_cast<TO>(result);
}


#endif
