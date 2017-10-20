#include "prop.h"
#include "stub/baseentity.h"
#include "util/misc.h"
#include "util/demangle.h"


void CC_ListProps(const CCommand& cmd)
{
	size_t len_obj = 0;
//	size_t len_mem = 0;
	for (auto prop : AutoList<IProp>::List()) {
		len_obj = Max(len_obj, strlen(prop->GetObjectName()));
//		len_mem = Max(len_mem, strlen(prop->GetMemberName()));
	}
	
	std::vector<IProp *> props_sorted;
	for (auto prop : AutoList<IProp>::List()) {
		props_sorted.push_back(prop);
	}
	std::sort(props_sorted.begin(), props_sorted.end(), [](const IProp *lhs, const IProp *rhs){
		std::string obj_lhs = lhs->GetObjectName();
		std::string obj_rhs = rhs->GetObjectName();
		if (obj_lhs != obj_rhs) return (obj_lhs < obj_rhs);
		
		std::string mem_lhs = lhs->GetMemberName();
		std::string mem_rhs = rhs->GetMemberName();
		return (mem_lhs < mem_rhs);
	});
	
	MAT_SINGLE_THREAD_BLOCK {
		Msg("%-8s  %7s  %-*s  %s\n", "KIND", "VALUE", len_obj, "CLASS", "MEMBER");
		for (auto prop : props_sorted) {
			const char *n_obj = prop->GetObjectName();
			const char *n_mem = prop->GetMemberName();
			const char *kind  = prop->GetKind();
			
			switch (prop->GetState()) {
			case IProp::State::INITIAL:
				Msg("%-8s  %7s  %-*s  %s\n", kind, "INIT", len_obj, n_obj, n_mem);
				break;
			case IProp::State::OK:
			{
				int off = -1;
				prop->GetOffset(off);
				Msg("%-8s  +0x%04x  %-*s  %s\n", kind, off, len_obj, n_obj, n_mem);
				break;
			}
			case IProp::State::FAIL:
				Msg("%-8s  %7s  %-*s  %s\n", kind, "FAIL", len_obj, n_obj, n_mem);
				break;
			}
		}
	}
}
static ConCommand ccmd_list_props("sig_list_props", &CC_ListProps,
	"List props and show their status", FCVAR_NONE);


namespace Prop
{
	void PreloadAll()
	{
		DevMsg("Prop::PreloadAll\n");
		for (auto prop : AutoList<IProp>::List()) {
			prop->Preload();
		}
		
		CCommand dummy;
		CC_ListProps(dummy);
	}
	
	
	bool FindOffset(int& off, const char *obj, const char *var)
	{
		for (auto prop : AutoList<IProp>::List()) {
			if (strcmp(obj, prop->GetObjectName()) == 0 && strcmp(var, prop->GetMemberName()) == 0) {
				return prop->GetOffset(off);
			}
		}
		
		return false;
	}
	
	int FindOffsetAssert(const char *obj, const char *var)
	{
		int off = -1;
		assert(FindOffset(off, obj, var));
		return off;
	}
	
	
#if 0
	static std::unordered_map<std::string, const datamap_t *> datamaps_by_classname;
	static std::unordered_map<std::string, std::string>       classnames_by_rtti_name;
	
	static void PreloadDataMaps()
	{
		static bool init = false;
		
		if (!init) {
			assert(datamaps_by_classname  .empty());
			assert(classnames_by_rtti_name.empty());
			
			auto dict = static_cast<CEntityFactoryDictionary *>(servertools->GetEntityFactoryDictionary());
			assert(dict != nullptr);
			
			FOR_EACH_DICT_FAST(dict->m_Factories, i) {
				const char *classname = dict->m_Factories.GetElementName(i);
				assert(classname != nullptr);
				assert(classname[0] != '\0');
				
				IServerNetworkable *sv_networkable = dict->Create(classname);
				assert(sv_networkable != nullptr);
				
				CBaseEntity *entity = sv_networkable->GetBaseEntity();
				assert(entity != nullptr);
				
				const datamap_t *p_datamap = entity->GetDataDescMap();
				assert(p_datamap != nullptr);
				
				const char *rtti_name = TypeName(entity);
				assert(rtti_name != nullptr);
				assert(rtti_name[0] != '\0');
				
				std::string rtti_name_demangled;
				assert(DemangleTypeName(rtti_name, rtti_name_demangled));
				rtti_name = rtti_name_demangled.c_str();
				
			//	servertools->RemoveEntityImmediate(entity);
				
				sm_datatable_info_t info;
				if (gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap(entity), "m_iEFlags", &info)) {
					*(int *)((char *)entity + info.actual_offset) |= EFL_KILLME;
				}
				
				ConColorMsg(Color(0x00, 0xff, 0xff, 0xff), "%-60s%s",
					CFmtStr("classname '%s':",  classname).Get(),
					CFmtStr("rtti_name '%s'\n", rtti_name).Get());
				
				auto result1 = datamaps_by_classname.insert(std::make_pair(classname, p_datamap));
				assert(result1.second);
				
				auto result2 = classnames_by_rtti_name.insert(std::make_pair(rtti_name, classname));
			//	assert(result2.second);
			}
			
			//	sm_datatable_info_t info;
			//	for ( int i = dict->m_Factories.First(); i != dict->m_Factories.InvalidIndex(); i = dict->m_Factories.Next( i ) )
			//	{
			//		IServerNetworkable *entity = dict->Create(dict->m_Factories.GetElementName(i));
			//		ServerClass *sclass = entity->GetServerClass();
			//		fprintf(fp,"%s - %s\n",sclass->GetName(), dict->m_Factories.GetElementName(i));
			//		
			//		if (!gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap(entity->GetBaseEntity()), "m_iEFlags", &info))
			//			continue;
			//		
			//		int *eflags = (int *)((char *)entity->GetBaseEntity() + info.actual_offset);
			//		*eflags |= (1<<0); // EFL_KILLME
			//	}
			
			init = true;
		}
	}
	
	const datamap_t *GetDataMapByClassname(const char *classname)
	{
		PreloadDataMaps();
		
		auto it = datamaps_by_classname.find(classname);
		if (it != datamaps_by_classname.end()) {
			return (*it).second;
		} else {
			return nullptr;
		}
	}
	
	const datamap_t *GetDataMapByRTTIName(const char *rtti_name)
	{
		PreloadDataMaps();
		
		auto it = classnames_by_rtti_name.find(rtti_name);
		if (it != classnames_by_rtti_name.end()) {
			return GetDataMapByClassname((*it).second.c_str());
		} else {
			return nullptr;
		}
	}
#endif
}
