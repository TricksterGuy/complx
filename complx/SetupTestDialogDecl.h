///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jan  1 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SETUPTESTDIALOGDECL_H__
#define __SETUPTESTDIALOGDECL_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/listbox.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SetupTestDialogDecl
///////////////////////////////////////////////////////////////////////////////
class SetupTestDialogDecl : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_panel2;
		wxStaticText* m_staticText11;
		wxChoice* tests;
		wxButton* m_button8;
		wxListBox* preconditions;
		wxListBox* postconditions;
		wxButton* m_button9;
		wxButton* m_button10;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnChooseTest( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnReloadXml( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		SetupTestDialogDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Setup Test"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 512,394 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~SetupTestDialogDecl();
	
};

#endif //__SETUPTESTDIALOGDECL_H__
