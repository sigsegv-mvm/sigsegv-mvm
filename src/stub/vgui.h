#ifndef _INCLUDE_SIGSEGV_STUB_VGUI_H_
#define _INCLUDE_SIGSEGV_STUB_VGUI_H_


#include "link/link.h"

#include <../client/iclientmode.h>
#include <vgui_controls/Frame.h>


//namespace vgui
//{
//	Frame::Frame(vgui::Panel *parent, const char *panelName, bool showTaskbarIcon = true, bool bPopup = true);
//}


//extern GlobalThunk<IClientMode *> g_pClientMode;
//inline IClientMode *GetClientModeNormal() { return g_pClientMode; }


class CBuildFactoryHelper
{
public:
	static void GetFactoryNames(CUtlVector<const char *>& list) {        ft_GetFactoryNames(list); }
	static vgui::Panel *InstancePanel(const char *className)    { return ft_InstancePanel(className); }
	
private:
	static StaticFuncThunk<void, CUtlVector<const char *>&> ft_GetFactoryNames;
	static StaticFuncThunk<vgui::Panel *, const char *>     ft_InstancePanel;
};


#endif
