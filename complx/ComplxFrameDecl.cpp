///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 25 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "MemoryGrid.hpp"

#include "ComplxFrameDecl.h"

// Using the construction of a static object to ensure that the help provider is set
class wxFBContextSensitiveHelpSetter
{
public:
wxFBContextSensitiveHelpSetter()
{
wxHelpProvider::Set( new wxHelpControllerHelpProvider );
}
};

static wxFBContextSensitiveHelpSetter s_wxFBSetTheHelpProvider;
///////////////////////////////////////////////////////////////////////////

ComplxFrameDecl::ComplxFrameDecl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	menu = new wxMenuBar( 0 );
	menuFile = new wxMenu();
	wxMenuItem* menuFileRandomizeAndLoad;
	menuFileRandomizeAndLoad = new wxMenuItem( menuFile, ID_RANDOMIZE_AND_LOAD, wxString( _("Randomize and Load") ) + wxT('\t') + wxT("Ctrl+Alt+O"), _("Randomizes memory and loads a asm file"), wxITEM_NORMAL );
	menuFile->Append( menuFileRandomizeAndLoad );
	
	wxMenuItem* menuFileRandomizeAndReload;
	menuFileRandomizeAndReload = new wxMenuItem( menuFile, ID_RANDOMIZE_AND_RELOAD, wxString( _("Randomize and Reload") ) + wxT('\t') + wxT("Ctrl+Alt+R"), _("Randomizes memory and reloads a asm file"), wxITEM_NORMAL );
	menuFile->Append( menuFileRandomizeAndReload );
	
	wxMenuItem* menuFileLoad;
	menuFileLoad = new wxMenuItem( menuFile, ID_LOAD, wxString( _("&Load") ) + wxT('\t') + wxT("Ctrl+O"), _("Initializes and loads an assembly file"), wxITEM_NORMAL );
	menuFile->Append( menuFileLoad );
	
	wxMenuItem* menuFileReload;
	menuFileReload = new wxMenuItem( menuFile, ID_RELOAD, wxString( _("&Reload") ) + wxT('\t') + wxT("Ctrl+R"), _("Initializes and reloads last file"), wxITEM_NORMAL );
	menuFile->Append( menuFileReload );
	
	wxMenuItem* menuFileLoadOver;
	menuFileLoadOver = new wxMenuItem( menuFile, ID_LOAD_OVER, wxString( _("Load &Over") ) + wxT('\t') + wxT("Ctrl+Shift+O"), _("Loads an assembly file"), wxITEM_NORMAL );
	menuFile->Append( menuFileLoadOver );
	
	wxMenuItem* menuFileReloadOver;
	menuFileReloadOver = new wxMenuItem( menuFile, ID_RELOAD_OVER, wxString( _("Reload O&ver") ) + wxT('\t') + wxT("Ctrl+Shift+R"), _("Reloads last assembly file"), wxITEM_NORMAL );
	menuFile->Append( menuFileReloadOver );
	
	menuFile->AppendSeparator();
	
	wxMenuItem* menuFileQuit;
	menuFileQuit = new wxMenuItem( menuFile, ID_QUIT, wxString( _("&Quit") ) + wxT('\t') + wxT("Ctrl+W"), _("Quits the program."), wxITEM_NORMAL );
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
	wxMenuItem* menuViewHideAddressesShowAll;
	menuViewHideAddressesShowAll = new wxMenuItem( menuViewHideAddresses, ID_SHOW_ALL, wxString( _("Show All") ) , _("Shows all memory addresses"), wxITEM_RADIO );
	menuViewHideAddresses->Append( menuViewHideAddressesShowAll );
	
	wxMenuItem* menuViewHideAddressesShowNonZer;
	menuViewHideAddressesShowNonZer = new wxMenuItem( menuViewHideAddresses, ID_SHOW_NON_ZERO, wxString( _("Show Non Zero") ) , _("Show all memory addresses that have a value other than 0"), wxITEM_RADIO );
	menuViewHideAddresses->Append( menuViewHideAddressesShowNonZer );
	
	wxMenuItem* menuViewHideAddressesShowOnlyCodeData;
	menuViewHideAddressesShowOnlyCodeData = new wxMenuItem( menuViewHideAddresses, ID_SHOW_ONLY_CODEDATA, wxString( _("Show Only Code/Data") ) , _("Shows addresses modified when your program was loaded."), wxITEM_RADIO );
	menuViewHideAddresses->Append( menuViewHideAddressesShowOnlyCodeData );
	
	wxMenuItem* menuViewHideAddressesCustom;
	menuViewHideAddressesCustom = new wxMenuItem( menuViewHideAddresses, ID_CUSTOM, wxString( _("Custom...") ) + wxT('\t') + wxT("Ctrl+H"), _("Allows you to show/hide memory address ranges"), wxITEM_NORMAL );
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
	
	wxMenuItem* menuStateRandomize;
	menuStateRandomize = new wxMenuItem( menuState, ID_RANDOMIZE, wxString( _("Randomize") ) + wxT('\t') + wxT("Ctrl+J"), _("Randomizes Memory"), wxITEM_NORMAL );
	menuState->Append( menuStateRandomize );
	
	wxMenuItem* menuStateReinitialize;
	menuStateReinitialize = new wxMenuItem( menuState, ID_REINITIALIZE, wxString( _("Reinitialize") ) + wxT('\t') + wxT("Ctrl+K"), _("Reinitializes Memory"), wxITEM_NORMAL );
	menuState->Append( menuStateReinitialize );
	
	menuStateTrueTraps = new wxMenuItem( menuState, ID_TRUE_TRAPS, wxString( _("True Traps") ) + wxT('\t') + wxT("Ctrl+T"), _("Enables True Traps"), wxITEM_CHECK );
	menuState->Append( menuStateTrueTraps );
	
	menuStateInterrupts = new wxMenuItem( menuState, ID_INTERRUPTS, wxString( _("Interrupts") ) + wxT('\t') + wxT("Ctrl+I"), _("Enables Interrupts"), wxITEM_CHECK );
	menuState->Append( menuStateInterrupts );
	
	menuState->AppendSeparator();
	
	wxMenuItem* menuStateClearConsole;
	menuStateClearConsole = new wxMenuItem( menuState, ID_CLEAR_CONSOLE, wxString( _("&Clear Console") ) , _("Clears the console output."), wxITEM_NORMAL );
	menuState->Append( menuStateClearConsole );
	
	wxMenuItem* menuStateClearConsoleInput;
	menuStateClearConsoleInput = new wxMenuItem( menuState, wxID_ANY, wxString( _("Clear Console Input") ) , _("Clears the console input."), wxITEM_NORMAL );
	menuState->Append( menuStateClearConsoleInput );
	
	menuState->AppendSeparator();
	
	menu->Append( menuState, _("State") ); 
	
	menuDebug = new wxMenu();
	wxMenuItem* menuDebugUndoStack;
	menuDebugUndoStack = new wxMenuItem( menuDebug, ID_UNDO_STACK, wxString( _("&Undo Stack") ) + wxT('\t') + wxT("F6"), _("Modifies the undo stack size"), wxITEM_NORMAL );
	menuDebug->Append( menuDebugUndoStack );
	
	wxMenuItem* menuDebugCallStack;
	menuDebugCallStack = new wxMenuItem( menuDebug, ID_CALL_STACK, wxString( _("&Call Stack") ) , _("Views the call stack."), wxITEM_NORMAL );
	menuDebug->Append( menuDebugCallStack );
	
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
	
	menu->Append( menuDebug, _("Debug") ); 
	
	menuTest = new wxMenu();
	wxMenuItem* menuTestRunTests;
	menuTestRunTests = new wxMenuItem( menuTest, ID_RUN_TESTS, wxString( _("&Run Tests") ) + wxT('\t') + wxT("Ctrl+Shift+T"), _("Run tests from an xml file."), wxITEM_NORMAL );
	menuTest->Append( menuTestRunTests );
	
	wxMenuItem* menuTestRerunTests;
	menuTestRerunTests = new wxMenuItem( menuTest, ID_RERUN_TESTS, wxString( _("&Rerun Tests") ) + wxT('\t') + wxT("Ctrl+Alt+T"), _("Rerun tests from an xml file."), wxITEM_NORMAL );
	menuTest->Append( menuTestRerunTests );
	
	menu->Append( menuTest, _("Test") ); 
	
	menuHelp = new wxMenu();
	wxMenuItem* menuHelpDocs;
	menuHelpDocs = new wxMenuItem( menuHelp, wxID_ANY, wxString( _("&Documentation") ) + wxT('\t') + wxT("F1"), _("Opens up the Complx Documentation."), wxITEM_NORMAL );
	menuHelp->Append( menuHelpDocs );
	
	wxMenuItem* menuHelpISA;
	menuHelpISA = new wxMenuItem( menuHelp, wxID_ANY, wxString( _("&LC-3 ISA") ) + wxT('\t') + wxT("SHIFT+F1"), _("Opens up the lc3 isa reference manual."), wxITEM_NORMAL );
	menuHelp->Append( menuHelpISA );
	
	wxMenuItem* menuHelpChangeLog;
	menuHelpChangeLog = new wxMenuItem( menuHelp, ID_CHANGE_LOG, wxString( _("&Change Log") ) , _("Shows the change log"), wxITEM_NORMAL );
	menuHelp->Append( menuHelpChangeLog );
	
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
	memory->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	memory->EnableDragRowSize( false );
	memory->SetRowLabelSize( 0 );
	memory->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	memory->SetLabelFont( wxFont( 10, 70, 90, 90, false, wxT("Courier New") ) );
	
	// Cell Defaults
	memory->SetDefaultCellFont( wxFont( 10, 70, 90, 90, false, wxT("Courier New") ) );
	memory->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_CENTRE );
	memory->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	subsizer->Add( memory, 1, wxALL|wxEXPAND, 5 );
	
	wxGridSizer* controlSizer;
	controlSizer = new wxGridSizer( 1, 8, 0, 0 );
	
	runButton = new wxButton( mainPanel, wxID_ANY, _("&Run"), wxDefaultPosition, wxDefaultSize, 0 );
	runButton->SetToolTip( _("Runs your code until it HALTs") );
	
	controlSizer->Add( runButton, 0, wxALL|wxEXPAND, 2 );
	
	runForButton = new wxButton( mainPanel, wxID_ANY, _("R&un For"), wxDefaultPosition, wxDefaultSize, 0 );
	runForButton->SetToolTip( _("Runs your code for X instructions") );
	
	controlSizer->Add( runForButton, 0, wxALL|wxEXPAND, 2 );
	
	stepButton = new wxButton( mainPanel, wxID_ANY, _("&Step"), wxDefaultPosition, wxDefaultSize, 0 );
	stepButton->SetToolTip( _("Runs one instruction") );
	
	controlSizer->Add( stepButton, 0, wxALL|wxEXPAND, 2 );
	
	backStepButton = new wxButton( mainPanel, wxID_ANY, _("&Back"), wxDefaultPosition, wxDefaultSize, 0 );
	backStepButton->SetToolTip( _("Undoes one instruction") );
	
	controlSizer->Add( backStepButton, 0, wxALL|wxEXPAND, 2 );
	
	nextLineButton = new wxButton( mainPanel, wxID_ANY, _("&Next Line"), wxDefaultPosition, wxDefaultSize, 0 );
	nextLineButton->SetToolTip( _("Run the next instruction (blackboxes subroutines)") );
	
	controlSizer->Add( nextLineButton, 0, wxALL|wxEXPAND, 2 );
	
	prevLineButton = new wxButton( mainPanel, wxID_ANY, _("&Prev Line"), wxDefaultPosition, wxDefaultSize, 0 );
	prevLineButton->SetToolTip( _("Undoes the last instruction (blackboxing subroutines)") );
	
	controlSizer->Add( prevLineButton, 0, wxALL|wxEXPAND, 2 );
	
	finishButton = new wxButton( mainPanel, wxID_ANY, _("&Finish"), wxDefaultPosition, wxDefaultSize, 0 );
	finishButton->SetToolTip( _("Steps out of the current subroutine.") );
	
	controlSizer->Add( finishButton, 0, wxALL|wxEXPAND, 2 );
	
	rewindButton = new wxButton( mainPanel, wxID_ANY, _("Re&wind"), wxDefaultPosition, wxDefaultSize, 0 );
	rewindButton->SetToolTip( _("Rewinds back to start (or undoes all of the instructions in the stack)") );
	
	controlSizer->Add( rewindButton, 0, wxALL, 2 );
	
	
	subsizer->Add( controlSizer, 0, wxEXPAND, 0 );
	
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
	r0text->SetMaxLength( 24 ); 
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
	r1text->SetMaxLength( 24 ); 
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
	r2text->SetMaxLength( 24 ); 
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
	r3text->SetMaxLength( 24 ); 
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
	r4text->SetMaxLength( 24 ); 
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
	r5text->SetMaxLength( 24 ); 
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
	r6text->SetMaxLength( 24 ); 
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
	r7text->SetMaxLength( 24 ); 
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
	cctext->SetMaxLength( 24 ); 
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
	pctext->SetMaxLength( 24 ); 
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
	statusBar = this->CreateStatusBar( 3, wxST_SIZEGRIP, wxID_ANY );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_IDLE, wxIdleEventHandler( ComplxFrameDecl::OnIdle ) );
	this->Connect( menuFileRandomizeAndLoad->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRandomizeAndLoad ) );
	this->Connect( menuFileRandomizeAndReload->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRandomizeAndReload ) );
	this->Connect( menuFileLoad->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnLoad ) );
	this->Connect( menuFileReload->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnReload ) );
	this->Connect( menuFileLoadOver->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnLoadOver ) );
	this->Connect( menuFileReloadOver->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnReloadOver ) );
	this->Connect( menuFileQuit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnQuit ) );
	this->Connect( menuViewNew->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnNewView ) );
	this->Connect( menuViewGoto->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnGoto ) );
	this->Connect( menuViewHideAddressesShowAll->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnUpdateHideAddresses ) );
	this->Connect( menuViewHideAddressesShowNonZer->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnUpdateHideAddresses ) );
	this->Connect( menuViewHideAddressesShowOnlyCodeData->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnUpdateHideAddresses ) );
	this->Connect( menuViewHideAddressesCustom->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnHideAddressesCustom ) );
	this->Connect( menuViewBasic->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnDumbDisassemble ) );
	this->Connect( menuViewNormal->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnNormalDisassemble ) );
	this->Connect( menuViewHighLevel->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnCDisassemble ) );
	this->Connect( menuViewInstructionHighlighting->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnInstructionHighlight ) );
	this->Connect( menuStateControlStep->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnStep ) );
	this->Connect( menuStateControlBack->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnBackStep ) );
	this->Connect( menuStateControlNextLine->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnNextLine ) );
	this->Connect( menuStateControlPreviousLine->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnPrevLine ) );
	this->Connect( menuStateControlRun->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRun ) );
	this->Connect( menuStateControlRunFor->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRunFor ) );
	this->Connect( menuStateControlRunAgain->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRunAgain ) );
	this->Connect( menuStateControlRewind->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRewind ) );
	this->Connect( menuStateControlFinish->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnFinish ) );
	this->Connect( menuStateRandomize->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRandomize ) );
	this->Connect( menuStateReinitialize->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnReinitialize ) );
	this->Connect( menuStateTrueTraps->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnTrueTraps ) );
	this->Connect( menuStateInterrupts->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnInterrupts ) );
	this->Connect( menuStateClearConsole->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnClearConsole ) );
	this->Connect( menuStateClearConsoleInput->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnClearConsoleInput ) );
	this->Connect( menuDebugUndoStack->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnUndoStack ) );
	this->Connect( menuDebugCallStack->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnCallStack ) );
	this->Connect( menuDebugBreakWatchpoints->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnBreakAndWatchpoints ) );
	this->Connect( menuDebugTemporary->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnTemppoint ) );
	this->Connect( menuDebugBreakpoint->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnBreakpoint ) );
	this->Connect( menuDebugWatchpoint->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnWatchpoint ) );
	this->Connect( menuDebugAdvanced->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnAdvancedBreakpoint ) );
	this->Connect( menuDebugBlackbox->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnBlackbox ) );
	this->Connect( menuTestRunTests->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRunTests ) );
	this->Connect( menuTestRerunTests->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRerunTests ) );
	this->Connect( menuHelpDocs->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnDocs ) );
	this->Connect( menuHelpISA->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnISA ) );
	this->Connect( menuHelpChangeLog->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnChangeLog ) );
	this->Connect( menuHelpCreateBugReport->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnCreateBugReport ) );
	this->Connect( menuHelpFirstTime->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnFirstTime ) );
	this->Connect( menuHelpTips->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnTips ) );
	this->Connect( menuHelpAbout->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnAbout ) );
	runButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnRun ), NULL, this );
	runForButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnRunFor ), NULL, this );
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
	this->Disconnect( wxEVT_IDLE, wxIdleEventHandler( ComplxFrameDecl::OnIdle ) );
	this->Disconnect( ID_RANDOMIZE_AND_LOAD, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRandomizeAndLoad ) );
	this->Disconnect( ID_RANDOMIZE_AND_RELOAD, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRandomizeAndReload ) );
	this->Disconnect( ID_LOAD, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnLoad ) );
	this->Disconnect( ID_RELOAD, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnReload ) );
	this->Disconnect( ID_LOAD_OVER, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnLoadOver ) );
	this->Disconnect( ID_RELOAD_OVER, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnReloadOver ) );
	this->Disconnect( ID_QUIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnQuit ) );
	this->Disconnect( ID_NEW_VIEW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnNewView ) );
	this->Disconnect( ID_GOTO_ADDRESS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnGoto ) );
	this->Disconnect( ID_SHOW_ALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnUpdateHideAddresses ) );
	this->Disconnect( ID_SHOW_NON_ZERO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnUpdateHideAddresses ) );
	this->Disconnect( ID_SHOW_ONLY_CODEDATA, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnUpdateHideAddresses ) );
	this->Disconnect( ID_CUSTOM, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnHideAddressesCustom ) );
	this->Disconnect( ID_BASIC, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnDumbDisassemble ) );
	this->Disconnect( ID_NORMAL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnNormalDisassemble ) );
	this->Disconnect( ID_HIGH_LEVEL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnCDisassemble ) );
	this->Disconnect( ID_INSTRUCTION_HIGHLIGHTING, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnInstructionHighlight ) );
	this->Disconnect( ID_STEP, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnStep ) );
	this->Disconnect( ID_BACK, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnBackStep ) );
	this->Disconnect( ID_NEXT_LINE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnNextLine ) );
	this->Disconnect( ID_PREVIOUS_LINE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnPrevLine ) );
	this->Disconnect( ID_RUN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRun ) );
	this->Disconnect( ID_RUN_FOR, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRunFor ) );
	this->Disconnect( ID_RUN_AGAIN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRunAgain ) );
	this->Disconnect( ID_REWIND, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRewind ) );
	this->Disconnect( ID_FINISH, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnFinish ) );
	this->Disconnect( ID_RANDOMIZE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRandomize ) );
	this->Disconnect( ID_REINITIALIZE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnReinitialize ) );
	this->Disconnect( ID_TRUE_TRAPS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnTrueTraps ) );
	this->Disconnect( ID_INTERRUPTS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnInterrupts ) );
	this->Disconnect( ID_CLEAR_CONSOLE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnClearConsole ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnClearConsoleInput ) );
	this->Disconnect( ID_UNDO_STACK, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnUndoStack ) );
	this->Disconnect( ID_CALL_STACK, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnCallStack ) );
	this->Disconnect( ID_BREAKPOINTS_AND_WATCHPOINTS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnBreakAndWatchpoints ) );
	this->Disconnect( ID_ADD_TEMPORARY_BREAKPOINT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnTemppoint ) );
	this->Disconnect( ID_ADD_BREAKPOINT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnBreakpoint ) );
	this->Disconnect( ID_ADD_WATCHPOINT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnWatchpoint ) );
	this->Disconnect( ID_ADVANCED_BREAKPOINT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnAdvancedBreakpoint ) );
	this->Disconnect( ID_ADD_BLACKBOX, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnBlackbox ) );
	this->Disconnect( ID_RUN_TESTS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRunTests ) );
	this->Disconnect( ID_RERUN_TESTS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnRerunTests ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnDocs ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnISA ) );
	this->Disconnect( ID_CHANGE_LOG, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnChangeLog ) );
	this->Disconnect( ID_CREATE_BUG_REPORT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnCreateBugReport ) );
	this->Disconnect( ID_FIRST_TIME_MESSAGE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnFirstTime ) );
	this->Disconnect( ID_TIPS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnTips ) );
	this->Disconnect( ID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnAbout ) );
	runButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnRun ), NULL, this );
	runForButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnRunFor ), NULL, this );
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
