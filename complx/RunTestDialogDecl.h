///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 25 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __RUNTESTDIALOGDECL_H__
#define __RUNTESTDIALOGDECL_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/treectrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/listbox.h>
#include <wx/textctrl.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class RunTestDialogDecl
///////////////////////////////////////////////////////////////////////////////
class RunTestDialogDecl : public wxDialog 
{
	private:
		wxButton* m_button4;
		wxButton* m_button2;
	
	protected:
		wxSplitterWindow* infoSplitter;
		wxPanel* m_panel1;
		wxTreeCtrl* testTree;
		wxPanel* right;
		wxBoxSizer* testInfoSizer;
		wxButton* m_button5;
		wxButton* m_button51;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnSelectTestItem( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnTestReport( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnReport( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRunTest( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRunTests( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		RunTestDialogDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Run Tests"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 661,432 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~RunTestDialogDecl();
		
		void infoSplitterOnIdle( wxIdleEvent& )
		{
			infoSplitter->SetSashPosition( 191 );
			infoSplitter->Disconnect( wxEVT_IDLE, wxIdleEventHandler( RunTestDialogDecl::infoSplitterOnIdle ), NULL, this );
		}
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class TestSuiteInfoPanelDecl
///////////////////////////////////////////////////////////////////////////////
class TestSuiteInfoPanelDecl : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* passed;
		wxStaticText* testsPassed;
		wxStaticText* checksPassed;
		wxStaticText* pointsText;
		wxStaticText* points;
		wxListBox* tests;
	
	public:
		
		TestSuiteInfoPanelDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL ); 
		~TestSuiteInfoPanelDecl();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class TestCaseInfoPanelDecl
///////////////////////////////////////////////////////////////////////////////
class TestCaseInfoPanelDecl : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* testName;
		wxStaticText* checksPassed;
		wxStaticText* pointsText;
		wxStaticText* points;
		wxStaticText* executions;
		wxListBox* testInfo;
		wxListBox* testInputs;
		wxListBox* testOutputs;
		wxStaticText* halted;
		wxStaticText* warning;
		wxButton* warningButton;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnWarnings( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		TestCaseInfoPanelDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 516,442 ), long style = wxTAB_TRAVERSAL ); 
		~TestCaseInfoPanelDecl();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CheckInfoPanelDecl
///////////////////////////////////////////////////////////////////////////////
class CheckInfoPanelDecl : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* passed;
		wxStaticText* pointsText;
		wxStaticText* points;
		wxStaticText* condition;
		wxStaticText* expected;
		wxStaticText* actual;
		wxTextCtrl* extra_output;
	
	public:
		
		CheckInfoPanelDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL ); 
		~CheckInfoPanelDecl();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class TestReportDialogDecl
///////////////////////////////////////////////////////////////////////////////
class TestReportDialogDecl : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* name;
		wxTextCtrl* report;
	
	public:
		
		TestReportDialogDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Test Report"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 382,470 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~TestReportDialogDecl();
	
};

#endif //__RUNTESTDIALOGDECL_H__
