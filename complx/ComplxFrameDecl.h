///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 30 2013)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __COMPLXFRAMEDECL_H__
#define __COMPLXFRAMEDECL_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/cshelp.h>
#include <wx/intl.h>
class MemoryGrid;

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/grid.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
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
		enum
		{
			ID_RANDOMIZE_AND_LOAD = 1000,
			ID_RANDOMIZE_AND_RELOAD,
			ID_LOAD,
			ID_RELOAD,
			ID_LOAD_OVER,
			ID_RELOAD_OVER,
			ID_LOAD_MACHINE,
			ID_SAVE_MACHINE,
			ID_QUIT,
			ID_NEW_VIEW,
			ID_GOTO_ADDRESS,
			ID_BASIC,
			ID_NORMAL,
			ID_HIGH_LEVEL,
			ID_INSTRUCTION_HIGHLIGHTING,
			ID_UNSIGNED_DECIMAL,
			ID_STEP,
			ID_BACK,
			ID_NEXT_LINE,
			ID_PREVIOUS_LINE,
			ID_RUN,
			ID_RUN_FOR,
			ID_RUN_AGAIN,
			ID_REWIND,
			ID_FINISH,
			ID_RANDOMIZE,
			ID_REINITIALIZE,
			ID_TRUE_TRAPS,
			ID_INTERRUPTS,
			ID_CLEAR_CONSOLE,
			ID_UNDO_STACK,
			ID_CALL_STACK,
			ID_BREAKPOINTS_AND_WATCHPOINTS,
			ID_ADD_TEMPORARY_BREAKPOINT,
			ID_ADD_BREAKPOINT,
			ID_ADD_WATCHPOINT,
			ID_ADVANCED_BREAKPOINT,
			ID_ADD_BLACKBOX,
			ID_RUN_TESTS,
			ID_RERUN_TESTS,
			ID_CHANGE_LOG,
			ID_CHECK_FOR_UPDATES,
			ID_FIRST_TIME_MESSAGE,
			ID_TIPS,
			ID_ABOUT
		};
		
		wxMenuBar* menu;
		wxMenu* menuFile;
		wxMenu* menuView;
		wxMenu* menuViewDisassemble;
		wxMenuItem* menuViewBasic;
		wxMenuItem* menuViewNormal;
		wxMenuItem* menuViewHighLevel;
		wxMenuItem* menuViewInstructionHighlighting;
		wxMenuItem* menuViewUnsignedDecimal;
		wxMenu* menuState;
		wxMenu* menuStateControl;
		wxMenuItem* menuStateTrueTraps;
		wxMenuItem* menuStateInterrupts;
		wxMenu* menuDebug;
		wxMenu* menuTest;
		wxMenu* menuHelp;
		wxPanel* mainPanel;
		MemoryGrid* memory;
		wxButton* runButton;
		wxButton* runForButton;
		wxButton* stepButton;
		wxButton* backStepButton;
		wxButton* nextLineButton;
		wxButton* prevLineButton;
		wxButton* finishButton;
		wxButton* rewindButton;
		wxTextCtrl* r0text;
		wxTextCtrl* r1text;
		wxTextCtrl* r2text;
		wxTextCtrl* r3text;
		wxTextCtrl* r4text;
		wxTextCtrl* r5text;
		wxTextCtrl* r6text;
		wxTextCtrl* r7text;
		wxTextCtrl* cctext;
		wxTextCtrl* pctext;
		wxStatusBar* statusBar;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnIdle( wxIdleEvent& event ) = 0;
		virtual void OnRandomizeAndLoad( wxCommandEvent& event ) = 0;
		virtual void OnRandomizeAndReload( wxCommandEvent& event ) = 0;
		virtual void OnLoad( wxCommandEvent& event ) = 0;
		virtual void OnReload( wxCommandEvent& event ) = 0;
		virtual void OnLoadOver( wxCommandEvent& event ) = 0;
		virtual void OnReloadOver( wxCommandEvent& event ) = 0;
		virtual void OnLoadMachine( wxCommandEvent& event ) = 0;
		virtual void OnSaveMachine( wxCommandEvent& event ) = 0;
		virtual void OnQuit( wxCommandEvent& event ) = 0;
		virtual void OnNewView( wxCommandEvent& event ) = 0;
		virtual void OnGoto( wxCommandEvent& event ) = 0;
		virtual void OnDumbDisassemble( wxCommandEvent& event ) = 0;
		virtual void OnNormalDisassemble( wxCommandEvent& event ) = 0;
		virtual void OnCDisassemble( wxCommandEvent& event ) = 0;
		virtual void OnInstructionHighlight( wxCommandEvent& event ) = 0;
		virtual void OnUnsignedDecimal( wxCommandEvent& event ) = 0;
		virtual void OnStep( wxCommandEvent& event ) = 0;
		virtual void OnBackStep( wxCommandEvent& event ) = 0;
		virtual void OnNextLine( wxCommandEvent& event ) = 0;
		virtual void OnPrevLine( wxCommandEvent& event ) = 0;
		virtual void OnRun( wxCommandEvent& event ) = 0;
		virtual void OnRunFor( wxCommandEvent& event ) = 0;
		virtual void OnRunAgain( wxCommandEvent& event ) = 0;
		virtual void OnRewind( wxCommandEvent& event ) = 0;
		virtual void OnFinish( wxCommandEvent& event ) = 0;
		virtual void OnRandomize( wxCommandEvent& event ) = 0;
		virtual void OnReinitialize( wxCommandEvent& event ) = 0;
		virtual void OnTrueTraps( wxCommandEvent& event ) = 0;
		virtual void OnInterrupts( wxCommandEvent& event ) = 0;
		virtual void OnClearConsole( wxCommandEvent& event ) = 0;
		virtual void OnClearConsoleInput( wxCommandEvent& event ) = 0;
		virtual void OnUndoStack( wxCommandEvent& event ) = 0;
		virtual void OnCallStack( wxCommandEvent& event ) = 0;
		virtual void OnBreakAndWatchpoints( wxCommandEvent& event ) = 0;
		virtual void OnTemppoint( wxCommandEvent& event ) = 0;
		virtual void OnBreakpoint( wxCommandEvent& event ) = 0;
		virtual void OnWatchpoint( wxCommandEvent& event ) = 0;
		virtual void OnAdvancedBreakpoint( wxCommandEvent& event ) = 0;
		virtual void OnBlackbox( wxCommandEvent& event ) = 0;
		virtual void OnRunTests( wxCommandEvent& event ) = 0;
		virtual void OnRerunTests( wxCommandEvent& event ) = 0;
		virtual void OnDocs( wxCommandEvent& event ) = 0;
		virtual void OnISA( wxCommandEvent& event ) = 0;
		virtual void OnChangeLog( wxCommandEvent& event ) = 0;
		virtual void OnCheckForUpdates( wxCommandEvent& event ) = 0;
		virtual void OnFirstTime( wxCommandEvent& event ) = 0;
		virtual void OnTips( wxCommandEvent& event ) = 0;
		virtual void OnAbout( wxCommandEvent& event ) = 0;
		virtual void OnTextKillFocus( wxFocusEvent& event ) = 0;
		virtual void OnBaseChange( wxMouseEvent& event ) = 0;
		virtual void OnRegisterChanged( wxCommandEvent& event ) = 0;
		
	
	public:
		
		ComplxFrameDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Complx LC-3 Simulator"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 704,576 ), long style = wxDEFAULT_FRAME_STYLE|wxRESIZE_BORDER|wxTAB_TRAVERSAL );
		
		~ComplxFrameDecl();
	
};

#endif //__COMPLXFRAMEDECL_H__
