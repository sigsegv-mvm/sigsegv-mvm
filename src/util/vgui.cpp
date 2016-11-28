#include "util/vgui.h"


// we have to use this stupid workaround to make MSVC do the member func ptr
// call properly due to vgui::Frame using virtual inheritance
class DummyClass {};

#if defined _MSC_VER
// there's a HIDDEN extra parameter in the ctor for classes with virtual
// inheritance on MSVC, which specifies whether the ctor is being called for the
// most-derived object (to tell it whether to initialize the vbtable at +0x4)
static MemberFuncThunk<DummyClass *, void, vgui::Panel *, const char *, bool, bool, bool> ft_Frame_ctor("[client] vgui::Frame::Frame");
void Frame_ctor(vgui::Frame *_this, vgui::Panel *parent, const char *panelName, bool showTaskbarIcon = true, bool bPopup = true) { ft_Frame_ctor(reinterpret_cast<DummyClass *>(_this), parent, panelName, showTaskbarIcon, bPopup, true); }
#else
static MemberFuncThunk<DummyClass *, void, vgui::Panel *, const char *, bool, bool> ft_Frame_ctor("[client] vgui::Frame::Frame");
void Frame_ctor(vgui::Frame *_this, vgui::Panel *parent, const char *panelName, bool showTaskbarIcon = true, bool bPopup = true) { ft_Frame_ctor(reinterpret_cast<DummyClass *>(_this), parent, panelName, showTaskbarIcon, bPopup); }
#endif


FrameWrapper *FrameWrapper::Create(vgui::Panel *parent, const char *panelName)
{
	auto pFrame = reinterpret_cast<FrameWrapper *>(::operator new(0x400));
	
	Frame_ctor(pFrame, parent, panelName);
	
//	pFrame->ModifyVTable();
	
	return pFrame;
}

void FrameWrapper::ModifyVTable()
{
	void ***ppVT = reinterpret_cast<void ***>(this);
	
	static void *mod_vtable[0x1000];
	static bool init = false;
	if (!init) {
		memcpy(mod_vtable, *ppVT, sizeof(mod_vtable));
		
//#if defined _WINDOWS
//		constexpr ptrdiff_t VTIDX_dtor = 0x007c / 4;
//		
//		(*ppVT)[VTIDX_dtor] = 
//#endif
		
		init = true;
	}
	
	*ppVT = mod_vtable;
}


CustomFrame::CustomFrame(const char *name, const char *title)
{
	this->m_pFrame = FrameWrapper::Create(g_pClientMode->GetViewport(), name);
	this->m_VPanel = this->m_pFrame->GetVPanel();
	
	this->m_pFrame->SetDeleteSelfOnClose(false);
	this->m_pFrame->SetScheme("ClientScheme.res");
	this->m_pFrame->SetTitle(title, true);
	
	this->m_pFrame->SetKeyBoardInputEnabled(false);
	this->m_pFrame->SetMouseInputEnabled   (false);
	
	this->m_pFrame->Activate();
}

CustomFrame::~CustomFrame()
{
	delete this->m_pFrame;
}


void CustomFrame::RemoveAll()
{
	while (!AutoList<CustomFrame>::List().empty()) {
		delete AutoList<CustomFrame>::List().front();
	}
}
