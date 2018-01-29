///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jan  1 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ADVANCEDLOADDIALOGDECL_H__
#define __ADVANCEDLOADDIALOGDECL_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/filepicker.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class AdvancedLoadDialogDecl
///////////////////////////////////////////////////////////////////////////////
class AdvancedLoadDialogDecl : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_panel2;
		wxStaticText* m_staticText1;
		wxFilePickerCtrl* assemblyFile;
		wxStaticText* m_staticText2;
		wxFilePickerCtrl* xmlFile;
		wxStaticText* m_staticText6;
		wxTextCtrl* consoleInput;
		wxStaticText* m_staticText0;
		wxChoice* regInitializer;
		wxTextCtrl* regFillValue;
		wxStaticText* m_staticText4;
		wxChoice* memInitializer;
		wxTextCtrl* memFillValue;
		wxStaticText* m_staticText5;
		wxTextCtrl* pcValue;
		wxStaticText* m_staticText7;
		wxCheckBox* trueTraps;
		wxStaticText* m_staticText8;
		wxCheckBox* interrupts;
		wxStaticText* m_staticText81;
		wxCheckBox* strictExecution;
		wxButton* m_button1;
		wxButton* m_button2;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnRegChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMemChoice( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		AdvancedLoadDialogDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Advanced Load"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 499,393 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~AdvancedLoadDialogDecl();
	
};

#endif //__ADVANCEDLOADDIALOGDECL_H__
