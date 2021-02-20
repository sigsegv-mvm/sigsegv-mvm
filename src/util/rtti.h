#ifndef _INCLUDE_SIGSEGV_UTIL_RTTI_H_
#define _INCLUDE_SIGSEGV_UTIL_RTTI_H_


#include "abi.h"
#include "util/demangle.h"


// using this for now to gate off unfinished code so that we can actually have usable builds...
#define RTTI_STATIC_CAST_ENABLE 0


#if defined __clang__
#error TODO
#elif defined __GNUC__
typedef abi::__class_type_info rtti_t;
#elif defined _MSC_VER
typedef _TypeDescriptor rtti_t;
#endif


#if defined __clang__
#error TODO
#elif defined __GNUC__
#define TYPEID_RAW_NAME(typeinfo) typeinfo.name()
#elif defined _MSC_VER
#define TYPEID_RAW_NAME(typeinfo) typeinfo.raw_name()
#endif

template<typename T>
const char *TypeName(bool demangle = false)
{
	/* the standard says typeid(type) shouldn't ever actually throw */
	try {
		const std::type_info& typeinfo = typeid(T);
		
		if (demangle) {
			return DemangleTypeName(typeinfo);
		} else {
			return TYPEID_RAW_NAME(typeinfo);
		}
	} catch (const std::bad_typeid& e) {
		Msg("%s: caught std::bad_typeid: %s\n", __PRETTY_FUNCTION__, e.what());
		return "<bad_typeid>";
	}
}

template<typename T>
const char *TypeName(T *t, bool demangle = false)
{
	/* the standard says typeid(expression) will throw if expression is nullptr */
	try {
		const std::type_info& typeinfo = typeid(*t);
		
		if (demangle) {
			return DemangleTypeName(typeinfo);
		} else {
			return TYPEID_RAW_NAME(typeinfo);
		}
	} catch (const std::bad_typeid& e) {
		Msg("%s: with 0x%08X: caught std::bad_typeid: %s\n", __PRETTY_FUNCTION__, (uintptr_t)t, e.what());
		return "<bad_typeid>";
	}
}

#undef TYPEID_RAW_NAME


namespace RTTI
{
	void PreLoad();
	
	const rtti_t *GetRTTI  (const char *name);
	const void  **GetVTable(const char *name);
	
	template<typename T> const rtti_t *GetRTTI()   { return GetRTTI  (TypeName<T>()); }
	template<typename T> const void  **GetVTable() { return GetVTable(TypeName<T>()); }
	
	
#if RTTI_STATIC_CAST_ENABLE
// TODO: actually implement rtti_scast for MSVC
#if defined __GNUC__ && !defined __CLANG__
	
	ptrdiff_t CalcStaticCastPtrDiff(const rtti_t *from, const rtti_t *to);
	
	
	/* this class is statically initialized by RTTI::StaticCastInfo<TO_PTR, FROM_PTR> to register its Init func,
	 * so that RTTI::PreLoad can call it and pre-compute the information for all future rtti_scast's */
	class StaticCastRegistrar
	{
	public:
		using InitFunc_t = void (*)();
		
		StaticCastRegistrar(InitFunc_t init_func) : m_MyInitFunc(init_func)
		{
			/* ensure no init funcs are registered multiple times */
			// TODO: could we have problems if, say, MSVC decides to COMDAT-fold some of our init funcs into the same addr?
			assert(s_InitFuncs.insert(init_func).second);
		}
		
		StaticCastRegistrar(StaticCastRegistrar&) = delete;
		
		~StaticCastRegistrar()
		{
			/* ensure that our init func was registered previously */
			assert(s_InitFuncs.erase(this->m_MyInitFunc) == 1);
		}
		
		static void InitAll()
		{
			for (const auto& init_func : s_InitFuncs) {
				(*init_func)();
			}
		}
		
	private:
		InitFunc_t m_MyInitFunc;
		
		static inline std::unordered_set<InitFunc_t> s_InitFuncs;
	};
	
	
	/* add a class template to this mess so that we can do non-lazy initialization of the static cast offsets:
	 * when the function rtti_scast<TO_PTR, FROM_PTR> is template-instantiated, it'll force the instantiation of
	 * RTTI::StaticCastInfo<TO_PTR, FROM_PTR>; and this allows us to put a "static constructor" in each individual
	 * RTTI::StaticCastInfo<TO_PTR, FROM_PTR> instantiation, which registers itself, so that RTTI::PreLoad will know
	 * about all rtti_scast type pairs that might possibly be done in the future and can precompute the offset for each
	 */
	template<typename TO_PTR, typename FROM_PTR/*,
		typename = std::enable_if_t<std::is_pointer_v<  TO_PTR>>,
		typename = std::enable_if_t<std::is_pointer_v<FROM_PTR>>*/>
	class StaticCastInfo
	{
	public:
		using FROM = std::remove_pointer_t<FROM_PTR>;
		using   TO = std::remove_pointer_t<  TO_PTR>;
		
		/* this is a purely static class (which, coincidentally, is used for "static" casts) */
		 StaticCastInfo()                = delete;
		 StaticCastInfo(StaticCastInfo&) = delete;
		~StaticCastInfo()                = delete;
		
		static ptrdiff_t GetPtrDiff()
		{
#ifdef _MSC_VER
#error TODO: check that our trick here works correctly in MSVC...
#endif
			// STOP ELIDING MY STATIC CONSTRUCTOR GOD DAMMIT
			void *volatile __fuck_you_compiler = &StaticCastInfo<TO_PTR, FROM_PTR>::s_Register;
			
			assert(s_Initialized);
			return s_PtrDiff;
		}
		
	private:
		static void Init()
		{
			auto rtti_from = GetRTTI<FROM>(); assert(rtti_from != nullptr);
			auto rtti_to   = GetRTTI<  TO>(); assert(rtti_to   != nullptr);
			
			s_PtrDiff = CalcStaticCastPtrDiff(rtti_from, rtti_to);
			s_Initialized = true;
			
			// REMOVE ME
			Msg("================================================================================\n");
			Msg("%s\n", __PRETTY_FUNCTION__);
			Msg("s_PtrDiff = %c0x%04X\n", (s_PtrDiff == 0 ? ' ' : (s_PtrDiff > 0 ? '+' : '-')), (s_PtrDiff >= 0 ? s_PtrDiff : -s_PtrDiff));
			Msg("================================================================================\n");
		}
		
		static inline bool s_Initialized = false;
		static inline ptrdiff_t s_PtrDiff;
		
		/* make sure we are registered for early ptr diff computation */
		static inline StaticCastRegistrar s_Register{&Init};
	};
	
#endif
#endif
}


/* it'd be nice to use SFINAE up here in the template, but it adds garbage to __PRETTY_FUNCTION__ */
template<typename TO_PTR, typename FROM_PTR/*,
	typename = std::enable_if_t<std::is_pointer_v<  TO_PTR>>,
	typename = std::enable_if_t<std::is_pointer_v<FROM_PTR>>*/>
inline TO_PTR rtti_dcast(const FROM_PTR ptr)
{
	static_assert(std::is_pointer_v<FROM_PTR>, "rtti_dcast: FROM_PTR isn't a pointer type");
	static_assert(std::is_pointer_v<  TO_PTR>, "rtti_dcast: TO_PTR isn't a pointer type");
	
	using FROM = std::remove_pointer_t<FROM_PTR>;
	using   TO = std::remove_pointer_t<  TO_PTR>;
	
	if (ptr == nullptr) {
		return nullptr;
	}
	
	auto rtti_from = RTTI::GetRTTI<FROM>(); assert(rtti_from != nullptr);
	auto rtti_to   = RTTI::GetRTTI<  TO>(); assert(rtti_to   != nullptr);
	
#if defined __clang__
	#error TODO
#elif defined __GNUC__
	/* GCC's __dynamic_cast is grumpy and won't do up-casts at runtime, so we
	 * have to manually take care of up-casting ourselves */
	void *result = (void *)ptr;
	if (static_cast<const std::type_info *>(rtti_from)->__do_upcast(rtti_to, &result)) {
	//	Msg(">>>>>> rtti_dcast: __do_upcast successful\n");
	#if 0 // THE CODE HERE IS WRONG WRONG WRONG WRONG WRONG WRONG WRONG WRONG!!
		/* __do_upcast actually adjusts us the wrong way, believe it or not, so we have to double-undo the adjustment */
		ptrdiff_t diff = (intptr_t)result - (intptr_t)ptr;
		result = (void *)((uintptr_t)result - (2 * diff));
	#endif
	} else {
	//	Msg(">>>>>> rtti_dcast: __do_upcast failed, trying __dynamic_cast [ptr: 0x%08X \"%s\"] [FROM: \"%s\"] [TO: \"%s\"]\n",
	//		(uintptr_t)ptr, TypeName<FROM>(ptr), TypeName<FROM>(), TypeName<TO>());
		result = abi::__dynamic_cast(result, rtti_from, rtti_to, -1);
	//	if (result == nullptr) {
	//		Msg(">>>>>>             __dynamic_cast returned nullptr\n");
	//	} else if (result == (void *)ptr) {
	//		Msg(">>>>>>             __dynamic_cast returned unmodified ptr: 0x%08X [\"%s\"]\n",
	//			(uintptr_t)result, TypeName(reinterpret_cast<TO_PTR>(result)));
	//	} else {
	//		ptrdiff_t delta = ((intptr_t)result - (intptr_t)ptr);
	//		Msg(">>>>>>             __dynamic_cast returned adjusted ptr: 0x%08X (%c0x%04X) [\"%s\"]\n",
	//			(uintptr_t)result, (delta > 0 ? '+' : '-'), delta, TypeName(reinterpret_cast<TO_PTR>(result)));
	//	}
	}
#elif defined _MSC_VER
	/* MSVC's __RTDynamicCast will happily do runtime up-casts and down-casts */
	void *result = __RTDynamicCast((void *)ptr, 0, (void *)rtti_from, (void *)rtti_to, false);
#endif
	
	return reinterpret_cast<TO_PTR>(result);
}


#if RTTI_STATIC_CAST_ENABLE && defined __GNUC__ && !defined __clang__

/* this should be faster than rtti_dcast: we always ASSUME that TO_PTR and FROM_PTR are convertible;
 * and we use typeinfo statically, ONE TIME (on the first rtti_scast of a particular <TO_PTR, FROM_PTR> pair),
 * to determine the ptr diff to use for all future rtti_scast's of that <TO_PTR, FROM_PTR> pair
 * 
 * VERY ROUGHLY SPEAKING,
 * dynamic_cast <==> rtti_dcast (aka rtti_cast)
 *  static_cast <==> rtti_scast
 */
template<typename TO_PTR, typename FROM_PTR/*,
	typename = std::enable_if_t<std::is_pointer_v<  TO_PTR>>,
	typename = std::enable_if_t<std::is_pointer_v<FROM_PTR>>*/>
inline TO_PTR rtti_scast(const FROM_PTR ptr)
{
	static_assert(std::is_pointer_v<FROM_PTR>, "rtti_scast: FROM_PTR isn't a pointer type");
	static_assert(std::is_pointer_v<  TO_PTR>, "rtti_scast: TO_PTR isn't a pointer type");
	
	using FROM = std::remove_pointer_t<FROM_PTR>;
	using   TO = std::remove_pointer_t<  TO_PTR>;
	
	if (ptr == nullptr) {
		return nullptr;
	}
	
	/* ALTERNATIVE IMPLEMENTATION: ditch all the RTTIStaticCastInfo and RTTIStaticCastRegister stuff, and instead just
	 * do something like this: (lazily computes the ptr diff on demand, the first time it's needed)
	 * static ptrdiff_t ptr_diff = []{ ... do the computation right here ... };
	 */
	
	static ptrdiff_t ptr_diff = RTTI::StaticCastInfo<TO_PTR, FROM_PTR>::GetPtrDiff();
	return reinterpret_cast<TO_PTR>((void *)((uintptr_t)ptr + ptr_diff));
}

// TODO: actually implement rtti_scast for MSVC
#else

// stand-in replacement for rtti_scast for !RTTI_STATIC_CAST_ENABLE
template<typename TO_PTR, typename FROM_PTR>
inline TO_PTR rtti_scast(const FROM_PTR ptr)
{
	if (ptr == nullptr) return nullptr;
	auto result = rtti_dcast<TO_PTR, FROM_PTR>(ptr);
	assert(result != nullptr);
	return result;
}

#endif


/* compat: before rtti_scast was invented, rtti_dcast was just called rtti_cast */
#define rtti_cast rtti_dcast
//template<TO_PTR, FROM_PTR> inline TO_PTR rtti_cast(const FROM_PTR ptr) { return rtti_dcast<TO_PTR, FROM_PTR>(ptr); }


#if 0
template<class TO, class FROM>
inline TO __fastcall jit_cast(const FROM ptr);

template<class TO, class FROM>
inline TO __fastcall jit_cast(const FROM ptr)
{
	
#if defined __clang__
	#error TODO
#elif defined __GNUC__
	__asm__ volatile ("nop; nop; nop; nop; nop" : : : "memory");
	
	// INITIAL: nop pad
	// LATER:   compare with nullptr and conditionally do the add/subtract adjustment
#elif defined _MSC_VER
	__asm
	{
		nop
		nop
		nop
		nop
		nop
	}
	
	// INITIAL: nop pad
	// LATER:   compare with nullptr and conditionally do the add/subtract adjustment
#endif
	
	return (TO)((uintptr_t)ptr + 0x20);
	
	// TODO: use "long nop" instead of multiple short ones
	
	// for the actual execution here, either just return (for offset == 0)
	// or do one signed 32-bit addition and return
	
	// if execution reaches here, then we need to JIT the front of the function
	// and then probably jmp or call back to the start of it
}

// TODO: to avoid repeatedly flushing the cache each time we JIT a new jit_cast instance,
// we might consider pre-JIT'ing every possible cast combination ahead of time
// (perhaps even just the ones that are actually used by the code, if possible)

// TODO: we need to be absolutely sure that we clean up any JIT pages we allocate (if even applicable)

// TODO: if modifying our own executable pages, make sure to un-write-protect, then JIT, then re-write-protect
#endif


#endif
