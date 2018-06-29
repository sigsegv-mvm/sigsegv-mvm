#ifndef _INCLUDE_SIGSEGV_STUB_ECON_H_
#define _INCLUDE_SIGSEGV_STUB_ECON_H_


#include "link/link.h"
#include "prop.h"


class IEconItemAttributeIterator;
class CEconItem;


class CAttributeManager
{
public:
	void NetworkStateChanged()           {}
	void NetworkStateChanged(void *pVar) {}
	
	template<typename T>
	static T AttribHookValue(T value, const char *attr, const CBaseEntity *ent, CUtlVector<CBaseEntity *> *vec = nullptr, bool b1 = true);
	
	static StaticFuncThunk<int, int, const char *, const CBaseEntity *, CUtlVector<CBaseEntity *> *, bool>     ft_AttribHookValue_int;
	static StaticFuncThunk<float, float, const char *, const CBaseEntity *, CUtlVector<CBaseEntity *> *, bool> ft_AttribHookValue_float;
};

template<> inline int CAttributeManager::AttribHookValue<int>(int value, const char *attr, const CBaseEntity *ent, CUtlVector<CBaseEntity *> *vec, bool b1)
{ return CAttributeManager::ft_AttribHookValue_int(value, attr, ent, vec, b1); }
template<> inline float CAttributeManager::AttribHookValue<float>(float value, const char *attr, const CBaseEntity *ent, CUtlVector<CBaseEntity *> *vec, bool b1)
{ return CAttributeManager::ft_AttribHookValue_float(value, attr, ent, vec, b1); }

template<typename T> void _CallAttribHookRef(T& value, const char *pszClass, const CBaseEntity *pEntity) { value = CAttributeManager::AttribHookValue<T>(value, pszClass, pEntity); }
#define CALL_ATTRIB_HOOK_INT(                value, name) _CallAttribHookRef<int>  (value, #name, this)
#define CALL_ATTRIB_HOOK_INT_ON_OTHER(  ent, value, name) _CallAttribHookRef<int>  (value, #name, ent )
#define CALL_ATTRIB_HOOK_FLOAT(              value, name) _CallAttribHookRef<float>(value, #name, this)
#define CALL_ATTRIB_HOOK_FLOAT_ON_OTHER(ent, value, name) _CallAttribHookRef<float>(value, #name, ent )


class CAttribute_String                 {};
class CAttribute_ItemSlotCriteria       {};
class CAttribute_DynamicRecipeComponent {};
class CAttribute_WorldItemPlacement     {};


#if 0
class attribute_data_union_t
{
public:
	attribute_data_union_t() : m_UInt(0) {}
	
	union
	{
		unsigned int                       m_UInt;
		float                              m_Float;
		uint64                            *m_UInt64;
		CAttribute_String                 *m_String;
		CAttribute_ItemSlotCriteria       *m_ItemSlotCriteria;
		CAttribute_DynamicRecipeComponent *m_DynamicRecipeComponent;
		CAttribute_WorldItemPlacement     *m_WorldItemPlacement;
	};
};
#else
union attribute_data_union_t
{
	unsigned int                       m_UInt;
	float                              m_Float;
	uint64                            *m_UInt64;
	CAttribute_String                 *m_String;
	CAttribute_ItemSlotCriteria       *m_ItemSlotCriteria;
	CAttribute_DynamicRecipeComponent *m_DynamicRecipeComponent;
	CAttribute_WorldItemPlacement     *m_WorldItemPlacement;
};
#endif
static_assert(sizeof(attribute_data_union_t) == 0x4, "");

struct static_attrib_t
{
	unsigned short m_iAttrIndex;    // +0x00
	attribute_data_union_t m_Value; // +0x04
};
static_assert(sizeof(static_attrib_t) == 0x8, "");


class CEconItemAttributeDefinition;

class ISchemaAttributeType
{
	// TODO: use vfunc thunks instead of relying on vtable matching up forever
protected:
	virtual ~ISchemaAttributeType() { assert(false); }
public:
	virtual int GetTypeUniqueIdentifier() const = 0;
	virtual void LoadEconAttributeValue(CEconItem *pItem, const CEconItemAttributeDefinition *pAttrDef, const attribute_data_union_t& value) const = 0;
	virtual std::string *ConvertEconAttributeValueToByteStream(const attribute_data_union_t& value, std::string *pString) const = 0;
	virtual bool BConvertStringToEconAttributeValue(const CEconItemAttributeDefinition *pAttrDef, const char *pString, attribute_data_union_t *pValue, bool b1) const = 0;
	virtual std::string *ConvertEconAttributeValueToString(const CEconItemAttributeDefinition *pAttrDef, const attribute_data_union_t& value, std::string *pString) const = 0;
	virtual void LoadByteStreamToEconAttributeValue(CEconItem *pItem, const CEconItemAttributeDefinition *pAttrDef, const std::string& string) const = 0;
	virtual void InitializeNewEconAttributeValue(attribute_data_union_t *pValue) const = 0;
	virtual void UnloadEconAttributeValue(attribute_data_union_t *pValue) const = 0;
	virtual bool OnIterateAttributeValue(IEconItemAttributeIterator *pIterator, const CEconItemAttributeDefinition *pAttrDef, const attribute_data_union_t& value) const = 0;
	virtual bool BSupportsGameplayModificationAndNetworking() const = 0;
};

template<typename T> class ISchemaAttributeTypeBase : public ISchemaAttributeType
{
public:
	virtual void ConvertTypedValueToByteStream(const T& value, std::string *pString) const = 0;
	virtual void ConvertByteStreamToTypedValue(const std::string& string, T *pValue) const = 0;
};

template<typename T> class CSchemaAttributeTypeBase : public ISchemaAttributeTypeBase<T> {};

class CSchemaAttributeType_Default : public CSchemaAttributeTypeBase<unsigned int> {};
class CSchemaAttributeType_Float   : public CSchemaAttributeTypeBase<float>        {};
class CSchemaAttributeType_UInt64  : public CSchemaAttributeTypeBase<uint64>       {};

template<typename T> class CSchemaAttributeTypeProtobufBase : public CSchemaAttributeTypeBase<T> {};

class CSchemaAttributeType_String                            : public CSchemaAttributeTypeProtobufBase<CAttribute_String>                 {};
class CSchemaAttributeType_ItemSlotCriteria                  : public CSchemaAttributeTypeProtobufBase<CAttribute_ItemSlotCriteria>       {};
class CSchemaAttributeType_DynamicRecipeComponentDefinedItem : public CSchemaAttributeTypeProtobufBase<CAttribute_DynamicRecipeComponent> {};
class CSchemaAttributeType_WorldItemPlacement                : public CSchemaAttributeTypeProtobufBase<CAttribute_WorldItemPlacement>     {};


struct attr_type_t
{
	CUtlConstString m_strName;
	ISchemaAttributeType *m_pType;
};
static_assert(sizeof(attr_type_t) == 0x8, "");


class CEconItemAttribute;
class CAttributeList
{
private:
	int vtable;
	
public:
	CUtlVector<CEconItemAttribute> m_Attributes;
	
	void AddAttribute(CEconItemAttribute *pAttr) { ft_AddAttribute(this, pAttr); }
	
private:
	CAttributeManager *m_pManager;
	
	static MemberFuncThunk<CAttributeList *, void, CEconItemAttribute *> ft_AddAttribute;
};
static_assert(sizeof(CAttributeList) == 0x1c, "");


constexpr int NUM_SHOOT_SOUND_TYPES = 15;
constexpr int NUM_VISUALS_BLOCKS    = 5;


struct perteamvisuals_t
{
	DECL_EXTRACT(const char *[NUM_SHOOT_SOUND_TYPES], m_Sounds);
};


class CEconItemDefinition
{
public:
	// TODO: MAKE THIS STUFF PRIVATE AND USE ACCESSORS INSTEAD!
	uint32_t vtable;
	KeyValues *m_pKV;
	short m_iItemDefIndex;
	bool m_bEnabled;
	byte m_iMinILevel;
	byte m_iMaxILevel;
	byte m_iItemQuality;
	byte m_iForcedItemQuality;
	byte m_iItemRarity;
	short m_iDefaultDropQuantity;
	byte m_iItemSeries;
	// ...
	
	DECL_EXTRACT(perteamvisuals_t *[NUM_VISUALS_BLOCKS], m_Visuals);
	
	// END TODO
	
	KeyValues *GetKeyValues() const { return this->m_pKV; }
	
	unsigned int GetEquipRegionBitMask() const { return this->m_nEquipRegionBitMask; }
	unsigned int GetEquipRegionMask()    const { return this->m_nEquipRegionMask;    }
	
	const char *GetItemName (const char *fallback = nullptr) const { return this->GetKVString("item_name",  fallback); }
	const char *GetName     (const char *fallback = nullptr) const { return this->GetKVString("name",       fallback); }
	const char *GetItemClass(const char *fallback = nullptr) const { return this->GetKVString("item_class", fallback); }
	
private:
	const char *GetKVString(const char *str, const char *fallback) const
	{
		if (this->m_pKV == nullptr) return nullptr;
		return this->m_pKV->GetString(str, fallback);
	}
	
	DECL_EXTRACT(unsigned int, m_nEquipRegionBitMask);
	DECL_EXTRACT(unsigned int, m_nEquipRegionMask);
};

class CTFItemDefinition : public CEconItemDefinition
{
public:
	int GetLoadoutSlot(int class_idx) const { return ft_GetLoadoutSlot(this, class_idx); }
	
private:
	static MemberFuncThunk<const CTFItemDefinition *, int, int> ft_GetLoadoutSlot;
};

class CEconItemView // DT_ScriptCreatedItem
{
public:
	static CEconItemView *Create()
	{
		/* using an arbitrary buffer size that's basically guaranteed to always
		 * be big enough, even if CEconItemView grows in the future */
		auto ptr = reinterpret_cast<CEconItemView *>(::operator new(0x200));
		ft_ctor(ptr);
		return ptr;
	}
	static void Destroy(CEconItemView *ptr)
	{
		if (ptr == nullptr) return;
		
		ptr->~CEconItemView();
		::operator delete(reinterpret_cast<void *>(ptr));
	}
	
	CAttributeList& GetAttributeList() const { return this->m_AttributeList; }
	
	void Init(int iItemDefIndex, int iQuality = 9999, int iLevel = 9999, unsigned int iAccountID = 0) {        ft_Init         (this, iItemDefIndex, iQuality, iLevel, iAccountID); }
	CTFItemDefinition *GetStaticData() const                                                          { return ft_GetStaticData(this); }
	CTFItemDefinition *GetItemDefinition() const                                                      { return ft_GetStaticData(this); }
	
	int GetItemDefIndex() const { return vt_GetItemDefIndex(this); }
	
private:
	virtual ~CEconItemView() { assert(false); }
	
	DECL_DATAMAP(short,          m_iItemDefinitionIndex);
	DECL_DATAMAP(int,            m_iEntityQuality);
	DECL_DATAMAP(int,            m_iEntityLevel);
	DECL_DATAMAP(int64,          m_iItemID);
	DECL_DATAMAP(bool,           m_bInitialized);
	DECL_DATAMAP(CAttributeList, m_AttributeList);
	DECL_DATAMAP(CAttributeList, m_NetworkedDynamicAttributesForDemos);
	DECL_DATAMAP(bool,           m_bOnlyIterateItemViewAttributes);
	
	static MemberFuncThunk<      CEconItemView *, void>                              ft_ctor;
	static MemberFuncThunk<      CEconItemView *, void, int, int, int, unsigned int> ft_Init;
	static MemberFuncThunk<const CEconItemView *, CTFItemDefinition *>               ft_GetStaticData;
	
	static MemberVFuncThunk<const CEconItemView *, int> vt_GetItemDefIndex;
};


class CEconItemAttributeDefinition
{
public:
	KeyValues *GetKeyValues() const       { return this->m_pKV; }
	unsigned short GetIndex() const       { return this->m_iIndex; }
	ISchemaAttributeType *GetType() const { return this->m_nAttributeType; }
	
	const char *GetName          (const char *fallback = nullptr) const { return this->GetKVString("name",            fallback); }
	const char *GetAttributeClass(const char *fallback = nullptr) const { return this->GetKVString("attribute_class", fallback); }
	
	template<typename T> bool IsType() const { return (dynamic_cast<T *>(this->GetType()) != nullptr); }
	
private:
	const char *GetKVString(const char *str, const char *fallback) const
	{
		if (this->m_pKV == nullptr) return nullptr;
		return this->m_pKV->GetString(str, fallback);
	}
	
	KeyValues *m_pKV;                       // +0x00
	unsigned short m_iIndex;                // +0x04
	ISchemaAttributeType *m_nAttributeType; // +0x08
	// ...
};

class CEconItemAttribute // DT_ScriptCreatedAttribute
{
public:
	static CEconItemAttribute *Create(short iDefIndex)
	{
		/* using an arbitrary buffer size that's basically guaranteed to always
		 * be big enough, even if CEconItemAttribute grows in the future */
		auto ptr = reinterpret_cast<CEconItemAttribute *>(::operator new(0x20));
		ft_ctor(ptr);
		ptr->m_iAttributeDefinitionIndex = iDefIndex;
		return ptr;
	}
	static void Destroy(CEconItemAttribute *ptr)
	{
		if (ptr == nullptr) return;
		::operator delete(reinterpret_cast<void *>(ptr));
	}
	
	attribute_data_union_t *GetValuePtr() { return &this->m_iRawValue32; }
	
	CEconItemAttributeDefinition *GetStaticData() const { return ft_GetStaticData(this); }
	
private:
	static MemberFuncThunk<      CEconItemAttribute *, void>                           ft_ctor;
	static MemberFuncThunk<const CEconItemAttribute *, CEconItemAttributeDefinition *> ft_GetStaticData;
	
	uint32_t __pad00;
	short m_iAttributeDefinitionIndex;
	attribute_data_union_t m_iRawValue32;
	int m_nRefundableCurrency;
};
static_assert(sizeof(CEconItemAttribute) == 0x10, "");


class CAttributeContainer : public CAttributeManager
{
public:
	CEconItemView *GetItem() { return &this->m_Item; }
	
private:
	DECL_SENDPROP_RW(CEconItemView, m_Item);
};


class CEconItemSystem
{
public:
	// TODO
	
private:
	// TODO
};
class CTFItemSystem : public CEconItemSystem {};


class CEconItemSchema
{
public:
	struct EquipRegion
	{
		CUtlConstStringBase<char> m_strName;
		unsigned int m_nBitMask;
		unsigned int m_nMask;
	};
	
	CEconItemDefinition *GetItemDefinition(int def_idx) const                          { return ft_GetItemDefinition           (this, def_idx); }
	CEconItemDefinition *GetItemDefinitionByName(const char *name) const               { return ft_GetItemDefinitionByName     (this, name); }
	CEconItemAttributeDefinition *GetAttributeDefinition(int def_idx) const            { return ft_GetAttributeDefinition      (this, def_idx); }
	CEconItemAttributeDefinition *GetAttributeDefinitionByName(const char *name) const { return ft_GetAttributeDefinitionByName(this, name); }
	
private:
	static MemberFuncThunk<const CEconItemSchema *, CEconItemDefinition *, int>                   ft_GetItemDefinition;
	static MemberFuncThunk<const CEconItemSchema *, CEconItemDefinition *, const char *>          ft_GetItemDefinitionByName;
	static MemberFuncThunk<const CEconItemSchema *, CEconItemAttributeDefinition *, int>          ft_GetAttributeDefinition;
	static MemberFuncThunk<const CEconItemSchema *, CEconItemAttributeDefinition *, const char *> ft_GetAttributeDefinitionByName;
};
class CTFItemSchema : public CEconItemSchema {};


CTFItemSchema *GetItemSchema();


class CItemGeneration
{
public:
	// TODO
	
private:
	// TODO
};


CItemGeneration *ItemGeneration();


void CopyStringAttributeValueToCharPointerOutput(const CAttribute_String *attr_str, const char **p_cstr);


extern GlobalThunk<const char *[NUM_VISUALS_BLOCKS]> g_TeamVisualSections;


int GetNumberOfLoadoutSlots();

bool IsValidLoadoutSlotNumber(int num);
int ClampLoadoutSlotNumber(int num);

const char *GetLoadoutSlotName(int slot);
int GetLoadoutSlotFromName(const char *name);


/* GetItemDefinition returns the def for the "default" item in the schema if the index doesn't match anything */
inline CTFItemDefinition *FilterOutDefaultItemDef(CTFItemDefinition *item_def)
{
	if (item_def != nullptr && strcmp(item_def->GetName("default"), "default") == 0) {
		return nullptr;
	}
	
	return item_def;
}


#endif
