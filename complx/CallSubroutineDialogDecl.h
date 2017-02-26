///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 25 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __CALLSUBROUTINEDIALOGDECL_H__
#define __CALLSUBROUTINEDIALOGDECL_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CallSubroutineDialogDecl
///////////////////////////////////////////////////////////////////////////////
class CallSubroutineDialogDecl : public wxDialog
{
private:

protected:
    wxPanel* m_panel1;
    wxStaticText* m_staticText8;
    wxStaticText* m_staticText9;
    wxStaticText* m_staticText10;
    wxStaticText* m_staticText11;
    wxStaticText* m_staticText1;
    wxChoice* subroutine;
    wxStaticText* m_staticText2;
    wxTextCtrl* stack;
    wxStaticText* m_staticText4;
    wxTextCtrl* params;
    wxStaticText* m_staticText5;
    wxCheckBox* randomRegisters;
    wxStaticText* m_staticText6;
    wxCheckBox* randomMemory;
    wxButton* m_button1;
    wxButton* m_button2;

public:

    CallSubroutineDialogDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Simulate Subroutine Call"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 513,350 ), long style = wxDEFAULT_DIALOG_STYLE );
    ~CallSubroutineDialogDecl();

};

#endif //__CALLSUBROUTINEDIALOGDECL_H__
