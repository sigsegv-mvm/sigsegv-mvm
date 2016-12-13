#include "stub/econ.h"


StaticFuncThunk<int, int, const char *, const CBaseEntity *, CUtlVector<CBaseEntity *> *, bool>     CAttributeManager::ft_AttribHookValue_int  ("CAttributeManager::AttribHookValue<int>");
StaticFuncThunk<float, float, const char *, const CBaseEntity *, CUtlVector<CBaseEntity *> *, bool> CAttributeManager::ft_AttribHookValue_float("CAttributeManager::AttribHookValue<float>");


MemberVFuncThunk<CEconItemView *, int> CEconItemView::vt_GetItemDefIndex(TypeName<CEconItemView>(), "CEconItemView::GetItemDefIndex");


MemberFuncThunk<CEconItemSchema *, CEconItemDefinition *, const char *>          CEconItemSchema::ft_GetItemDefinitionByName     ("CEconItemSchema::GetItemDefinitionByName");
MemberFuncThunk<CEconItemSchema *, CEconItemAttributeDefinition *, const char *> CEconItemSchema::ft_GetAttributeDefinitionByName("CEconItemSchema::GetAttributeDefinitionByName");


static StaticFuncThunk<CTFItemSchema *> ft_GetItemSchema("GetItemSchema");
CTFItemSchema *GetItemSchema() { return ft_GetItemSchema(); }
