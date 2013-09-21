///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 11 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __RUNFORDIALOGDECL_H__
#define __RUNFORDIALOGDECL_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/spinctrl.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/valgen.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class RunForDialogDecl
///////////////////////////////////////////////////////////////////////////////
class RunForDialogDecl : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText12;
		wxStaticText* m_staticText3;
		wxSpinCtrl* value_text;
		wxButton* okButton;
		wxButton* cancelButton;
        int value;

	public:

		RunForDialogDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Run For"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 288,144 ), long style = wxDEFAULT_DIALOG_STYLE );
		~RunForDialogDecl();

};

#endif //__RUNFORDIALOGDECL_H__
