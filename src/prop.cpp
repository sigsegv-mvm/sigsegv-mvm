#include "prop.h"


void CC_ListProps(const CCommand& cmd)
{
	size_t len_obj = 0;
//	size_t len_mem = 0;
	for (auto prop : AutoList<IProp>::List()) {
		len_obj = Max(len_obj, strlen(prop->GetObjectName()));
//		len_mem = Max(len_mem, strlen(prop->GetMemberName()));
	}
	
	Msg("%-8s  %7s  %-*s  %s\n", "KIND", "VALUE", len_obj, "CLASS", "MEMBER");
	for (auto prop : AutoList<IProp>::List()) {
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
	
	bool FindOffset(int& off, const char *obj, const char *mem)
	{
		for (auto prop : AutoList<IProp>::List()) {
			if (strcmp(obj, prop->GetObjectName()) == 0 && strcmp(mem, prop->GetMemberName()) == 0) {
				return prop->GetOffset(off);
			}
		}
		
		return false;
	}
}
