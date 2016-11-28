#include "stub/vgui.h"


//namespace vgui
//{
//	static MemberFuncThunk<vgui::Frame *, void, vgui::Panel *, const char *, bool, bool> ft_vgui_Frame_ctor("[client] vgui::Frame::Frame");
//	Frame::Frame(vgui::Panel *parent, const char *panelName, bool showTaskbarIcon, bool bPopup) { ft_vgui_Frame_ctor(this, parent, panelName, showTaskbarIcon, bPopup); }
//}


//GlobalThunk<IClientMode *> g_pClientMode("g_pClientMode");
//StaticFuncThunk<IClientMode *> ft_GetClientModeNormal("[client] GetClientModeNormal");


StaticFuncThunk<void, CUtlVector<const char *>&> CBuildFactoryHelper::ft_GetFactoryNames("[client] CBuildFactoryHelper::GetFactoryNames");
StaticFuncThunk<vgui::Panel *, const char *>     CBuildFactoryHelper::ft_InstancePanel  ("[client] CBuildFactoryHelper::InstancePanel");
