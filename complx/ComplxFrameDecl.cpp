///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "MemoryGrid.hpp"

#include "ComplxFrameDecl.h"

// Using the construction of a static object to ensure that the help provider is set
// wx Manages the most recent HelpProvider passed to ::Set, but not the previous ones
// If ::Set gets called more than once, the previous one is returned and should be deleted
class wxFBContextSensitiveHelpSetter
{
public:
wxFBContextSensitiveHelpSetter()
{
auto* help = new wxSimpleHelpProvider();
wxHelpProvider* old = wxHelpProvider::Set( help );
if (old != 0){
delete old;
}
}
};

static wxFBContextSensitiveHelpSetter s_wxFBSetTheHelpProvider;
///////////////////////////////////////////////////////////////////////////

ComplxFrameDecl::ComplxFrameDecl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	menu = new wxMenuBar( 0 );
	menuFile = new wxMenu();
	wxMenuItem* menuFileLoad;
	menuFileLoad = new wxMenuItem( menuFile, ID_LOAD, wxString( _("&Load") ) + wxT('\t') + wxT("Ctrl+O"), _("Loads a asm file"), wxITEM_NORMAL );
	menuFile->Append( menuFileLoad );

	wxMenuItem* menuFileReload;
	menuFileReload = new wxMenuItem( menuFile, ID_RELOAD, wxString( _("&Reload") ) + wxT('\t') + wxT("Ctrl+R"), _("Reloads current asm file under same conditions."), wxITEM_NORMAL );
	menuFile->Append( menuFileReload );

	wxMenuItem* menuFileAdvancedLoad;
	menuFileAdvancedLoad = new wxMenuItem( menuFile, ID_ADVANCED_LOAD, wxString( _("&Advanced Load...") ) , _("Opens dialog for loading program under special conditions."), wxITEM_NORMAL );
	menuFile->Append( menuFileAdvancedLoad );

	menuFile->AppendSeparator();

	wxMenuItem* menuFileQuit;
	menuFileQuit = new wxMenuItem( menuFile, ID_QUIT, wxString( _("&Quit") ) + wxT('\t') + wxT("Ctrl+Q"), _("Quits the program."), wxITEM_NORMAL );
	menuFile->Append( menuFileQuit );

	menu->Append( menuFile, _("File") );

	menuView = new wxMenu();
	wxMenuItem* menuViewNew;
	menuViewNew = new wxMenuItem( menuView, ID_NEW_VIEW, wxString( _("New View") ) + wxT('\t') + wxT("Ctrl+V"), _("Shows a new view of main memory."), wxITEM_NORMAL );
	menuView->Append( menuViewNew );

	wxMenuItem* menuViewGoto;
	menuViewGoto = new wxMenuItem( menuView, ID_GOTO_ADDRESS, wxString( _("Goto Address") ) + wxT('\t') + wxT("Ctrl+G"), _("Moves memory to a specific address."), wxITEM_NORMAL );
	menuView->Append( menuViewGoto );

	menuViewHideAddresses = new wxMenu();
	wxMenuItem* menuViewHideAddressesItem = new wxMenuItem( menuView, wxID_ANY, _("Hide Addresses"), wxEmptyString, wxITEM_NORMAL, menuViewHideAddresses );
	menuViewHideAddressesShowAll = new wxMenuItem( menuViewHideAddresses, ID_SHOW_ALL, wxString( _("Show All") ) , _("Shows all memory addresses"), wxITEM_RADIO );
	menuViewHideAddresses->Append( menuViewHideAddressesShowAll );

	menuViewHideAddressesShowOnlyCodeData = new wxMenuItem( menuViewHideAddresses, ID_SHOW_ONLY_CODEDATA, wxString( _("Show Only Code/Data") ) , _("Shows addresses modified when your program was loaded."), wxITEM_RADIO );
	menuViewHideAddresses->Append( menuViewHideAddressesShowOnlyCodeData );

	menuViewHideAddressesCustom = new wxMenuItem( menuViewHideAddresses, ID_CUSTOM, wxString( _("Custom...") ) + wxT('\t') + wxT("Ctrl+H"), _("Allows you to show/hide memory address ranges"), wxITEM_RADIO );
	menuViewHideAddresses->Append( menuViewHideAddressesCustom );

	menuView->Append( menuViewHideAddressesItem );

	menuViewDisassemble = new wxMenu();
	wxMenuItem* menuViewDisassembleItem = new wxMenuItem( menuView, wxID_ANY, _("Disassemble"), wxEmptyString, wxITEM_NORMAL, menuViewDisassemble );
	menuViewBasic = new wxMenuItem( menuViewDisassemble, ID_BASIC, wxString( _("Basic") ) , _("Disassembles ignoring symbols"), wxITEM_RADIO );
	menuViewDisassemble->Append( menuViewBasic );

	menuViewNormal = new wxMenuItem( menuViewDisassemble, ID_NORMAL, wxString( _("Normal") ) , _("Disassembles with symbol info."), wxITEM_RADIO );
	menuViewDisassemble->Append( menuViewNormal );

	menuViewHighLevel = new wxMenuItem( menuViewDisassemble, ID_HIGH_LEVEL, wxString( _("High Level") ) , _("Disassembles into C-like statements"), wxITEM_RADIO );
	menuViewDisassemble->Append( menuViewHighLevel );
	menuViewHighLevel->Check( true );

	menuView->Append( menuViewDisassembleItem );

	menuViewInstructionHighlighting = new wxMenuItem( menuView, ID_INSTRUCTION_HIGHLIGHTING, wxString( _("Instruction Highlighting") ) , _("Highlights bits of the instruction."), wxITEM_CHECK );
	menuView->Append( menuViewInstructionHighlighting );
	menuViewInstructionHighlighting->Check( true );

	wxMenuItem* menuViewFlipMemory;
	menuViewFlipMemory = new wxMenuItem( menuView, ID_FLIP_MEMORY, wxString( _("Flip Memory") ) , _("Reverses memory such that high addresses come first."), wxITEM_CHECK );
	menuView->Append( menuViewFlipMemory );

	menu->Append( menuView, _("View") );

	menuState = new wxMenu();
	menuStateControl = new wxMenu();
	wxMenuItem* menuStateControlItem = new wxMenuItem( menuState, wxID_ANY, _("Control"), wxEmptyString, wxITEM_NORMAL, menuStateControl );
	wxMenuItem* menuStateControlStep;
	menuStateControlStep = new wxMenuItem( menuStateControl, ID_STEP, wxString( _("Step") ) + wxT('\t') + wxT("F2"), _("Runs one instruction"), wxITEM_NORMAL );
	menuStateControl->Append( menuStateControlStep );

	wxMenuItem* menuStateControlBack;
	menuStateControlBack = new wxMenuItem( menuStateControl, ID_BACK, wxString( _("Back") ) + wxT('\t') + wxT("Shift+F2"), _("Undoes one instruction"), wxITEM_NORMAL );
	menuStateControl->Append( menuStateControlBack );

	wxMenuItem* menuStateControlNextLine;
	menuStateControlNextLine = new wxMenuItem( menuStateControl, ID_NEXT_LINE, wxString( _("Next Line") ) + wxT('\t') + wxT("F3"), _("Steps over the next instruction."), wxITEM_NORMAL );
	menuStateControl->Append( menuStateControlNextLine );

	wxMenuItem* menuStateControlPreviousLine;
	menuStateControlPreviousLine = new wxMenuItem( menuStateControl, ID_PREVIOUS_LINE, wxString( _("Previous Line") ) + wxT('\t') + wxT("Shift+F3"), _("Steps over the previous instruction."), wxITEM_NORMAL );
	menuStateControl->Append( menuStateControlPreviousLine );

	wxMenuItem* menuStateControlRun;
	menuStateControlRun = new wxMenuItem( menuStateControl, ID_RUN, wxString( _("Run") ) + wxT('\t') + wxT("F4"), _("Runs until program halts."), wxITEM_NORMAL );
	menuStateControl->Append( menuStateControlRun );

	wxMenuItem* menuStateControlRunFor;
	menuStateControlRunFor = new wxMenuItem( menuStateControl, ID_RUN_FOR, wxString( _("Run For...") ) + wxT('\t') + wxT("Ctrl+F4"), _("Runs for a specified number of steps"), wxITEM_NORMAL );
	menuStateControl->Append( menuStateControlRunFor );

	wxMenuItem* menuStateControlRunAgain;
	menuStateControlRunAgain = new wxMenuItem( menuStateControl, ID_RUN_AGAIN, wxString( _("Run Again...") ) + wxT('\t') + wxT("Ctrl+Space"), _("Runs the default number of instructions (see RunFor)"), wxITEM_NORMAL );
	menuStateControl->Append( menuStateControlRunAgain );

	wxMenuItem* menuStateControlRewind;
	menuStateControlRewind = new wxMenuItem( menuStateControl, ID_REWIND, wxString( _("Rewind") ) + wxT('\t') + wxT("Shift+F4"), _("Undoes all instructions."), wxITEM_NORMAL );
	menuStateControl->Append( menuStateControlRewind );

	wxMenuItem* menuStateControlFinish;
	menuStateControlFinish = new wxMenuItem( menuStateControl, ID_FINISH, wxString( _("Finish") ) + wxT('\t') + wxT("Shift+F5"), _("Finishes current subroutine."), wxITEM_NORMAL );
	menuStateControl->Append( menuStateControlFinish );

	menuState->Append( menuStateControlItem );

	menuStateControlMode = new wxMenu();
	wxMenuItem* menuStateControlModeItem = new wxMenuItem( menuState, wxID_ANY, _("Control Mode"), wxEmptyString, wxITEM_NORMAL, menuStateControlMode );
	wxMenuItem* menuStateControlModeSimple;
	menuStateControlModeSimple = new wxMenuItem( menuStateControlMode, ID_SIMPLE, wxString( _("Simple") ) , _("Show commonly used control buttons."), wxITEM_RADIO );
	menuStateControlMode->Append( menuStateControlModeSimple );

	wxMenuItem* menuStateControlModeAdvanced;
	menuStateControlModeAdvanced = new wxMenuItem( menuStateControlMode, ID_ADVANCED, wxString( _("Advanced") ) , _("Show control buttons related to subroutines."), wxITEM_RADIO );
	menuStateControlMode->Append( menuStateControlModeAdvanced );

	menuState->Append( menuStateControlModeItem );

	menuState->AppendSeparator();

	menuStateTrueTraps = new wxMenuItem( menuState, ID_TRUE_TRAPS, wxString( _("&True Traps") ) , _("Enables True Traps"), wxITEM_CHECK );
	menuState->Append( menuStateTrueTraps );

	menuStateInterrupts = new wxMenuItem( menuState, ID_INTERRUPTS, wxString( _("&Interrupts") ) , _("Enables Interrupts"), wxITEM_CHECK );
	menuState->Append( menuStateInterrupts );

	menuStateStrictExecution = new wxMenuItem( menuState, ID_STRICT_EXECUTION_MODE, wxString( _("&Strict Execution Mode") ) + wxT('\t') + wxT("Ctrl+S"), _("Enables strict execution mode."), wxITEM_CHECK );
	menuState->Append( menuStateStrictExecution );
	menuStateStrictExecution->Check( true );

	menuState->AppendSeparator();

	wxMenuItem* menuStateClearConsole;
	menuStateClearConsole = new wxMenuItem( menuState, ID_CLEAR_CONSOLE, wxString( _("&Clear Console") ) , _("Clears the console output."), wxITEM_NORMAL );
	menuState->Append( menuStateClearConsole );

	wxMenuItem* menuStateClearConsoleInput;
	menuStateClearConsoleInput = new wxMenuItem( menuState, wxID_ANY, wxString( _("Clear Console Input") ) , _("Clears the console input."), wxITEM_NORMAL );
	menuState->Append( menuStateClearConsoleInput );

	menu->Append( menuState, _("State") );

	menuDebug = new wxMenu();
	wxMenuItem* menuDebugUndoStack;
	menuDebugUndoStack = new wxMenuItem( menuDebug, ID_UNDO_STACK, wxString( _("&Undo Stack") ) + wxT('\t') + wxT("F6"), _("Modifies the undo stack size"), wxITEM_NORMAL );
	menuDebug->Append( menuDebugUndoStack );

	wxMenuItem* menuDebugCallStack;
	menuDebugCallStack = new wxMenuItem( menuDebug, ID_CALL_STACK, wxString( _("&Call Stack") ) , _("Views the call stack."), wxITEM_NORMAL );
	menuDebug->Append( menuDebugCallStack );

	menuDebug->AppendSeparator();

	wxMenuItem* menuDebugSimulateSubroutineCall;
	menuDebugSimulateSubroutineCall = new wxMenuItem( menuDebug, ID_SIMULATE_SUBROUTINE_CALL, wxString( _("Simulate Subroutine &Call...") ) , _("Simulates a subroutine call."), wxITEM_NORMAL );
	menuDebug->Append( menuDebugSimulateSubroutineCall );

	menuDebug->AppendSeparator();

	wxMenuItem* menuDebugBreakWatchpoints;
	menuDebugBreakWatchpoints = new wxMenuItem( menuDebug, ID_BREAKPOINTS_AND_WATCHPOINTS, wxString( _("Breakpoints &and Watchpoints") ) + wxT('\t') + wxT("F7"), _("View all breakpoints and watchpoints"), wxITEM_NORMAL );
	menuDebug->Append( menuDebugBreakWatchpoints );

	wxMenuItem* menuDebugTemporary;
	menuDebugTemporary = new wxMenuItem( menuDebug, ID_ADD_TEMPORARY_BREAKPOINT, wxString( _("Add &Temporary Breakpoint") ) + wxT('\t') + wxT("F8"), _("Adds/Removes a one time breakpoint"), wxITEM_NORMAL );
	menuDebug->Append( menuDebugTemporary );

	wxMenuItem* menuDebugBreakpoint;
	menuDebugBreakpoint = new wxMenuItem( menuDebug, ID_ADD_BREAKPOINT, wxString( _("Add &Breakpoint") ) + wxT('\t') + wxT("F9"), _("Adds/Removes a breakpoint"), wxITEM_NORMAL );
	menuDebug->Append( menuDebugBreakpoint );

	wxMenuItem* menuDebugWatchpoint;
	menuDebugWatchpoint = new wxMenuItem( menuDebug, ID_ADD_WATCHPOINT, wxString( _("Add &Watchpoint") ) + wxT('\t') + wxT("F11"), _("Adds a watchpoint."), wxITEM_NORMAL );
	menuDebug->Append( menuDebugWatchpoint );

	wxMenuItem* menuDebugAdvanced;
	menuDebugAdvanced = new wxMenuItem( menuDebug, ID_ADVANCED_BREAKPOINT, wxString( _("Ad&vanced Breakpoint") ) + wxT('\t') + wxT("F12"), _("Adds a customizable breakpoint."), wxITEM_NORMAL );
	menuDebug->Append( menuDebugAdvanced );

	wxMenuItem* menuDebugBlackbox;
	menuDebugBlackbox = new wxMenuItem( menuDebug, ID_ADD_BLACKBOX, wxString( _("Add B&lackbox") ) + wxT('\t') + wxT("F5"), _("Adds a blackbox."), wxITEM_NORMAL );
	menuDebug->Append( menuDebugBlackbox );

	menuDebug->AppendSeparator();

	wxMenuItem* menuDebugTraceFile;
	menuDebugTraceFile = new wxMenuItem( menuDebug, ID_TRACE_FILE, wxString( _("Trace File...") ) , _("Sets a trace file to output state to."), wxITEM_NORMAL );
	menuDebug->Append( menuDebugTraceFile );

	menu->Append( menuDebug, _("Debug") );

	menuTest = new wxMenu();
	wxMenuItem* menuTestSetupReplayString;
	menuTestSetupReplayString = new wxMenuItem( menuTest, ID_SETUP_REPLAY_STRING, wxString( _("Se&tup Replay String...") ) + wxT('\t') + wxT("Ctrl+T"), _("Loads a replay string from the pylc3 unit test framework."), wxITEM_NORMAL );
	menuTest->Append( menuTestSetupReplayString );

	wxMenuItem* menuTestReloadReplayString;
	menuTestReloadReplayString = new wxMenuItem( menuTest, ID_RELOAD_REPLAY_STRING, wxString( _("&Reload Replay String") ) + wxT('\t') + wxT("Ctrl+Alt+T"), _("Reloads a replay string from the pylc3 unit test framework."), wxITEM_NORMAL );
	menuTest->Append( menuTestReloadReplayString );

	menu->Append( menuTest, _("Test") );

	menuHelp = new wxMenu();
	wxMenuItem* menuHelpDocs;
	menuHelpDocs = new wxMenuItem( menuHelp, wxID_ANY, wxString( _("&Documentation") ) + wxT('\t') + wxT("F1"), _("Opens up the Complx Documentation."), wxITEM_NORMAL );
	menuHelp->Append( menuHelpDocs );

	wxMenuItem* menuHelpCreateBugReport;
	menuHelpCreateBugReport = new wxMenuItem( menuHelp, ID_CREATE_BUG_REPORT, wxString( _("Create &Bug Report") ) , _("Create a bug report"), wxITEM_NORMAL );
	menuHelp->Append( menuHelpCreateBugReport );

	wxMenuItem* menuHelpFirstTime;
	menuHelpFirstTime = new wxMenuItem( menuHelp, ID_FIRST_TIME_MESSAGE, wxString( _("&First Time Message") ) , _("Displays the first time running message."), wxITEM_NORMAL );
	menuHelp->Append( menuHelpFirstTime );

	wxMenuItem* menuHelpTips;
	menuHelpTips = new wxMenuItem( menuHelp, ID_TIPS, wxString( _("Tips") ) , _("Displays the tips"), wxITEM_NORMAL );
	menuHelp->Append( menuHelpTips );

	wxMenuItem* menuHelpAbout;
	menuHelpAbout = new wxMenuItem( menuHelp, ID_ABOUT, wxString( _("&About") ) , _("Shows info about this program"), wxITEM_NORMAL );
	menuHelp->Append( menuHelpAbout );

	menu->Append( menuHelp, _("Help") );

	this->SetMenuBar( menu );

	wxBoxSizer* main;
	main = new wxBoxSizer( wxVERTICAL );

	mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* subsizer;
	subsizer = new wxBoxSizer( wxVERTICAL );

	memory = new MemoryGrid( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxWANTS_CHARS );

	// Grid
	memory->CreateGrid( 5, 5 );
	memory->EnableEditing( true );
	memory->EnableGridLines( false );
	memory->EnableDragGridSize( false );
	memory->SetMargins( 0, 0 );

	// Columns
	memory->EnableDragColMove( true );
	memory->EnableDragColSize( true );
	memory->SetColLabelSize( 20 );
	memory->SetColLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Rows
	memory->EnableDragRowSize( false );
	memory->SetRowLabelSize( 0 );
	memory->SetRowLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Label Appearance
	memory->SetLabelFont( wxFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	// Cell Defaults
	memory->SetDefaultCellFont( wxFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );
	memory->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_CENTER );
	memory->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	subsizer->Add( memory, 1, wxEXPAND, 5 );

	controlSizer = new wxBoxSizer( wxHORIZONTAL );

	runButton = new wxButton( mainPanel, wxID_ANY, _("&Run"), wxDefaultPosition, wxDefaultSize, 0 );
	runButton->SetToolTip( _("Runs your code until it HALTs") );

	controlSizer->Add( runButton, 1, wxALL|wxEXPAND, 2 );

	stepButton = new wxButton( mainPanel, wxID_ANY, _("&Step"), wxDefaultPosition, wxDefaultSize, 0 );
	stepButton->SetToolTip( _("Runs one instruction") );

	controlSizer->Add( stepButton, 1, wxALL|wxEXPAND, 2 );

	backStepButton = new wxButton( mainPanel, wxID_ANY, _("&Back"), wxDefaultPosition, wxDefaultSize, 0 );
	backStepButton->SetToolTip( _("Undoes one instruction") );

	controlSizer->Add( backStepButton, 1, wxALL|wxEXPAND, 2 );

	nextLineButton = new wxButton( mainPanel, wxID_ANY, _("&Next Line"), wxDefaultPosition, wxDefaultSize, 0 );
	nextLineButton->Hide();
	nextLineButton->SetToolTip( _("Run the next instruction (blackboxes subroutines)") );

	controlSizer->Add( nextLineButton, 1, wxALL|wxEXPAND, 2 );

	prevLineButton = new wxButton( mainPanel, wxID_ANY, _("&Prev Line"), wxDefaultPosition, wxDefaultSize, 0 );
	prevLineButton->Hide();
	prevLineButton->SetToolTip( _("Undoes the last instruction (blackboxing subroutines)") );

	controlSizer->Add( prevLineButton, 1, wxALL|wxEXPAND, 2 );

	finishButton = new wxButton( mainPanel, wxID_ANY, _("&Finish"), wxDefaultPosition, wxDefaultSize, 0 );
	finishButton->Hide();
	finishButton->SetToolTip( _("Steps out of the current subroutine.") );

	controlSizer->Add( finishButton, 1, wxALL|wxEXPAND, 2 );

	rewindButton = new wxButton( mainPanel, wxID_ANY, _("Rewin&d"), wxDefaultPosition, wxDefaultSize, 0 );
	rewindButton->SetToolTip( _("Rewinds back to start (or undoes all of the instructions in the stack)") );

	controlSizer->Add( rewindButton, 1, wxALL|wxEXPAND, 2 );


	subsizer->Add( controlSizer, 0, wxEXPAND, 5 );

	wxFlexGridSizer* stateSizer;
	stateSizer = new wxFlexGridSizer( 3, 4, 8, 16 );
	stateSizer->AddGrowableCol( 0 );
	stateSizer->AddGrowableCol( 1 );
	stateSizer->AddGrowableCol( 2 );
	stateSizer->AddGrowableCol( 3 );
	stateSizer->SetFlexibleDirection( wxBOTH );
	stateSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxBoxSizer* R0;
	R0 = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText* r0label;
	r0label = new wxStaticText( mainPanel, wxID_ANY, _("R0"), wxDefaultPosition, wxDefaultSize, 0 );
	r0label->Wrap( -1 );
	R0->Add( r0label, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT, 2 );

	r0text = new wxTextCtrl( mainPanel, wxID_ANY, _("x0000"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_PROCESS_ENTER|wxTE_RIGHT, wxDefaultValidator, wxT("R0") );
	#ifdef __WXGTK__
	if ( !r0text->HasFlag( wxTE_MULTILINE ) )
	{
	r0text->SetMaxLength( 24 );
	}
	#else
	r0text->SetMaxLength( 24 );
	#endif
	r0text->SetToolTip( _("R0's value.\n (enter an expression i.e. R0+5 or PC+10 or LABEL[0])\n Double click to change the base.") );

	R0->Add( r0text, 1, wxALIGN_CENTER|wxLEFT|wxRIGHT, 2 );


	stateSizer->Add( R0, 1, wxEXPAND, 0 );

	wxBoxSizer* R1;
	R1 = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText* r1abel;
	r1abel = new wxStaticText( mainPanel, wxID_ANY, _("R1"), wxDefaultPosition, wxDefaultSize, 0 );
	r1abel->Wrap( -1 );
	R1->Add( r1abel, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT, 2 );

	r1text = new wxTextCtrl( mainPanel, wxID_ANY, _("x0000"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_PROCESS_ENTER|wxTE_RIGHT, wxDefaultValidator, wxT("R1") );
	#ifdef __WXGTK__
	if ( !r1text->HasFlag( wxTE_MULTILINE ) )
	{
	r1text->SetMaxLength( 24 );
	}
	#else
	r1text->SetMaxLength( 24 );
	#endif
	r1text->SetToolTip( _("R1's value.\n (enter an expression i.e. R0+5 or PC+10 or LABEL[0])\n Double click to change the base.") );

	R1->Add( r1text, 1, wxALIGN_CENTER|wxLEFT|wxRIGHT, 2 );


	stateSizer->Add( R1, 1, wxEXPAND, 5 );

	wxBoxSizer* R2;
	R2 = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText* r2label;
	r2label = new wxStaticText( mainPanel, wxID_ANY, _("R2"), wxDefaultPosition, wxDefaultSize, 0 );
	r2label->Wrap( -1 );
	R2->Add( r2label, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT, 2 );

	r2text = new wxTextCtrl( mainPanel, wxID_ANY, _("x0000"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_PROCESS_ENTER|wxTE_RIGHT, wxDefaultValidator, wxT("R2") );
	#ifdef __WXGTK__
	if ( !r2text->HasFlag( wxTE_MULTILINE ) )
	{
	r2text->SetMaxLength( 24 );
	}
	#else
	r2text->SetMaxLength( 24 );
	#endif
	r2text->SetToolTip( _("R2's value.\n (enter an expression i.e. R0+5 or PC+10 or LABEL[0])\n Double click to change the base.") );

	R2->Add( r2text, 1, wxALIGN_CENTER|wxLEFT|wxRIGHT, 2 );


	stateSizer->Add( R2, 1, wxEXPAND, 5 );

	wxBoxSizer* R3;
	R3 = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText* r3label;
	r3label = new wxStaticText( mainPanel, wxID_ANY, _("R3"), wxDefaultPosition, wxDefaultSize, 0 );
	r3label->Wrap( -1 );
	R3->Add( r3label, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT, 2 );

	r3text = new wxTextCtrl( mainPanel, wxID_ANY, _("x0000"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_PROCESS_ENTER|wxTE_RIGHT, wxDefaultValidator, wxT("R3") );
	#ifdef __WXGTK__
	if ( !r3text->HasFlag( wxTE_MULTILINE ) )
	{
	r3text->SetMaxLength( 24 );
	}
	#else
	r3text->SetMaxLength( 24 );
	#endif
	r3text->SetToolTip( _("R3's value.\n (enter an expression i.e. R0+5 or PC+10 or LABEL[0])\n Double click to change the base.") );

	R3->Add( r3text, 1, wxALIGN_CENTER|wxLEFT|wxRIGHT, 2 );


	stateSizer->Add( R3, 1, wxEXPAND, 5 );

	wxBoxSizer* R4;
	R4 = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText* r4label;
	r4label = new wxStaticText( mainPanel, wxID_ANY, _("R4"), wxDefaultPosition, wxDefaultSize, 0 );
	r4label->Wrap( -1 );
	R4->Add( r4label, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT, 2 );

	r4text = new wxTextCtrl( mainPanel, wxID_ANY, _("x0000"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_PROCESS_ENTER|wxTE_RIGHT, wxDefaultValidator, wxT("R4") );
	#ifdef __WXGTK__
	if ( !r4text->HasFlag( wxTE_MULTILINE ) )
	{
	r4text->SetMaxLength( 24 );
	}
	#else
	r4text->SetMaxLength( 24 );
	#endif
	r4text->SetToolTip( _("R4's value.\n (enter an expression i.e. R0+5 or PC+10 or LABEL[0])\n Double click to change the base.") );

	R4->Add( r4text, 1, wxALIGN_CENTER|wxLEFT|wxRIGHT, 2 );


	stateSizer->Add( R4, 1, wxEXPAND, 5 );

	wxBoxSizer* R5;
	R5 = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText* r5label;
	r5label = new wxStaticText( mainPanel, wxID_ANY, _("R5"), wxDefaultPosition, wxDefaultSize, 0 );
	r5label->Wrap( -1 );
	R5->Add( r5label, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT, 2 );

	r5text = new wxTextCtrl( mainPanel, wxID_ANY, _("x0000"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_PROCESS_ENTER|wxTE_RIGHT, wxDefaultValidator, wxT("R5") );
	#ifdef __WXGTK__
	if ( !r5text->HasFlag( wxTE_MULTILINE ) )
	{
	r5text->SetMaxLength( 24 );
	}
	#else
	r5text->SetMaxLength( 24 );
	#endif
	r5text->SetToolTip( _("R5's value.\n (enter an expression i.e. R0+5 or PC+10 or LABEL[0])\n Double click to change the base.") );

	R5->Add( r5text, 1, wxALIGN_CENTER|wxLEFT|wxRIGHT, 2 );


	stateSizer->Add( R5, 1, wxEXPAND, 5 );

	wxBoxSizer* R6;
	R6 = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText* r6label;
	r6label = new wxStaticText( mainPanel, wxID_ANY, _("R6"), wxDefaultPosition, wxDefaultSize, 0 );
	r6label->Wrap( -1 );
	R6->Add( r6label, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT, 2 );

	r6text = new wxTextCtrl( mainPanel, wxID_ANY, _("x0000"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_PROCESS_ENTER|wxTE_RIGHT, wxDefaultValidator, wxT("R6") );
	#ifdef __WXGTK__
	if ( !r6text->HasFlag( wxTE_MULTILINE ) )
	{
	r6text->SetMaxLength( 24 );
	}
	#else
	r6text->SetMaxLength( 24 );
	#endif
	r6text->SetToolTip( _("R6's value.\n (enter an expression i.e. R0+5 or PC+10 or LABEL[0])\n Double click to change the base.") );

	R6->Add( r6text, 1, wxALIGN_CENTER|wxLEFT|wxRIGHT, 2 );


	stateSizer->Add( R6, 1, wxEXPAND, 5 );

	wxBoxSizer* R7;
	R7 = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText* r7label;
	r7label = new wxStaticText( mainPanel, wxID_ANY, _("R7"), wxDefaultPosition, wxDefaultSize, 0 );
	r7label->Wrap( -1 );
	R7->Add( r7label, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT, 2 );

	r7text = new wxTextCtrl( mainPanel, wxID_ANY, _("x0000"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_PROCESS_ENTER|wxTE_RIGHT, wxDefaultValidator, wxT("R7") );
	#ifdef __WXGTK__
	if ( !r7text->HasFlag( wxTE_MULTILINE ) )
	{
	r7text->SetMaxLength( 24 );
	}
	#else
	r7text->SetMaxLength( 24 );
	#endif
	r7text->SetToolTip( _("R7's value.\n (enter an expression i.e. R0+5 or PC+10 or LABEL[0])\n Double click to change the base.") );

	R7->Add( r7text, 1, wxALIGN_CENTER|wxLEFT|wxRIGHT, 2 );


	stateSizer->Add( R7, 1, wxEXPAND, 5 );

	wxBoxSizer* CC;
	CC = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText* cclabel;
	cclabel = new wxStaticText( mainPanel, wxID_ANY, _("CC"), wxDefaultPosition, wxDefaultSize, 0 );
	cclabel->Wrap( -1 );
	CC->Add( cclabel, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT, 2 );

	cctext = new wxTextCtrl( mainPanel, wxID_ANY, _("z"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_PROCESS_ENTER|wxTE_RIGHT, wxDefaultValidator, wxT("CC") );
	#ifdef __WXGTK__
	if ( !cctext->HasFlag( wxTE_MULTILINE ) )
	{
	cctext->SetMaxLength( 24 );
	}
	#else
	cctext->SetMaxLength( 24 );
	#endif
	cctext->SetToolTip( _("CC's state.\n (enter an expression i.e. R0+5 or PC+10 or LABEL[0])\n Double click to change the base.") );

	CC->Add( cctext, 1, wxALIGN_CENTER|wxLEFT|wxRIGHT, 2 );


	stateSizer->Add( CC, 1, wxEXPAND, 5 );

	wxBoxSizer* PC;
	PC = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText* pclabel;
	pclabel = new wxStaticText( mainPanel, wxID_ANY, _("PC"), wxDefaultPosition, wxDefaultSize, 0 );
	pclabel->Wrap( -1 );
	PC->Add( pclabel, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT, 2 );

	pctext = new wxTextCtrl( mainPanel, wxID_ANY, _("x0000"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_PROCESS_ENTER|wxTE_RIGHT, wxDefaultValidator, wxT("PC") );
	#ifdef __WXGTK__
	if ( !pctext->HasFlag( wxTE_MULTILINE ) )
	{
	pctext->SetMaxLength( 24 );
	}
	#else
	pctext->SetMaxLength( 24 );
	#endif
	pctext->SetToolTip( _("PC's value.\n (enter an expression i.e. R0+5 or PC+10 or LABEL[0])\n Double click to change the base.") );

	PC->Add( pctext, 1, wxALIGN_CENTER|wxLEFT|wxRIGHT, 2 );


	stateSizer->Add( PC, 1, wxEXPAND, 5 );


	stateSizer->Add( 0, 0, 1, wxEXPAND, 5 );


	stateSizer->Add( 0, 0, 1, wxEXPAND, 5 );


	subsizer->Add( stateSizer, 0, wxEXPAND, 5 );


	mainPanel->SetSizer( subsizer );
	mainPanel->Layout();
	subsizer->Fit( mainPanel );
	main->Add( mainPanel, 1, wxEXPAND | wxALL, 0 );


	this->SetSizer( main );
	this->Layout();
	statusBar = this->CreateStatusBar( 5, wxSTB_SIZEGRIP, wxID_ANY );

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_ACTIVATE, wxActivateEventHandler( ComplxFrameDecl::OnActivate ) );
	this->Connect( wxEVT_IDLE, wxIdleEventHandler( ComplxFrameDecl::OnIdle ) );
	menuFile->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnLoad ), this, menuFileLoad->GetId());
	menuFile->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnReload ), this, menuFileReload->GetId());
	menuFile->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnAdvancedLoad ), this, menuFileAdvancedLoad->GetId());
	menuFile->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnQuit ), this, menuFileQuit->GetId());
	menuView->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnNewView ), this, menuViewNew->GetId());
	menuView->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnGoto ), this, menuViewGoto->GetId());
	menuViewHideAddresses->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnUpdateHideAddresses ), this, menuViewHideAddressesShowAll->GetId());
	menuViewHideAddresses->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnUpdateHideAddresses ), this, menuViewHideAddressesShowOnlyCodeData->GetId());
	menuViewHideAddresses->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnHideAddressesCustom ), this, menuViewHideAddressesCustom->GetId());
	menuViewDisassemble->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnDumbDisassemble ), this, menuViewBasic->GetId());
	menuViewDisassemble->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnNormalDisassemble ), this, menuViewNormal->GetId());
	menuViewDisassemble->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnCDisassemble ), this, menuViewHighLevel->GetId());
	menuView->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnInstructionHighlight ), this, menuViewInstructionHighlighting->GetId());
	menuView->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnFlipMemory ), this, menuViewFlipMemory->GetId());
	menuStateControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnStep ), this, menuStateControlStep->GetId());
	menuStateControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnBackStep ), this, menuStateControlBack->GetId());
	menuStateControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnNextLine ), this, menuStateControlNextLine->GetId());
	menuStateControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnPrevLine ), this, menuStateControlPreviousLine->GetId());
	menuStateControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRun ), this, menuStateControlRun->GetId());
	menuStateControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRunFor ), this, menuStateControlRunFor->GetId());
	menuStateControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRunAgain ), this, menuStateControlRunAgain->GetId());
	menuStateControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRewind ), this, menuStateControlRewind->GetId());
	menuStateControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnFinish ), this, menuStateControlFinish->GetId());
	menuStateControlMode->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnControlModeSimple ), this, menuStateControlModeSimple->GetId());
	menuStateControlMode->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnControlModeAdvanced ), this, menuStateControlModeAdvanced->GetId());
	menuState->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnTrueTraps ), this, menuStateTrueTraps->GetId());
	menuState->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnInterrupts ), this, menuStateInterrupts->GetId());
	menuState->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnStrictExecution ), this, menuStateStrictExecution->GetId());
	menuState->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnClearConsole ), this, menuStateClearConsole->GetId());
	menuState->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnClearConsoleInput ), this, menuStateClearConsoleInput->GetId());
	menuDebug->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnUndoStack ), this, menuDebugUndoStack->GetId());
	menuDebug->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnCallStack ), this, menuDebugCallStack->GetId());
	menuDebug->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnSubroutineCall ), this, menuDebugSimulateSubroutineCall->GetId());
	menuDebug->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnBreakAndWatchpoints ), this, menuDebugBreakWatchpoints->GetId());
	menuDebug->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnTemppoint ), this, menuDebugTemporary->GetId());
	menuDebug->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnBreakpoint ), this, menuDebugBreakpoint->GetId());
	menuDebug->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnWatchpoint ), this, menuDebugWatchpoint->GetId());
	menuDebug->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnAdvancedBreakpoint ), this, menuDebugAdvanced->GetId());
	menuDebug->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnBlackbox ), this, menuDebugBlackbox->GetId());
	menuDebug->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnTraceFile ), this, menuDebugTraceFile->GetId());
	menuTest->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnSetupReplayString ), this, menuTestSetupReplayString->GetId());
	menuTest->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnReloadReplayString ), this, menuTestReloadReplayString->GetId());
	menuHelp->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnDocs ), this, menuHelpDocs->GetId());
	menuHelp->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnCreateBugReport ), this, menuHelpCreateBugReport->GetId());
	menuHelp->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnFirstTime ), this, menuHelpFirstTime->GetId());
	menuHelp->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnTips ), this, menuHelpTips->GetId());
	menuHelp->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnAbout ), this, menuHelpAbout->GetId());
	runButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnRun ), NULL, this );
	stepButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnStep ), NULL, this );
	backStepButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnBackStep ), NULL, this );
	nextLineButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnNextLine ), NULL, this );
	prevLineButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnPrevLine ), NULL, this );
	finishButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnFinish ), NULL, this );
	rewindButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnRewind ), NULL, this );
	r0text->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( ComplxFrameDecl::OnTextKillFocus ), NULL, this );
	r0text->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( ComplxFrameDecl::OnBaseChange ), NULL, this );
	r0text->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ComplxFrameDecl::OnBaseChangeContext ), NULL, this );
	r0text->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ComplxFrameDecl::OnRegisterChanged ), NULL, this );
	r1text->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( ComplxFrameDecl::OnTextKillFocus ), NULL, this );
	r1text->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( ComplxFrameDecl::OnBaseChange ), NULL, this );
	r1text->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ComplxFrameDecl::OnBaseChangeContext ), NULL, this );
	r1text->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ComplxFrameDecl::OnRegisterChanged ), NULL, this );
	r2text->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( ComplxFrameDecl::OnTextKillFocus ), NULL, this );
	r2text->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( ComplxFrameDecl::OnBaseChange ), NULL, this );
	r2text->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ComplxFrameDecl::OnBaseChangeContext ), NULL, this );
	r2text->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ComplxFrameDecl::OnRegisterChanged ), NULL, this );
	r3text->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( ComplxFrameDecl::OnTextKillFocus ), NULL, this );
	r3text->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( ComplxFrameDecl::OnBaseChange ), NULL, this );
	r3text->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ComplxFrameDecl::OnBaseChangeContext ), NULL, this );
	r3text->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ComplxFrameDecl::OnRegisterChanged ), NULL, this );
	r4text->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( ComplxFrameDecl::OnTextKillFocus ), NULL, this );
	r4text->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( ComplxFrameDecl::OnBaseChange ), NULL, this );
	r4text->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ComplxFrameDecl::OnBaseChangeContext ), NULL, this );
	r4text->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ComplxFrameDecl::OnRegisterChanged ), NULL, this );
	r5text->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( ComplxFrameDecl::OnTextKillFocus ), NULL, this );
	r5text->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( ComplxFrameDecl::OnBaseChange ), NULL, this );
	r5text->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ComplxFrameDecl::OnBaseChangeContext ), NULL, this );
	r5text->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ComplxFrameDecl::OnRegisterChanged ), NULL, this );
	r6text->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( ComplxFrameDecl::OnTextKillFocus ), NULL, this );
	r6text->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( ComplxFrameDecl::OnBaseChange ), NULL, this );
	r6text->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ComplxFrameDecl::OnBaseChangeContext ), NULL, this );
	r6text->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ComplxFrameDecl::OnRegisterChanged ), NULL, this );
	r7text->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( ComplxFrameDecl::OnTextKillFocus ), NULL, this );
	r7text->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( ComplxFrameDecl::OnBaseChange ), NULL, this );
	r7text->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ComplxFrameDecl::OnBaseChangeContext ), NULL, this );
	r7text->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ComplxFrameDecl::OnRegisterChanged ), NULL, this );
	cctext->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( ComplxFrameDecl::OnTextKillFocus ), NULL, this );
	cctext->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ComplxFrameDecl::OnRegisterChanged ), NULL, this );
	pctext->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( ComplxFrameDecl::OnTextKillFocus ), NULL, this );
	pctext->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( ComplxFrameDecl::OnBaseChange ), NULL, this );
	pctext->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ComplxFrameDecl::OnBaseChangeContext ), NULL, this );
	pctext->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ComplxFrameDecl::OnRegisterChanged ), NULL, this );
}

ComplxFrameDecl::~ComplxFrameDecl()
{
	// Disconnect Events
	this->Disconnect( wxEVT_ACTIVATE, wxActivateEventHandler( ComplxFrameDecl::OnActivate ) );
	this->Disconnect( wxEVT_IDLE, wxIdleEventHandler( ComplxFrameDecl::OnIdle ) );
	runButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnRun ), NULL, this );
	stepButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnStep ), NULL, this );
	backStepButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnBackStep ), NULL, this );
	nextLineButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnNextLine ), NULL, this );
	prevLineButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnPrevLine ), NULL, this );
	finishButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnFinish ), NULL, this );
	rewindButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnRewind ), NULL, this );
	r0text->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( ComplxFrameDecl::OnTextKillFocus ), NULL, this );
	r0text->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( ComplxFrameDecl::OnBaseChange ), NULL, this );
	r0text->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ComplxFrameDecl::OnBaseChangeContext ), NULL, this );
	r0text->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ComplxFrameDecl::OnRegisterChanged ), NULL, this );
	r1text->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( ComplxFrameDecl::OnTextKillFocus ), NULL, this );
	r1text->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( ComplxFrameDecl::OnBaseChange ), NULL, this );
	r1text->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ComplxFrameDecl::OnBaseChangeContext ), NULL, this );
	r1text->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ComplxFrameDecl::OnRegisterChanged ), NULL, this );
	r2text->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( ComplxFrameDecl::OnTextKillFocus ), NULL, this );
	r2text->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( ComplxFrameDecl::OnBaseChange ), NULL, this );
	r2text->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ComplxFrameDecl::OnBaseChangeContext ), NULL, this );
	r2text->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ComplxFrameDecl::OnRegisterChanged ), NULL, this );
	r3text->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( ComplxFrameDecl::OnTextKillFocus ), NULL, this );
	r3text->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( ComplxFrameDecl::OnBaseChange ), NULL, this );
	r3text->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ComplxFrameDecl::OnBaseChangeContext ), NULL, this );
	r3text->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ComplxFrameDecl::OnRegisterChanged ), NULL, this );
	r4text->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( ComplxFrameDecl::OnTextKillFocus ), NULL, this );
	r4text->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( ComplxFrameDecl::OnBaseChange ), NULL, this );
	r4text->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ComplxFrameDecl::OnBaseChangeContext ), NULL, this );
	r4text->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ComplxFrameDecl::OnRegisterChanged ), NULL, this );
	r5text->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( ComplxFrameDecl::OnTextKillFocus ), NULL, this );
	r5text->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( ComplxFrameDecl::OnBaseChange ), NULL, this );
	r5text->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ComplxFrameDecl::OnBaseChangeContext ), NULL, this );
	r5text->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ComplxFrameDecl::OnRegisterChanged ), NULL, this );
	r6text->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( ComplxFrameDecl::OnTextKillFocus ), NULL, this );
	r6text->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( ComplxFrameDecl::OnBaseChange ), NULL, this );
	r6text->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ComplxFrameDecl::OnBaseChangeContext ), NULL, this );
	r6text->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ComplxFrameDecl::OnRegisterChanged ), NULL, this );
	r7text->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( ComplxFrameDecl::OnTextKillFocus ), NULL, this );
	r7text->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( ComplxFrameDecl::OnBaseChange ), NULL, this );
	r7text->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ComplxFrameDecl::OnBaseChangeContext ), NULL, this );
	r7text->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ComplxFrameDecl::OnRegisterChanged ), NULL, this );
	cctext->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( ComplxFrameDecl::OnTextKillFocus ), NULL, this );
	cctext->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ComplxFrameDecl::OnRegisterChanged ), NULL, this );
	pctext->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( ComplxFrameDecl::OnTextKillFocus ), NULL, this );
	pctext->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( ComplxFrameDecl::OnBaseChange ), NULL, this );
	pctext->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ComplxFrameDecl::OnBaseChangeContext ), NULL, this );
	pctext->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ComplxFrameDecl::OnRegisterChanged ), NULL, this );

}
