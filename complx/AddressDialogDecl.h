///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  4 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __AddressDialogDecl__
#define __AddressDialogDecl__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/valtext.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class AddressDialogDecl
///////////////////////////////////////////////////////////////////////////////
class AddressDialogDecl : public wxDialog
{
private:

protected:
    wxStaticText* m_staticText11;
    wxTextCtrl* address_text;
    wxStaticText* m_staticText12;
    wxTextCtrl* value_text;

    wxButton* okButton;

    wxButton* cancelButton;

public:
    wxString address;
    wxString value;

    AddressDialogDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Address"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 182,115 ), long style = wxDEFAULT_DIALOG_STYLE );
    ~AddressDialogDecl();

};

#endif //__AddressDialogDecl__
