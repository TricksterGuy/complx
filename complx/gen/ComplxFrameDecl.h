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
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/dataview.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/advprops.h>
#include <wx/textctrl.h>
#include <wx/aui/auibook.h>
#include <wx/statusbr.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ComplxFrameDecl
///////////////////////////////////////////////////////////////////////////////
class ComplxFrameDecl : public wxFrame
{
	private:

	protected:
		wxMenuBar* m_menubar3;
		wxMenu* menuFile;
		wxMenu* menuView;
		wxMenu* menuState;
		wxMenu* menuDebug;
		wxMenu* menuTest;
		wxMenu* menuHelp;
		wxPanel* toolbarPanel;
		wxAuiToolBar* m_auiToolBar1;
		wxAuiToolBarItem* m_tool1;
		wxAuiToolBarItem* m_tool2;
		wxAuiToolBarItem* m_tool3;
		wxAuiToolBarItem* m_tool4;
		wxAuiToolBarItem* m_tool5;
		wxAuiToolBarItem* m_tool6;
		wxAuiToolBarItem* m_tool7;
		wxAuiToolBarItem* m_tool8;
		wxPanel* memoryPanel;
		wxDataViewCtrl* memoryView;
		wxPanel* statePanel;
		wxPropertyGridManager* statePropGridManager;
		wxPropertyGridPage* statePropGrid;
		wxPGProperty* registers;
		wxAuiNotebook* infoNotebook;
		wxPanel* console;
		wxTextCtrl* m_textCtrl1;
		wxTextCtrl* m_textCtrl2;
		wxPanel* warnings;
		wxTextCtrl* warningsText;
		wxPanel* trace;
		wxTextCtrl* traceText;
		wxPanel* logging;
		wxTextCtrl* loggingText;
		wxStatusBar* m_statusBar2;

	public:

		ComplxFrameDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Complx"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 984,787 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		wxAuiManager m_mgr;

		~ComplxFrameDecl();

};

