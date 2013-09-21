///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __COLORLCDGUI__
#define __COLORLCDGUI__

#include <wx/intl.h>

#include <wx/panel.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class COLORLCDGUI
///////////////////////////////////////////////////////////////////////////////
class COLORLCDGUI : public wxDialog 
{
	private:
	
	protected:
		wxPanel* displayPanel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnPaint( wxPaintEvent& event ) { event.Skip(); }
		
	
	public:
		
		COLORLCDGUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Display"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxDIALOG_NO_PARENT|wxRESIZE_BORDER );
		~COLORLCDGUI();
	
};

#endif //__COLORLCDGUI__
