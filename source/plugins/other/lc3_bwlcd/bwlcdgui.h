///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __BWLCDGUI__
#define __BWLCDGUI__

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
/// Class BWLCDGUI
///////////////////////////////////////////////////////////////////////////////
class BWLCDGUI : public wxDialog
{
private:

protected:
    wxPanel* displayPanel;

    // Virtual event handlers, overide them in your derived class
    virtual void OnPaint( wxPaintEvent& event )
    {
        event.Skip();
    }


public:

    BWLCDGUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("BWDisplay"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxDIALOG_NO_PARENT|wxRESIZE_BORDER );
    ~BWLCDGUI();

};

#endif //__BWLCDGUI__
