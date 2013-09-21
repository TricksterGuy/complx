///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __MemoryViewFrameDecl__
#define __MemoryViewFrameDecl__

#include <wx/intl.h>

class MemoryGrid;

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/grid.h>
#include <wx/sizer.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class MemoryViewFrameDecl
///////////////////////////////////////////////////////////////////////////////
class MemoryViewFrameDecl : public wxFrame 
{
	private:
	
	protected:
		enum
		{
			ID_GOTO_ADDRESS = 1000,
		};
		
		wxMenuBar* menu;
		wxMenu* menuView;
		MemoryGrid* memory;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnGoto( wxCommandEvent& event ) = 0;
		
	
	public:
		
		MemoryViewFrameDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Memory View"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 608,352 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~MemoryViewFrameDecl();
	
};

#endif //__MemoryViewFrameDecl__
