///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
class MemoryView;

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/dataview.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/advprops.h>
#include <wx/textctrl.h>
#include <wx/valtext.h>
#include <wx/aui/auibook.h>
#include <wx/statusbr.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

#define ID_ADVANCED_LOAD 1000
#define ID_STEP 1001
#define ID_BACK 1002

///////////////////////////////////////////////////////////////////////////////
/// Class ComplxFrameDecl
///////////////////////////////////////////////////////////////////////////////
class ComplxFrameDecl : public wxFrame
{
	private:

	protected:
		wxMenuBar* menuBar;
		wxMenu* menuFile;
		wxMenu* menuView;
		wxMenu* menuControl;
		wxMenu* menuState;
		wxMenu* menuDebug;
		wxMenu* menuTest;
		wxMenu* menuHelp;
		wxPanel* toolbarPanel;
		wxAuiToolBar* m_auiToolBar1;
		wxButton* m_button2;
		wxButton* m_button3;
		wxButton* m_button4;
		wxButton* m_button5;
		wxButton* m_button6;
		wxButton* m_button7;
		wxButton* m_button8;
		wxAuiToolBar* m_auiToolBar2;
		wxButton* m_button10;
		wxButton* m_button11;
		wxPanel* memoryPanel;
		MemoryView* memoryView;
		wxPanel* statePanel;
		wxPropertyGridManager* statePropGridManager;
		wxPropertyGridPage* statePropGrid;
		wxPGProperty* registers;
		wxAuiNotebook* infoNotebook;
		wxPanel* consolePanel;
		wxTextCtrl* consoleText;
		wxTextCtrl* consoleInputText;
		wxPanel* warningPanel;
		wxTextCtrl* warningText;
		wxPanel* tracePanel;
		wxTextCtrl* traceText;
		wxPanel* loggingPanel;
		wxTextCtrl* loggingText;
		wxStatusBar* m_statusBar2;

		// Virtual event handlers, overide them in your derived class
		virtual void OnLoad( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnReload( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnExit( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStep( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBack( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStateChange( wxPropertyGridEvent& event ) { event.Skip(); }


	public:
		wxString consoleInput;

		ComplxFrameDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Complx2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1024,800 ), int64_t style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		wxAuiManager m_mgr;

		~ComplxFrameDecl();

};

