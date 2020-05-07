///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
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
		wxStaticText* m_staticText101;
		wxChoice* subroutineMode;
		wxStaticText* m_staticText1;
		wxChoice* subroutine;
		wxPanel* callingConventionPanel;
		wxStaticText* m_staticText2;
		wxTextCtrl* stack;
		wxStaticText* m_staticText20;
		wxTextCtrl* r5ValueLCC;
		wxStaticText* m_staticText19;
		wxTextCtrl* r7ValueLCC;
		wxStaticText* m_staticText4;
		wxTextCtrl* params;
		wxPanel* passByRegistersPanel;
		wxStaticText* m_staticText111;
		wxTextCtrl* r0Value;
		wxStaticText* m_staticText112;
		wxTextCtrl* r1Value;
		wxStaticText* m_staticText13;
		wxTextCtrl* r2Value;
		wxStaticText* m_staticText14;
		wxTextCtrl* r3Value;
		wxStaticText* m_staticText15;
		wxTextCtrl* r4Value;
		wxStaticText* m_staticText16;
		wxTextCtrl* r5Value;
		wxStaticText* m_staticText17;
		wxTextCtrl* r6Value;
		wxStaticText* m_staticText18;
		wxTextCtrl* r7Value;
		wxButton* m_button1;
		wxButton* m_button2;

		// Virtual event handlers, overide them in your derived class
		virtual void OnSubroutineModeChange( wxCommandEvent& event ) { event.Skip(); }


	public:

		CallSubroutineDialogDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Simulate Subroutine Call"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 512,360 ), long style = wxDEFAULT_DIALOG_STYLE );
		~CallSubroutineDialogDecl();

};

