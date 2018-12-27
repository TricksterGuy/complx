///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "../gen/ComplxFrameDecl.h"

///////////////////////////////////////////////////////////////////////////

ComplxFrameDecl::ComplxFrameDecl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(wxAUI_MGR_DEFAULT);

	m_menubar3 = new wxMenuBar( 0 );
	menuFile = new wxMenu();
	m_menubar3->Append( menuFile, wxT("File") );

	menuView = new wxMenu();
	m_menubar3->Append( menuView, wxT("View") );

	menuState = new wxMenu();
	m_menubar3->Append( menuState, wxT("State") );

	menuDebug = new wxMenu();
	m_menubar3->Append( menuDebug, wxT("Debug") );

	menuTest = new wxMenu();
	m_menubar3->Append( menuTest, wxT("Test") );

	menuHelp = new wxMenu();
	m_menubar3->Append( menuHelp, wxT("Help") );

	this->SetMenuBar( m_menubar3 );

	toolbarPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_mgr.AddPane( toolbarPanel, wxAuiPaneInfo() .Top() .CaptionVisible( false ).CloseButton( false ).PaneBorder( false ).Dock().Resizable().FloatingSize( wxDefaultSize ).BestSize( wxSize( -1,48 ) ).Layer( 10 ).ToolbarPane() );

	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxVERTICAL );

	m_auiToolBar1 = new wxAuiToolBar( toolbarPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT );
	m_tool1 = m_auiToolBar1->AddTool( wxID_ANY, wxT("tool"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_auiToolBar1->AddSeparator();

	m_tool2 = m_auiToolBar1->AddTool( wxID_ANY, wxT("tool"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_tool3 = m_auiToolBar1->AddTool( wxID_ANY, wxT("tool"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_tool4 = m_auiToolBar1->AddTool( wxID_ANY, wxT("tool"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_tool5 = m_auiToolBar1->AddTool( wxID_ANY, wxT("tool"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_auiToolBar1->AddSeparator();

	m_tool6 = m_auiToolBar1->AddTool( wxID_ANY, wxT("tool"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_tool7 = m_auiToolBar1->AddTool( wxID_ANY, wxT("tool"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_tool8 = m_auiToolBar1->AddTool( wxID_ANY, wxT("tool"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_auiToolBar1->Realize();

	bSizer111->Add( m_auiToolBar1, 1, wxALL|wxEXPAND, 5 );


	toolbarPanel->SetSizer( bSizer111 );
	toolbarPanel->Layout();
	bSizer111->Fit( toolbarPanel );
	memoryPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_mgr.AddPane( memoryPanel, wxAuiPaneInfo() .Name( wxT("memory") ).Center() .Caption( wxT("Memory") ).CloseButton( false ).MaximizeButton( true ).MinimizeButton( true ).PinButton( true ).Gripper().Dock().Resizable().FloatingSize( wxDefaultSize ).CentrePane() );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	memoryView = new wxDataViewCtrl( memoryPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_HORIZ_RULES|wxDV_SINGLE|wxDV_VERT_RULES );
	bSizer6->Add( memoryView, 1, wxEXPAND, 5 );


	memoryPanel->SetSizer( bSizer6 );
	memoryPanel->Layout();
	bSizer6->Fit( memoryPanel );
	statePanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_mgr.AddPane( statePanel, wxAuiPaneInfo() .Name( wxT("state") ).Right() .Caption( wxT("State") ).MaximizeButton( true ).MinimizeButton( true ).PinButton( true ).Gripper().Dock().Resizable().FloatingSize( wxDefaultSize ).BestSize( wxSize( 200,-1 ) ) );

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
	m_mgr.AddPane( infoNotebook, wxAuiPaneInfo() .Bottom() .PinButton( true ).Dock().Resizable().FloatingSize( wxDefaultSize ).BestSize( wxSize( -1,200 ) ) );

	console = new wxPanel( infoNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );

	m_textCtrl1 = new wxTextCtrl( console, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP );
	m_textCtrl1->Enable( false );

	bSizer11->Add( m_textCtrl1, 1, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );

	m_textCtrl2 = new wxTextCtrl( console, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_textCtrl2, 0, wxALL|wxEXPAND, 5 );


	console->SetSizer( bSizer11 );
	console->Layout();
	bSizer11->Fit( console );
	infoNotebook->AddPage( console, wxT("Console"), true, wxNullBitmap );
	warnings = new wxPanel( infoNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );

	warningsText = new wxTextCtrl( warnings, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	warningsText->Enable( false );

	bSizer13->Add( warningsText, 1, wxALL|wxEXPAND, 5 );


	warnings->SetSizer( bSizer13 );
	warnings->Layout();
	bSizer13->Fit( warnings );
	infoNotebook->AddPage( warnings, wxT("Warnings"), false, wxNullBitmap );
	trace = new wxPanel( infoNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );

	traceText = new wxTextCtrl( trace, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	traceText->Enable( false );

	bSizer14->Add( traceText, 1, wxEXPAND, 5 );


	trace->SetSizer( bSizer14 );
	trace->Layout();
	bSizer14->Fit( trace );
	infoNotebook->AddPage( trace, wxT("Trace"), false, wxNullBitmap );
	logging = new wxPanel( infoNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );

	loggingText = new wxTextCtrl( logging, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	loggingText->Enable( false );

	bSizer15->Add( loggingText, 1, wxEXPAND, 5 );


	logging->SetSizer( bSizer15 );
	logging->Layout();
	bSizer15->Fit( logging );
	infoNotebook->AddPage( logging, wxT("Logging"), false, wxNullBitmap );

	m_statusBar2 = this->CreateStatusBar( 1, wxSTB_SIZEGRIP, wxID_ANY );

	m_mgr.Update();
	this->Centre( wxBOTH );
}

ComplxFrameDecl::~ComplxFrameDecl()
{
	m_mgr.UnInit();

}
