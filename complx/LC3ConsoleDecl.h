///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct  8 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __LC3CONSOLEDECL_H__
#define __LC3CONSOLEDECL_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/valtext.h>
#include <wx/splitter.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class LC3ConsoleDecl
///////////////////////////////////////////////////////////////////////////////
class LC3ConsoleDecl : public wxFrame 
{
	private:
	
	protected:
		wxPanel* panel;
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panel2;
		wxTextCtrl* output;
		wxPanel* m_panel3;
		wxTextCtrl* input_text;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnText( wxCommandEvent& event ) = 0;
		virtual void OnEnterKey( wxCommandEvent& event ) = 0;
		
	
	public:
		wxString input; 
		
		LC3ConsoleDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Console"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 352,288 ), long style = wxCAPTION|wxICONIZE|wxMINIMIZE|wxMINIMIZE_BOX|wxTAB_TRAVERSAL );
		
		~LC3ConsoleDecl();
		
		void m_splitter1OnIdle( wxIdleEvent& )
		{
			m_splitter1->SetSashPosition( 224 );
			m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( LC3ConsoleDecl::m_splitter1OnIdle ), NULL, this );
		}
	
};

#endif //__LC3CONSOLEDECL_H__
