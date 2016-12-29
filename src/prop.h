#ifndef _INCLUDE_SIGSEGV_PROP_H_
#define _INCLUDE_SIGSEGV_PROP_H_


#include "mem/extract.h"
#include "util/rtti.h"


/* from src/public/dt_utlvector_send.cpp */
struct CSendPropExtra_UtlVector
{
	SendTableProxyFn m_DataTableProxyFn;	// If it's a datatable, then this is the proxy they specified.
	SendVarProxyFn m_ProxyFn;				// If it's a non-datatable, then this is the proxy they specified.
	EnsureCapacityFn m_EnsureCapacityFn;
	int m_ElementStride;					// Distance between each element in the array.
	int m_Offset;							// # bytes from the parent structure to its utlvector.
	int m_nMaxElements;						// For debugging...
};


namespace Prop
{
	void PreloadAll();
	
	bool FindOffset(int& off, const char *obj, const char *var);
	int FindOffsetAssert(const char *obj, const char *var);
	
//	const datamap_t *GetDataMapByClassname(const char *classname);
//	const datamap_t *GetDataMapByRTTIName(const char *rtti_name);
//	
//	template<typename T> datamap_t *GetDataMapByRTTI()
//	{
//		return GetDataMapByRTTIName(TypeName<T>());
//	}
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
	virtual size_t GetSize() const = 0;
	virtual const char *GetKind() const = 0;
	
	void Preload() { this->DoCalcOffset(); }
	
	int GetOffsetAssert();
	bool GetOffset(int& off);
	
	State GetState() const { return this->m_State; }
	
protected:
	IProp() {}
	
	virtual bool CalcOffset(int& off) const = 0;
	
private:
	void DoCalcOffset();
	
	State m_State = State::INITIAL;
	int m_Offset = -1;
};

inline int IProp::GetOffsetAssert()
{
	int off = -1;
	assert(this->GetOffset(off));
	return off;
}

inline bool IProp::GetOffset(int& off)
{
	State state = this->m_State;
	
	if (state == State::INITIAL) {
		this->DoCalcOffset();
		state = this->m_State;
	}
	
	if (state != State::OK) {
		return false;
	}
	
	off = this->m_Offset;
	return true;
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
	virtual size_t GetSize() const override            { return sizeof(T); }
	
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
	CProp_SendProp(const char *obj, const char *mem, const char *sv_class, void (*sc_func)(void *, void *), const char *remote_name = nullptr) :
		IPropTyped<T>(obj, mem), m_pszServerClass(sv_class), m_pStateChangedFunc(sc_func), m_pszRemoteName(remote_name) {}
	
	virtual const char *GetKind() const override { return "SENDPROP"; }
	
	void StateChanged(void *obj, void *var)
	{
		(*this->m_pStateChangedFunc)(obj, var);
	}
	
private:
	virtual bool CalcOffset(int& off) const override
	{
		ServerClass *sv_class = this->FindServerClass();
		if (sv_class == nullptr) {
			Warning("CProp_SendProp: %s::%s FAIL: can't find ServerClass \"%s\"\n", this->GetObjectName(), this->GetMemberName(), this->m_pszServerClass);
			return false;
		}
		
		if (!this->FindSendProp(off, sv_class->m_pTable)) {
			Warning("CProp_SendProp: %s::%s FAIL: can't find SendProp \"%s\"\n", this->GetObjectName(), this->GetMemberName(), this->GetSendPropMemberName());
			return false;
		}
		
		return true;
	}
	
	ServerClass *FindServerClass() const
	{
		for (ServerClass *sv_class = gamedll->GetAllServerClasses(); sv_class != nullptr; sv_class = sv_class->m_pNext) {
			if (strcmp(sv_class->GetName(), this->m_pszServerClass) == 0) {
				return sv_class;
			}
		}
		
		return nullptr;
	}
	
	bool FindSendProp(int& off, SendTable *s_table) const
	{
		for (int i = 0; i < s_table->GetNumProps(); ++i) {
			SendProp *s_prop = s_table->GetProp(i);
			
			if (s_prop->GetName() != nullptr && strcmp(s_prop->GetName(), this->GetSendPropMemberName()) == 0) {
				if (!this->IsSendPropUtlVector(off, s_prop)) {
					off = s_prop->GetOffset();
				}
				return true;
			}
			
			if (s_prop->GetDataTable() != nullptr) {
				if (this->FindSendProp(off, s_prop->GetDataTable())) {
					return true;
				}
			}
		}
		
		return false;
	}
	
	bool IsSendPropUtlVector(int& off, SendProp *q_prop) const
	{
		SendTable *s_table = q_prop->GetDataTable();
		if (s_table == nullptr) return false;
		
		auto SendProxy_LengthTable = reinterpret_cast<SendTableProxyFn>(AddrManager::GetAddr("SendProxy_LengthTable"));
		assert(SendProxy_LengthTable != nullptr);
		
		for (int i = 0; i < s_table->GetNumProps(); ++i) {
			SendProp *s_prop = s_table->GetProp(i);
			
			if (s_prop->GetName() != nullptr && strcmp(s_prop->GetName(), "lengthproxy") == 0 &&
				s_prop->GetDataTable() != nullptr && s_prop->GetDataTableProxyFn() == SendProxy_LengthTable) {
				auto extra = reinterpret_cast<const CSendPropExtra_UtlVector *>(s_prop->GetExtraData());
				if (extra != nullptr) {
					off = extra->m_Offset;
					return true;
				}
			}
		}
		
		return false;
	}
	
	const char *GetSendPropMemberName() const
	{
		if (this->m_pszRemoteName != nullptr) {
			return this->m_pszRemoteName;
		} else {
			return this->GetMemberName();
		}
	}
	
	const char *m_pszServerClass;
	void (*m_pStateChangedFunc)(void *, void *);
	const char *m_pszRemoteName;
};


template<typename T>
class CProp_DataMap : public IPropTyped<T>
{
public:
	CProp_DataMap(const char *obj, const char *mem) :
		IPropTyped<T>(obj, mem) {}
	
	virtual const char *GetKind() const override { return "DATAMAP"; }
	
	void StateChanged(void *obj, void *var) {}
	
private:
	virtual bool CalcOffset(int& off) const override
	{
		char str_DataMap[1024];
		V_sprintf_safe(str_DataMap, "%s::m_DataMap", this->GetObjectName());
		
		auto datamap = (datamap_t *)AddrManager::GetAddr(str_DataMap);
		if (datamap == nullptr) {
			Warning("CProp_DataMap: %s::%s FAIL: no addr for %s\n", this->GetObjectName(), this->GetMemberName(), str_DataMap);
			return false;
		}
		
		sm_datatable_info_t info;
		if (!gamehelpers->FindDataMapInfo(datamap, this->GetMemberName(), &info)) {
			Warning("CProp_DataMap: %s::%s FAIL: in FindDataMapInfo\n", this->GetObjectName(), this->GetMemberName());
			return false;
		}
		
//		const datamap_t *datamap = Prop::GetDataMapByRTTIName(this->GetObjectName());
//		if (datamap == nullptr) {
//			Warning("CProp_DataMap: %s::%s FAIL: can't find datamap for class %s\n", this->GetObjectName(), this->GetMemberName(), this->GetObjectName());
//			return false;
//		}
//		
//		sm_datatable_info_t info;
//		if (!gamehelpers->FindDataMapInfo(const_cast<datamap_t *>(datamap), this->GetMemberName(), &info)) {
//			Warning("CProp_DataMap: %s::%s FAIL: in FindDataMapInfo\n", this->GetObjectName(), this->GetMemberName());
//			return false;
//		}
		
		off = GetTypeDescOffs(info.prop);
		return true;
	}
};
//#ifdef __GNUC__
//#warning TODO: delete gamedata/sigsegv/datamaps.txt and remove from PackageScript and gameconf.cpp
//#endif


template<typename T>
class CProp_Extract : public IPropTyped<T>
{
public:
	CProp_Extract(const char *obj, const char *mem, IExtract<T *> *extractor) :
		IPropTyped<T>(obj, mem), m_Extractor(extractor) {}
	virtual ~CProp_Extract()
	{
		if (this->m_Extractor != nullptr) {
			delete this->m_Extractor;
		}
	}
	
	virtual const char *GetKind() const override { return "EXTRACT"; }
	
	void StateChanged(void *obj, void *var) {}
	
private:
	virtual bool CalcOffset(int& off) const override
	{
		if (!this->m_Extractor->Init()) {
			Warning("CProp_Extract: %s::%s FAIL: in extractor Init\n", this->GetObjectName(), this->GetMemberName());
			return false;
		}
		
		if (!this->m_Extractor->Check()) {
			Warning("CProp_Extract: %s::%s FAIL: in extractor Check\n", this->GetObjectName(), this->GetMemberName());
			return false;
		}
		
		off = (int)this->m_Extractor->Extract();
		return true;
	}
	
	IExtract<T *> *m_Extractor;
};


template<typename T>
class CProp_Relative : public IPropTyped<T>
{
public:
	enum RelativeMethod
	{
		REL_MANUAL,
		REL_AFTER,
		REL_BEFORE,
	};
	
	CProp_Relative(const char *obj, const char *mem, IProp *prop, RelativeMethod method, int diff = 0) :
		IPropTyped<T>(obj, mem), m_RelProp(prop)
	{
		switch (method) {
		default:
		case REL_MANUAL: this->m_iDiff =  diff;                   break;
		case REL_AFTER:  this->m_iDiff =  diff + prop->GetSize(); break;
		case REL_BEFORE: this->m_iDiff = -diff - this->GetSize(); break;
		}
	}
	
	virtual const char *GetKind() const override { return "RELATIVE"; }
	
	void StateChanged(void *obj, void *var) {}
	
private:
	virtual bool CalcOffset(int& off) const override
	{
		int base_off = 0;
		
		if (!this->m_RelProp->GetOffset(base_off)) {
			Warning("CProp_Relative: %s::%s FAIL: in base prop GetOffset\n", this->GetObjectName(), this->GetMemberName());
			return false;
		}
		
		off = base_off + this->m_iDiff;
		return true;
	}
	
	IProp *m_RelProp;
	int m_iDiff;
};


#define T_PARAMS typename IPROP, IPROP *PROP, const size_t *ADJUST, bool NET, bool RW
#define T_ARGS            IPROP,        PROP,               ADJUST,      NET,      RW


template<typename T, T_PARAMS> class CPropAccessorBase
{
private:
	/* determine whether we should be returning writable refs/ptrs */
//	static constexpr bool ReadOnly() { return (NET && !RW); }
	
	/* reference typedefs */
	using RefRO_t = const T&;
	using RefRW_t =       T&;
//	using Ref_t   = typename std::conditional<ReadOnly(), RefRO_t, RefRW_t>::type;
	using Ref_t   = typename std::conditional<(NET && !RW), RefRO_t, RefRW_t>::type;
	
	/* pointer typedefs */
	using PtrRO_t = const T*;
	using PtrRW_t =       T*;
//	using Ptr_t   = typename std::conditional<ReadOnly(), PtrRO_t, PtrRW_t>::type;
	using Ptr_t   = typename std::conditional<(NET && !RW), PtrRO_t, PtrRW_t>::type;
	
public:
	CPropAccessorBase()                                    = delete;
	CPropAccessorBase(CPropAccessorBase&  copy)            = delete;
	CPropAccessorBase(CPropAccessorBase&& move)            = delete;
	CPropAccessorBase& operator=(CPropAccessorBase&& move) = delete;
	
	/* conversion operators */
	operator Ref_t() const { return this->Get(); }
	
//	template<typename A, bool RW2 = (!NET || RW)> operator typename std::enable_if<( RW2 && std::is_convertible<T, A>::value),       A&>::type() const { return static_cast<      A&>(this->GetRW()); }
//	template<typename A, bool RW2 = (!NET || RW)> operator typename std::enable_if<(!RW2 && std::is_convertible<T, A>::value), const A&>::type() const { return static_cast<const A&>(this->GetRO()); }
	
	/* assignment */
	template<typename A> const T& operator=(const CPropAccessorBase<A, T_ARGS>& val) { return this->Set(static_cast<const T>(val.GetRO())); }
	template<typename A> const T& operator=(const A& val)                            { return this->Set(static_cast<const T>(val));         }
	
	/* assignment with modify */
	template<typename A> const T& operator+= (const A& val) { return this->Set(this->GetRO() +  static_cast<const T>(val)); }
	template<typename A> const T& operator-= (const A& val) { return this->Set(this->GetRO() -  static_cast<const T>(val)); }
	template<typename A> const T& operator*= (const A& val) { return this->Set(this->GetRO() *  static_cast<const T>(val)); }
	template<typename A> const T& operator/= (const A& val) { return this->Set(this->GetRO() /  static_cast<const T>(val)); }
	template<typename A> const T& operator%= (const A& val) { return this->Set(this->GetRO() %  static_cast<const T>(val)); }
	template<typename A> const T& operator&= (const A& val) { return this->Set(this->GetRO() &  static_cast<const T>(val)); }
	template<typename A> const T& operator|= (const A& val) { return this->Set(this->GetRO() |  static_cast<const T>(val)); }
	template<typename A> const T& operator^= (const A& val) { return this->Set(this->GetRO() ^  static_cast<const T>(val)); }
	template<typename A> const T& operator<<=(const A& val) { return this->Set(this->GetRO() << static_cast<const T>(val)); }
	template<typename A> const T& operator>>=(const A& val) { return this->Set(this->GetRO() >> static_cast<const T>(val)); }
	
	/* comparison */
	template<typename A> auto operator==(const A& val) const { return (this->GetRO() == val); }
	template<typename A> auto operator!=(const A& val) const { return (this->GetRO() != val); }
	template<typename A> auto operator< (const A& val) const { return (this->GetRO() <  val); }
	template<typename A> auto operator> (const A& val) const { return (this->GetRO() >  val); }
	template<typename A> auto operator<=(const A& val) const { return (this->GetRO() <= val); }
	template<typename A> auto operator>=(const A& val) const { return (this->GetRO() >= val); }
	
	/* unary arithmetic/logic */
	auto operator+() const { return +this->GetRO(); }
	auto operator-() const { return -this->GetRO(); }
	auto operator~() const { return ~this->GetRO(); }
	
	/* binary arithmetic/logic */
	template<typename A> friend auto operator+ (const CPropAccessorBase<T, T_ARGS>& acc, const A& val) { return (acc.GetRO() +  val        ); }
	template<typename A> friend auto operator+ (const A& val, const CPropAccessorBase<T, T_ARGS>& acc) { return (val         +  acc.GetRO()); }
	template<typename A> friend auto operator- (const CPropAccessorBase<T, T_ARGS>& acc, const A& val) { return (acc.GetRO() -  val        ); }
	template<typename A> friend auto operator- (const A& val, const CPropAccessorBase<T, T_ARGS>& acc) { return (val         -  acc.GetRO()); }
	template<typename A> friend auto operator* (const CPropAccessorBase<T, T_ARGS>& acc, const A& val) { return (acc.GetRO() *  val        ); }
	template<typename A> friend auto operator* (const A& val, const CPropAccessorBase<T, T_ARGS>& acc) { return (val         *  acc.GetRO()); }
	template<typename A> friend auto operator/ (const CPropAccessorBase<T, T_ARGS>& acc, const A& val) { return (acc.GetRO() /  val        ); }
	template<typename A> friend auto operator/ (const A& val, const CPropAccessorBase<T, T_ARGS>& acc) { return (val         /  acc.GetRO()); }
	template<typename A> friend auto operator% (const CPropAccessorBase<T, T_ARGS>& acc, const A& val) { return (acc.GetRO() %  val        ); }
	template<typename A> friend auto operator% (const A& val, const CPropAccessorBase<T, T_ARGS>& acc) { return (val         %  acc.GetRO()); }
	template<typename A> friend auto operator& (const CPropAccessorBase<T, T_ARGS>& acc, const A& val) { return (acc.GetRO() &  val        ); }
	template<typename A> friend auto operator& (const A& val, const CPropAccessorBase<T, T_ARGS>& acc) { return (val         &  acc.GetRO()); }
	template<typename A> friend auto operator| (const CPropAccessorBase<T, T_ARGS>& acc, const A& val) { return (acc.GetRO() |  val        ); }
	template<typename A> friend auto operator| (const A& val, const CPropAccessorBase<T, T_ARGS>& acc) { return (val         |  acc.GetRO()); }
	template<typename A> friend auto operator^ (const CPropAccessorBase<T, T_ARGS>& acc, const A& val) { return (acc.GetRO() ^  val        ); }
	template<typename A> friend auto operator^ (const A& val, const CPropAccessorBase<T, T_ARGS>& acc) { return (val         ^  acc.GetRO()); }
	template<typename A> friend auto operator<<(const CPropAccessorBase<T, T_ARGS>& acc, const A& val) { return (acc.GetRO() << val        ); }
	template<typename A> friend auto operator<<(const A& val, const CPropAccessorBase<T, T_ARGS>& acc) { return (val         << acc.GetRO()); }
	template<typename A> friend auto operator>>(const CPropAccessorBase<T, T_ARGS>& acc, const A& val) { return (acc.GetRO() >> val        ); }
	template<typename A> friend auto operator>>(const A& val, const CPropAccessorBase<T, T_ARGS>& acc) { return (val         >> acc.GetRO()); }
	
	/* operator& */
	Ptr_t operator&() const { return this->GetPtr(); }
	
	/* operator-> */
	Ptr_t operator->() const { return this->GetPtr(); }
	
	/* pre-increment */
	const T& operator++() { return this->Set(this->GetRO() + 1); }
	const T& operator--() { return this->Set(this->GetRO() - 1); }
	
	/* post-increment */
	template<typename T2 = T> typename std::enable_if<!std::is_abstract<T2>::value, T2>::type operator++(int) { T copy = this->GetRO(); this->Set(this->GetRO() + 1); return copy; }
	template<typename T2 = T> typename std::enable_if<!std::is_abstract<T2>::value, T2>::type operator--(int) { T copy = this->GetRO(); this->Set(this->GetRO() - 1); return copy; }
	
	/* indexing */
	#ifdef __GNUC__
	//#warning TODO: ensure that operator[] returns reference types and that their constness is correct
	#endif
	template<typename A> auto operator[](const A& idx) const { return this->Get()[idx]; }
	
//	template<typename T2 = T, bool RW2 = (!NET || RW)> typename std::enable_if<( RW2 && std::is_array<T2>::value),       T&/* remove extent */>::type operator[](/* TODO */) const/*?*/ { /* TODO */ }
//	template<typename T2 = T, bool RW2 = (!NET || RW)> typename std::enable_if<(!RW2 && std::is_array<T2>::value), const T&/* remove extent */>::type operator[](/* TODO */) const/*?*/ { /* TODO */ }
//	template<typename T2 = T, bool RW2 = (!NET || RW)> typename std::enable_if<( RW2 && std::is_array<T2>::value),       decltype(std::declval<T>()[0])&>::type operator[](ptrdiff_t idx) const { return this->Get_RW()[idx]; }
//	template<typename T2 = T, bool RW2 = (!NET || RW)> typename std::enable_if<(!RW2 && std::is_array<T2>::value), const decltype(std::declval<T>()[0])&>::type operator[](ptrdiff_t idx) const { return this->Get_RO()[idx]; }
	// only const for now
//	template<typename T2 = T> typename std::enable_if<std::is_function<std::declval<T>()[0]>::value, >::type 
	// maybe abuse begin() / end() iterator stuff?
	
	/* begin() and end() passthru */
	#ifdef __GNUC__
	//#warning TODO: ensure that begin() and end() return the right things based on constness
	#endif
	auto begin() { return this->Get().begin(); }
	auto end()   { return this->Get().end();   }
	
	/* not implemented yet */
	template<typename... ARGS> auto operator() (ARGS...) = delete; // TODO
	template<typename... ARGS> auto operator*  (ARGS...) = delete; // TODO
	template<typename... ARGS> auto operator->*(ARGS...) = delete; // TODO
	
protected:
	RefRO_t Set(RefRO_t val)
	{
		if (NET) {
			if (memcmp(this->GetPtrRO(), &val, sizeof(T)) != 0) {
				PROP->StateChanged(reinterpret_cast<void *>(this->GetInstanceBaseAddr()), this->GetPtrRW());
				return (this->GetRW() = val);
			} else {
				return this->GetRO();
			}
		} else {
			return (this->GetRW() = val);
		}
	}
	
	
	/* reference getters */
	RefRO_t GetRO() const { return *this->GetPtrRO(); }
	RefRW_t GetRW() const { return *this->GetPtrRW(); }
	Ref_t   Get  () const { return *this->GetPtr  (); }
	
	/* pointer getters */
	PtrRO_t GetPtrRO() const { return reinterpret_cast<PtrRO_t>(this->GetInstanceVarAddr()); }
	PtrRW_t GetPtrRW() const { return reinterpret_cast<PtrRW_t>(this->GetInstanceVarAddr()); }
	Ptr_t   GetPtr  () const { return reinterpret_cast<Ptr_t  >(this->GetInstanceVarAddr()); }
	
	
	uintptr_t GetInstanceBaseAddr() const { return (reinterpret_cast<uintptr_t>(this) - *ADJUST); }
	
private:
	uintptr_t GetInstanceVarAddr() const { return (this->GetInstanceBaseAddr() + this->GetCachedVarOffset()); }
	
	ptrdiff_t GetCachedVarOffset() const
	{
		static ptrdiff_t s_CachedVarOff = PROP->GetOffsetAssert();
		return s_CachedVarOff;
	}
};

template<typename U, T_PARAMS> struct CPropAccessorHandle : public CPropAccessorBase<CHandle<U>, T_ARGS>
{
	using CPropAccessorBase<CHandle<U>, T_ARGS>::operator=;
	
	operator U*() const   { return this->GetRO().Get(); }
	U* operator->() const { return this->GetRO().Get(); }
};


template<typename T, T_PARAMS> struct CPropAccessor : public CPropAccessorBase<T, T_ARGS>
{
	using CPropAccessorBase<T, T_ARGS>::operator=;
};
template<typename U, T_PARAMS> struct CPropAccessor<CHandle<U>, T_ARGS> : public CPropAccessorHandle<U, T_ARGS>
{
	using CPropAccessorHandle<U, T_ARGS>::operator=;
};


/* some sanity checks to ensure zero-size, no ctors, no vtable, etc */
#define CHECK_ACCESSOR(ACCESSOR) \
	static_assert( std::is_empty                <ACCESSOR>::value, "NOT GOOD: Prop accessor isn't an empty type"     ); \
	static_assert(!std::is_polymorphic          <ACCESSOR>::value, "NOT GOOD: Prop accessor has virtual functions"   ); \
	static_assert(!std::is_default_constructible<ACCESSOR>::value, "NOT GOOD: Prop accessor is default-constructible"); \
	static_assert(!std::is_copy_constructible   <ACCESSOR>::value, "NOT GOOD: Prop accessor is copy-constructible"   ); \
	static_assert(!std::is_move_constructible   <ACCESSOR>::value, "NOT GOOD: Prop accessor is move-constructible"   )


#define DECL_PROP(TYPE, PROPNAME, VARIANT, NET, RW) \
	typedef CProp_##VARIANT<TYPE> _type_prop_##PROPNAME; \
	static _type_prop_##PROPNAME s_prop_##PROPNAME; \
	static const size_t _adj_##PROPNAME; \
	typedef CPropAccessor<TYPE, _type_prop_##PROPNAME, &s_prop_##PROPNAME, &_adj_##PROPNAME, NET, RW> _type_accessor_##PROPNAME; \
	_type_accessor_##PROPNAME PROPNAME; \
	CHECK_ACCESSOR(_type_accessor_##PROPNAME)

#define DECL_SENDPROP(   TYPE, PROPNAME) DECL_PROP(TYPE, PROPNAME, SendProp, true,  false)
#define DECL_SENDPROP_RW(TYPE, PROPNAME) DECL_PROP(TYPE, PROPNAME, SendProp, true,  true )
#define DECL_DATAMAP(    TYPE, PROPNAME) DECL_PROP(TYPE, PROPNAME, DataMap,  false, false)
#define DECL_EXTRACT(    TYPE, PROPNAME) DECL_PROP(TYPE, PROPNAME, Extract,  false, false)
#define DECL_RELATIVE(   TYPE, PROPNAME) DECL_PROP(TYPE, PROPNAME, Relative, false, false)


// for IMPL_SENDPROP, add an additional argument for the "remote name" (e.g. in CBaseEntity, m_MoveType's remote name is "movetype")
#define IMPL_SENDPROP(TYPE, CLASSNAME, PROPNAME, SVCLASS, ...) \
	void NetworkStateChanged_##CLASSNAME##_##PROPNAME(void *obj, void *var) { reinterpret_cast<CLASSNAME *>(obj)->NetworkStateChanged(var); } \
	const size_t CLASSNAME::_adj_##PROPNAME = offsetof(CLASSNAME, PROPNAME); \
	CProp_SendProp<TYPE> CLASSNAME::s_prop_##PROPNAME(#CLASSNAME, #PROPNAME, #SVCLASS, &NetworkStateChanged_##CLASSNAME##_##PROPNAME, ##__VA_ARGS__)
#define IMPL_DATAMAP(TYPE, CLASSNAME, PROPNAME) \
	const size_t CLASSNAME::_adj_##PROPNAME = offsetof(CLASSNAME, PROPNAME); \
	CProp_DataMap<TYPE> CLASSNAME::s_prop_##PROPNAME(#CLASSNAME, #PROPNAME)
#define IMPL_EXTRACT(TYPE, CLASSNAME, PROPNAME, EXTRACTOR) \
	const size_t CLASSNAME::_adj_##PROPNAME = offsetof(CLASSNAME, PROPNAME); \
	CProp_Extract<TYPE> CLASSNAME::s_prop_##PROPNAME(#CLASSNAME, #PROPNAME, EXTRACTOR)
#define IMPL_RELATIVE(TYPE, CLASSNAME, PROPNAME, RELPROP, DIFF) \
	const size_t CLASSNAME::_adj_##PROPNAME = offsetof(CLASSNAME, PROPNAME); \
	CProp_Relative<TYPE> CLASSNAME::s_prop_##PROPNAME(#CLASSNAME, #PROPNAME, &CLASSNAME::s_prop_##RELPROP, CProp_Relative<TYPE>::REL_MANUAL, DIFF)
#define IMPL_REL_AFTER(TYPE, CLASSNAME, PROPNAME, RELPROP, ...) \
	const size_t CLASSNAME::_adj_##PROPNAME = offsetof(CLASSNAME, PROPNAME); \
	CProp_Relative<TYPE> CLASSNAME::s_prop_##PROPNAME(#CLASSNAME, #PROPNAME, &CLASSNAME::s_prop_##RELPROP, CProp_Relative<TYPE>::REL_AFTER, ##__VA_ARGS__)
#define IMPL_REL_BEFORE(TYPE, CLASSNAME, PROPNAME, RELPROP, ...) \
	const size_t CLASSNAME::_adj_##PROPNAME = offsetof(CLASSNAME, PROPNAME); \
	CProp_Relative<TYPE> CLASSNAME::s_prop_##PROPNAME(#CLASSNAME, #PROPNAME, &CLASSNAME::s_prop_##RELPROP, CProp_Relative<TYPE>::REL_BEFORE, ##__VA_ARGS__)


#endif
