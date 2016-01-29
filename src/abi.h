#ifndef _INCLUDE_SIGSEGV_ABI_H_
#define _INCLUDE_SIGSEGV_ABI_H_


#if defined __GNUC__
#include <cxxabi.h>
#else
namespace abi
{
	typedef void __class_type_info;
}
#endif


/* GCC ABI */

struct vtable
{
	ptrdiff_t off_to_top;
	abi::__class_type_info *typeinfo;
	void *vfptrs[0x1000];
};


/* MSVC ABI */

#if defined _MSC_VER
#pragma warning(disable:4200)
#endif

#if !defined _MSC_VER
struct _PMD
{
	int mdisp;
	int pdisp;
	int vdisp;
};

struct _TypeDescriptor
{
	const void *pVFTable;
	void *spare;
	char name[];
};
#endif

struct __RTTI_BaseClassDescriptor
{
	_TypeDescriptor *pTypeDescriptor;
	unsigned long numContainedBases;
	_PMD where;
	unsigned long attributes;
};

struct __RTTI_BaseClassArray
{
	__RTTI_BaseClassDescriptor *arrayOfBaseClassDescriptors[];
};

struct __RTTI_ClassHierarchyDescriptor
{
	unsigned long signature;
	unsigned long offset;
	unsigned long numBaseClasses;
	__RTTI_BaseClassArray *pBaseClassArray;
};

struct __RTTI_CompleteObjectLocator
{
	unsigned long signature;
	unsigned long offset;
	unsigned long cdOffset;
	_TypeDescriptor *pTypeDescriptor;
	__RTTI_ClassHierarchyDescriptor *pClassDescriptor;
};

#if defined _MSC_VER
#pragma warning(default:4200)
#endif

#if defined _MSC_VER
/* from VC/crt/src/vcruntime/rtti.cpp */
extern "C" PVOID __CLRCALL_OR_CDECL __RTDynamicCast (
	PVOID inptr,
	LONG VfDelta,
//	const _TypeDescriptor *SrcType,
//	const _TypeDescriptor *TargetType,
	PVOID SrcType,
	PVOID TargetType,
	BOOL isReference
	) throw(...);
#endif


#endif
