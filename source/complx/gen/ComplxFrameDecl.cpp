///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "../MemoryView.hpp"

#include "ComplxFrameDecl.h"

///////////////////////////////////////////////////////////////////////////

ComplxFrameDecl::ComplxFrameDecl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetExtraStyle( wxWS_EX_VALIDATE_RECURSIVELY );
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(wxAUI_MGR_DEFAULT);

	menuBar = new wxMenuBar( 0 );
	menuFile = new wxMenu();
	wxMenuItem* menuFileLoad;
	menuFileLoad = new wxMenuItem( menuFile, wxID_OPEN, wxString( wxT("&Load") ) + wxT('\t') + wxT("Ctrl+O"), wxT("Loads an assembly file"), wxITEM_NORMAL );
	menuFile->Append( menuFileLoad );

	wxMenuItem* menuFileReload;
	menuFileReload = new wxMenuItem( menuFile, wxID_REFRESH, wxString( wxT("&Reload") ) + wxT('\t') + wxT("Ctrl+R"), wxT("Reloads current asm file under same conditions"), wxITEM_NORMAL );
	menuFile->Append( menuFileReload );

	wxMenuItem* menuFileAdvancedLoad;
	menuFileAdvancedLoad = new wxMenuItem( menuFile, ID_ADVANCED_LOAD, wxString( wxT("&Advanced Load") ) , wxT("Opens dialog for loading program under special conditions."), wxITEM_NORMAL );
	menuFile->Append( menuFileAdvancedLoad );

	wxMenuItem* menuFileExit;
	menuFileExit = new wxMenuItem( menuFile, wxID_EXIT, wxString( wxT("&Exit") ) + wxT('\t') + wxT("Alt+F4"), wxT("Exits the Program"), wxITEM_NORMAL );
	menuFile->Append( menuFileExit );

	menuBar->Append( menuFile, wxT("File") );

	menuView = new wxMenu();
	menuBar->Append( menuView, wxT("View") );

	menuControl = new wxMenu();
	wxMenuItem* menuControlStep;
	menuControlStep = new wxMenuItem( menuControl, ID_STEP, wxString( wxT("&Step") ) + wxT('\t') + wxT("F2"), wxT("Executes one instruction"), wxITEM_NORMAL );
	menuControl->Append( menuControlStep );

	wxMenuItem* menuControlBack;
	menuControlBack = new wxMenuItem( menuControl, ID_BACK, wxString( wxT("&Back") ) + wxT('\t') + wxT("F3"), wxT("Undo one instruction"), wxITEM_NORMAL );
	menuControl->Append( menuControlBack );

	menuBar->Append( menuControl, wxT("Control") );

	menuState = new wxMenu();
	menuBar->Append( menuState, wxT("State") );

	menuDebug = new wxMenu();
	menuBar->Append( menuDebug, wxT("Debug") );

	menuTest = new wxMenu();
	menuBar->Append( menuTest, wxT("Test") );

	menuHelp = new wxMenu();
	menuBar->Append( menuHelp, wxT("Help") );

	this->SetMenuBar( menuBar );

	toolbarPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_mgr.AddPane( toolbarPanel, wxAuiPaneInfo() .Top() .CaptionVisible( false ).CloseButton( false ).PaneBorder( false ).Dock().Resizable().FloatingSize( wxDefaultSize ).BestSize( wxSize( -1,48 ) ).Layer( 10 ).ToolbarPane() );

	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxHORIZONTAL );

	m_auiToolBar1 = new wxAuiToolBar( toolbarPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_GRIPPER|wxAUI_TB_HORZ_LAYOUT );
	m_button2 = new wxButton( m_auiToolBar1, wxID_ANY, wxT("Run"), wxDefaultPosition, wxDefaultSize, 0 );
	m_auiToolBar1->AddControl( m_button2 );
	m_button3 = new wxButton( m_auiToolBar1, wxID_ANY, wxT("Step"), wxDefaultPosition, wxDefaultSize, 0 );
	m_auiToolBar1->AddControl( m_button3 );
	m_button4 = new wxButton( m_auiToolBar1, wxID_ANY, wxT("Back"), wxDefaultPosition, wxDefaultSize, 0 );
	m_auiToolBar1->AddControl( m_button4 );
	m_button5 = new wxButton( m_auiToolBar1, wxID_ANY, wxT("Step Over"), wxDefaultPosition, wxDefaultSize, 0 );
	m_auiToolBar1->AddControl( m_button5 );
	m_button6 = new wxButton( m_auiToolBar1, wxID_ANY, wxT("Back Over"), wxDefaultPosition, wxDefaultSize, 0 );
	m_auiToolBar1->AddControl( m_button6 );
	m_button7 = new wxButton( m_auiToolBar1, wxID_ANY, wxT("Step Out"), wxDefaultPosition, wxDefaultSize, 0 );
	m_auiToolBar1->AddControl( m_button7 );
	m_button8 = new wxButton( m_auiToolBar1, wxID_ANY, wxT("Rewind"), wxDefaultPosition, wxDefaultSize, 0 );
	m_auiToolBar1->AddControl( m_button8 );
	m_auiToolBar1->Realize();

	bSizer111->Add( m_auiToolBar1, 1, wxALL|wxEXPAND, 5 );

	m_auiToolBar2 = new wxAuiToolBar( toolbarPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT );
	m_button10 = new wxButton( m_auiToolBar2, wxID_ANY, wxT("Breakpoint"), wxDefaultPosition, wxDefaultSize, 0 );
	m_auiToolBar2->AddControl( m_button10 );
	m_button11 = new wxButton( m_auiToolBar2, wxID_ANY, wxT("Watchpoint"), wxDefaultPosition, wxDefaultSize, 0 );
	m_auiToolBar2->AddControl( m_button11 );
	m_auiToolBar2->Realize();

	bSizer111->Add( m_auiToolBar2, 1, wxALL|wxALIGN_BOTTOM|wxEXPAND, 5 );


	toolbarPanel->SetSizer( bSizer111 );
	toolbarPanel->Layout();
	bSizer111->Fit( toolbarPanel );
	memoryPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_mgr.AddPane( memoryPanel, wxAuiPaneInfo() .Name( wxT("memory") ).Center() .Caption( wxT("Memory") ).CloseButton( false ).MaximizeButton( true ).MinimizeButton( true ).PinButton( true ).Gripper().Dock().Resizable().FloatingSize( wxDefaultSize ).CentrePane() );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	memoryView = new MemoryView( memoryPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_HORIZ_RULES|wxDV_ROW_LINES|wxDV_SINGLE|wxDV_VERT_RULES );
	memoryView->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	bSizer6->Add( memoryView, 1, wxEXPAND, 5 );


	memoryPanel->SetSizer( bSizer6 );
	memoryPanel->Layout();
	bSizer6->Fit( memoryPanel );
	statePanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_mgr.AddPane( statePanel, wxAuiPaneInfo() .Name( wxT("state") ).Right() .Caption( wxT("State") ).MaximizeButton( true ).MinimizeButton( true ).PinButton( true ).Gripper().Dock().Resizable().FloatingSize( wxDefaultSize ).BestSize( wxSize( 256,-1 ) ) );

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );

	statePropGridManager = new wxPropertyGridManager(statePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPGMAN_DEFAULT_STYLE);
	statePropGridManager->SetExtraStyle( wxPG_EX_MODE_BUTTONS );

	statePropGrid = statePropGridManager->AddPage( wxEmptyString, wxNullBitmap );
	registers = statePropGrid->Append( new wxPropertyCategory( wxT("Registers"), wxT("Registers") ) );
	bSizer8->Add( statePropGridManager, 1, wxALL|wxEXPAND, 5 );


	statePanel->SetSizer( bSizer8 );
	statePanel->Layout();
	bSizer8->Fit( statePanel );
	infoNotebook = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE );
	m_mgr.AddPane( infoNotebook, wxAuiPaneInfo() .Bottom() .PinButton( true ).Dock().Resizable().FloatingSize( wxDefaultSize ).BestSize( wxSize( -1,256 ) ) );

	consolePanel = new wxPanel( infoNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );

	consoleText = new wxTextCtrl( consolePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP );
	consoleText->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	bSizer11->Add( consoleText, 1, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );

	consoleInputText = new wxTextCtrl( consolePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	consoleInputText->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	consoleInputText->SetValidator( wxTextValidator( wxFILTER_ASCII, &consoleInput ) );

	bSizer11->Add( consoleInputText, 0, wxALL|wxEXPAND, 5 );


	consolePanel->SetSizer( bSizer11 );
	consolePanel->Layout();
	bSizer11->Fit( consolePanel );
	infoNotebook->AddPage( consolePanel, wxT("Console"), true, wxNullBitmap );
	warningPanel = new wxPanel( infoNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );

	warningText = new wxTextCtrl( warningPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP );
	bSizer13->Add( warningText, 1, wxALL|wxEXPAND, 5 );


	warningPanel->SetSizer( bSizer13 );
	warningPanel->Layout();
	bSizer13->Fit( warningPanel );
	infoNotebook->AddPage( warningPanel, wxT("Warnings"), false, wxNullBitmap );
	tracePanel = new wxPanel( infoNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );

	traceText = new wxTextCtrl( tracePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP );
	traceText->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	bSizer14->Add( traceText, 1, wxEXPAND, 5 );


	tracePanel->SetSizer( bSizer14 );
	tracePanel->Layout();
	bSizer14->Fit( tracePanel );
	infoNotebook->AddPage( tracePanel, wxT("Trace"), false, wxNullBitmap );
	loggingPanel = new wxPanel( infoNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );

	loggingText = new wxTextCtrl( loggingPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP );
	loggingText->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	bSizer15->Add( loggingText, 1, wxEXPAND, 5 );


	loggingPanel->SetSizer( bSizer15 );
	loggingPanel->Layout();
	bSizer15->Fit( loggingPanel );
	infoNotebook->AddPage( loggingPanel, wxT("Logging"), false, wxNullBitmap );

	m_statusBar2 = this->CreateStatusBar( 1, wxSTB_SIZEGRIP, wxID_ANY );

	m_mgr.Update();
	this->Centre( wxBOTH );

	// Connect Events
	menuFile->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnLoad ), this, menuFileLoad->GetId());
	menuFile->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnReload ), this, menuFileReload->GetId());
	menuFile->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnExit ), this, menuFileExit->GetId());
	menuControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnStep ), this, menuControlStep->GetId());
	menuControl->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ComplxFrameDecl::OnBack ), this, menuControlBack->GetId());
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnStep ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnBack ), NULL, this );
	statePropGridManager->Connect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( ComplxFrameDecl::OnStateChange ), NULL, this );
}

ComplxFrameDecl::~ComplxFrameDecl()
{
	// Disconnect Events
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnStep ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ComplxFrameDecl::OnBack ), NULL, this );
	statePropGridManager->Disconnect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( ComplxFrameDecl::OnStateChange ), NULL, this );

	m_mgr.UnInit();

}
