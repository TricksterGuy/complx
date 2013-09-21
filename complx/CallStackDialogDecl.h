///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 30 2013)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __CALLSTACKDIALOGDECL_H__
#define __CALLSTACKDIALOGDECL_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/valgen.h>
#include <wx/dynarray.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CallStackDialogDecl
///////////////////////////////////////////////////////////////////////////////
class CallStackDialogDecl : public wxDialog 
{
	private:
	
	protected:
		wxListBox* callList;
		wxButton* rewindButton;
		wxButton* viewStackButton;
		wxButton* closeButton;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnRewindToCall( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnViewStackFrame( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		wxArrayInt frame; 
		
		CallStackDialogDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Call Stack"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 431,365 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~CallStackDialogDecl();
	
};

#endif //__CALLSTACKDIALOGDECL_H__
