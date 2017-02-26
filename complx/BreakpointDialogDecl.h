///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __BreakpointDialogDecl__
#define __BreakpointDialogDecl__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/valtext.h>
#include <wx/spinctrl.h>
#include <wx/valgen.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class BreakpointDialogDecl
///////////////////////////////////////////////////////////////////////////////
class BreakpointDialogDecl : public wxDialog
{
private:

protected:
    wxStaticText* m_staticText2;
    wxTextCtrl* name_text;
    wxStaticText* m_staticText1;
    wxTextCtrl* address_text;
    wxStaticText* m_staticText3;
    wxTextCtrl* condition_text;
    wxStaticText* m_staticText4;
    wxSpinCtrl* times_spin;
    wxStaticText* m_staticText5;
    wxSpinCtrl* hits_spin;
    wxCheckBox* enabled_check;


    wxButton* okButton;
    wxButton* cancelButton;

public:
    wxString name;
    wxString address;
    wxString condition;
    int times;
    int hits;
    bool enabled;

    BreakpointDialogDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Advanced Breakpoint"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 224,296 ), long style = wxDEFAULT_DIALOG_STYLE );
    ~BreakpointDialogDecl();

};

#endif //__BreakpointDialogDecl__
