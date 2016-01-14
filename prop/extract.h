#ifndef _INCLUDE_SIGSEGV_PROP_EXTRACT_H_
#define _INCLUDE_SIGSEGV_PROP_EXTRACT_H_


#include "extension.h"
#include "mem/extract.h"


template<typename T, const char *const *CLASS, const char *const *PROP, class EXTRACTOR>
class CProp_Extract
{
public:
	const T& operator=(const T& val) { this->Set(val); return val; }
	operator const T&() const        { return this->Get(); }
	
private:
	const T& Get() const;
	void Set(const T& val) const;
	
	static int CalcOffset();
	
	static bool s_bInit;
	static int s_iOffset;
};

template<typename T, const char *const *CLASS, const char *const *PROP, class EXTRACTOR>
bool CProp_Extract<T, CLASS, PROP, EXTRACTOR>::s_bInit = false;

template<typename T, const char *const *CLASS, const char *const *PROP, class EXTRACTOR>
int CProp_Extract<T, CLASS, PROP, EXTRACTOR>::s_iOffset = 0;

template<typename T, const char *const *CLASS, const char *const *PROP, class EXTRACTOR>
inline const T& CProp_Extract<T, CLASS, PROP, EXTRACTOR>::Get() const
{
	if (!s_bInit) {
		s_iOffset = CalcOffset();
		s_bInit = true;
	}
	
	return *reinterpret_cast<const T *>((uintptr_t)this + s_iOffset);
}

template<typename T, const char *const *CLASS, const char *const *PROP, class EXTRACTOR>
inline void CProp_Extract<T, CLASS, PROP, EXTRACTOR>::Set(const T& val) const
{
	if (!s_bInit) {
		s_iOffset = CalcOffset();
		s_bInit = true;
	}
	
	*reinterpret_cast<T *>((uintptr_t)this + s_iOffset) = val;
}

template<typename T, const char *const *CLASS, const char *const *PROP, class EXTRACTOR>
inline int CProp_Extract<T, CLASS, PROP, EXTRACTOR>::CalcOffset()
{
	EXTRACTOR extractor;
	
	assert(extractor.Init());
	assert(extractor.Check());
	
	int offset = extractor.Extract();
	
	DevMsg("[EXTRACT]  0x%04x %s::%s\n", offset, *CLASS, *PROP);
	return offset;
}


#endif
