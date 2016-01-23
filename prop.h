#ifndef _INCLUDE_SIGSEGV_PROP_H_
#define _INCLUDE_SIGSEGV_PROP_H_


#include "mem/extract.h"


namespace Prop
{
	void PreloadAll();
}


class IProp : public AutoList<IProp>
{
public:
	enum class State : int
	{
		INITIAL,
		OK,
		FAIL,
	};
	
	virtual const char *GetObjectName() const = 0;
	virtual const char *GetMemberName() const = 0;
	virtual const char *GetKind() const = 0;
	
	void Preload() { this->DoCalcOffset(); }
	
	int GetOffset();
	
	State GetState() const { return this->m_State; }
	
protected:
	IProp() {}
	
	virtual bool CalcOffset(int& off) const = 0;
	
private:
	void DoCalcOffset();
	
	State m_State = State::INITIAL;
	int m_Offset = -1;
};

inline int IProp::GetOffset()
{
	this->DoCalcOffset();
	
	assert(this->m_State != State::FAIL);
	return this->m_Offset;
}

inline void IProp::DoCalcOffset()
{
	if (this->m_State == State::INITIAL) {
		if (this->CalcOffset(this->m_Offset)) {
			this->m_State = State::OK;
		} else {
			this->m_State = State::FAIL;
		}
	}
}


// template: AVOID IF POSSIBLE
// ctor:     OKAY
// dtor:     OKAY
// virtual:  OKAY
// members:  OKAY
template<typename T>
class IPropTyped : public IProp
{
public:
	virtual const char *GetObjectName() const override { return this->m_pszObjName; }
	virtual const char *GetMemberName() const override { return this->m_pszMemName; }
	
protected:
	IPropTyped(const char *obj, const char *mem) :
		m_pszObjName(obj), m_pszMemName(mem) {}
	
private:
	const char *m_pszObjName;
	const char *m_pszMemName;
};


template<typename T>
class CProp_SendProp : public IPropTyped<T>
{
public:
	CProp_SendProp(const char *obj, const char *mem, const char *sv_class) :
		IPropTyped<T>(obj, mem), m_pszServerClass(sv_class) {}
	
	virtual const char *GetKind() const override { return "SENDPROP"; }
	
private:
	virtual bool CalcOffset(int& off) const override
	{
		sm_sendprop_info_t info;
		if (!gamehelpers->FindSendPropInfo(this->m_pszServerClass, this->GetMemberName(), &info)) {
			DevMsg("CProp_SendProp: %s::%s FAIL: in FindSendPropInfo\n", this->GetObjectName(), this->GetMemberName());
			return false;
		}
		
		off = info.prop->GetOffset();
		return true;
	}
	
	const char *m_pszServerClass;
};


template<typename T>
class CProp_DataMap : public IPropTyped<T>
{
public:
	CProp_DataMap(const char *obj, const char *mem) :
		IPropTyped<T>(obj, mem) {}
	
	virtual const char *GetKind() const override { return "DATAMAP"; }
	
private:
	virtual bool CalcOffset(int& off) const override
	{
		char str_DataMap[1024];
		snprintf(str_DataMap, sizeof(str_DataMap), "%s::m_DataMap", this->GetObjectName());
		
		datamap_t *map = (datamap_t *)AddrManager::GetAddr(str_DataMap);
		if (map == nullptr) {
			DevMsg("CProp_DataMap: %s::%s FAIL: no addr for %s\n", this->GetObjectName(), this->GetMemberName(), str_DataMap);
			return false;
		}
		
		sm_datatable_info_t info;
		if (!gamehelpers->FindDataMapInfo(map, this->GetMemberName(), &info)) {
			DevMsg("CProp_DataMap: %s::%s FAIL: in FindDataMapInfo\n", this->GetObjectName(), this->GetMemberName());
			return false;
		}
		
		off = GetTypeDescOffs(info.prop);
		return true;
	}
};


template<typename T>
class CProp_Extract : public IPropTyped<T>
{
public:
	CProp_Extract(const char *obj, const char *mem, IExtract<T> *extractor) :
		IPropTyped<T>(obj, mem), m_Extractor(extractor) {}
	virtual ~CProp_Extract()
	{
		if (this->m_Extractor != nullptr) {
			delete this->m_Extractor;
		}
	}
	
	virtual const char *GetKind() const override { return "EXTRACT"; }
	
private:
	virtual bool CalcOffset(int& off) const override
	{
		if (!this->m_Extractor->Init()) {
			DevMsg("CProp_Extract: %s::%s FAIL: in extractor Init\n", this->GetObjectName(), this->GetMemberName());
			return false;
		}
		
		if (!this->m_Extractor->Check()) {
			DevMsg("CProp_Extract: %s::%s FAIL: in extractor Check\n", this->GetObjectName(), this->GetMemberName());
			return false;
		}
		
		off = this->m_Extractor->Extract();
		return true;
	}
	
	IExtract<T> *m_Extractor;
};


// template: OKAY
// ctor:     NOPE
// dtor:     NOPE
// virtual:  NOPE
// members:  NOPE
template<typename T, typename IPROP, IPROP *PROP, bool NET = false>
class CPropAccessor
{
public:
	CPropAccessor() = delete;
//	~CPropAccessor() = delete;
	
	const T& operator=(const T& val) { this->Set(val); return val; }
	operator const T&() const        { return this->Get(); }
	
	/* this is questionable and probably ought to be removed */
	operator T*() const              { return this->GetPtr(); }
	
private:
	T *GetPtr() const
	{
		return reinterpret_cast<T *>((uintptr_t)this + PROP->GetOffset());
	}
	
	const T& Get() const
	{
		return *this->GetPtr();
	}
	
	void Set(const T& val) const
	{
		if (NET) {
			/* TODO: update network state */
			assert(false);
		}
		
		*this->GetPtr() = val;
	}
};


#define DEF_SENDPROP(T, P) \
	static CProp_SendProp<T> s_prop_##P; \
	CPropAccessor<T, CProp_SendProp<T>, &s_prop_##P, true> P
#define DEF_DATAMAP(T, P) \
	static CProp_DataMap<T> s_prop_##P; \
	CPropAccessor<T, CProp_DataMap<T>, &s_prop_##P, false> P
#define DEF_EXTRACT(T, P) \
	static CProp_Extract<T> s_prop_##P; \
	CPropAccessor<T, CProp_Extract<T>, &s_prop_##P, false> P

#define IMPL_SENDPROP(T, C, P, SC) \
	CProp_SendProp<T> C::s_prop_##P(#C, #P, #SC)
#define IMPL_DATAMAP(T, C, P) \
	CProp_DataMap<T> C::s_prop_##P(#C, #P)
#define IMPL_EXTRACT(T, C, P, X) \
	CProp_Extract<T> C::s_prop_##P(#C, #P, X)


#endif
