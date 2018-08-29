#ifndef _INCLUDE_SIGSEGV_UTIL_VGUI_H_
#define _INCLUDE_SIGSEGV_UTIL_VGUI_H_


#include "stub/vgui.h"
#include "util/autolist.h"


// be very careful:
// - the purpose of this is mainly for overriding virtual functions
// - only call parent virtual functions and/or use the global VGUI interfaces
class FrameWrapper : public vgui::Frame
{
public:
	// disallowed: using the constructor
	FrameWrapper()                    = delete;
	FrameWrapper(const FrameWrapper&) = delete;
	
	// disallowed: doing anything in the destructor
	// (unless we can find a non-ugly way to override the dtor vtable entry)
	virtual ~FrameWrapper() {}
	
	
	
	static FrameWrapper *Create(vgui::Panel *parent, const char *panelName);
	
private:
	void ModifyVTable();
	
	// disallowed: non-static data members
};
static_assert(sizeof(FrameWrapper) == sizeof(vgui::Frame));


class CustomFrame : public AutoList<CustomFrame>
{
public:
	CustomFrame(const char *name, const char *title);
	virtual ~CustomFrame();
	
	
	
	static void RemoveAll();
	
private:
	FrameWrapper *m_pFrame = nullptr;
	vgui::VPANEL m_VPanel = 0;
};


#endif
