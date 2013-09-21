///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct  8 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "RunTestDialogDecl.h"

///////////////////////////////////////////////////////////////////////////

RunTestDialogDecl::RunTestDialogDecl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	infoSplitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	infoSplitter->Connect( wxEVT_IDLE, wxIdleEventHandler( RunTestDialogDecl::infoSplitterOnIdle ), NULL, this );
	
	m_panel1 = new wxPanel( infoSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	testTree = new wxTreeCtrl( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE );
	bSizer5->Add( testTree, 1, wxEXPAND|wxALL, 4 );
	
	
	m_panel1->SetSizer( bSizer5 );
	m_panel1->Layout();
	bSizer5->Fit( m_panel1 );
	right = new wxPanel( infoSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	testInfoSizer = new wxBoxSizer( wxVERTICAL );
	
	
	right->SetSizer( testInfoSizer );
	right->Layout();
	testInfoSizer->Fit( right );
	infoSplitter->SplitVertically( m_panel1, right, 191 );
	bSizer1->Add( infoSplitter, 1, wxEXPAND, 5 );
	
	
	bSizer2->Add( bSizer1, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer8->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_button5 = new wxButton( this, wxID_ANY, wxT("Test Report"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button5->SetToolTip( wxT("Gives a report for the currently selected test case") );
	
	bSizer8->Add( m_button5, 0, wxALL, 5 );
	
	m_button51 = new wxButton( this, wxID_ANY, wxT("Full Report"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button51->SetToolTip( wxT("Gives a report for all test cases") );
	
	bSizer8->Add( m_button51, 0, wxALL, 5 );
	
	m_button4 = new wxButton( this, wxID_ANY, wxT("Run Test"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button4->SetToolTip( wxT("Runs the currently selected test case") );
	
	bSizer8->Add( m_button4, 0, wxALL, 5 );
	
	m_button2 = new wxButton( this, wxID_ANY, wxT("Run Tests"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button2->SetToolTip( wxT("Run all test cases") );
	
	bSizer8->Add( m_button2, 0, wxALL, 5 );
	
	
	bSizer2->Add( bSizer8, 0, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer2 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	testTree->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( RunTestDialogDecl::OnSelectTestItem ), NULL, this );
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunTestDialogDecl::OnTestReport ), NULL, this );
	m_button51->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunTestDialogDecl::OnReport ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunTestDialogDecl::OnRunTest ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunTestDialogDecl::OnRunTests ), NULL, this );
}

RunTestDialogDecl::~RunTestDialogDecl()
{
	// Disconnect Events
	testTree->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( RunTestDialogDecl::OnSelectTestItem ), NULL, this );
	m_button5->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunTestDialogDecl::OnTestReport ), NULL, this );
	m_button51->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunTestDialogDecl::OnReport ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunTestDialogDecl::OnRunTest ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RunTestDialogDecl::OnRunTests ), NULL, this );
	
}

TestSuiteInfoPanelDecl::TestSuiteInfoPanelDecl( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer261;
	bSizer261 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticText* m_staticText111;
	m_staticText111 = new wxStaticText( this, wxID_ANY, wxT("Passed?:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText111->Wrap( -1 );
	bSizer261->Add( m_staticText111, 0, wxALL, 4 );
	
	passed = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	passed->Wrap( -1 );
	bSizer261->Add( passed, 0, wxALL, 4 );
	
	
	bSizer13->Add( bSizer261, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticText* m_staticText11;
	m_staticText11 = new wxStaticText( this, wxID_ANY, wxT("Tests Passed:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bSizer26->Add( m_staticText11, 0, wxALL, 4 );
	
	testsPassed = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	testsPassed->Wrap( -1 );
	bSizer26->Add( testsPassed, 0, wxALL, 4 );
	
	
	bSizer13->Add( bSizer26, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Checks Passed:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer11->Add( m_staticText1, 0, wxALL, 4 );
	
	checksPassed = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	checksPassed->Wrap( -1 );
	bSizer11->Add( checksPassed, 0, wxALL, 4 );
	
	
	bSizer13->Add( bSizer11, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxHORIZONTAL );
	
	pointsText = new wxStaticText( this, wxID_ANY, wxT("Points:"), wxDefaultPosition, wxDefaultSize, 0 );
	pointsText->Wrap( -1 );
	bSizer111->Add( pointsText, 0, wxALL, 4 );
	
	points = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	points->Wrap( -1 );
	bSizer111->Add( points, 0, wxALL, 4 );
	
	
	bSizer13->Add( bSizer111, 0, wxEXPAND, 5 );
	
	wxStaticText* m_staticText32;
	m_staticText32 = new wxStaticText( this, wxID_ANY, wxT("Tests"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText32->Wrap( -1 );
	bSizer13->Add( m_staticText32, 0, wxTOP|wxRIGHT|wxLEFT, 4 );
	
	tests = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_NEEDED_SB ); 
	bSizer13->Add( tests, 1, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 4 );
	
	
	this->SetSizer( bSizer13 );
	this->Layout();
}

TestSuiteInfoPanelDecl::~TestSuiteInfoPanelDecl()
{
}

TestCaseInfoPanelDecl::TestCaseInfoPanelDecl( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );
	
	testName = new wxStaticText( this, wxID_ANY, wxT("TestName"), wxDefaultPosition, wxDefaultSize, 0 );
	testName->Wrap( -1 );
	bSizer15->Add( testName, 0, wxALL, 4 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Checks Passed:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer11->Add( m_staticText1, 0, wxALL, 4 );
	
	checksPassed = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	checksPassed->Wrap( -1 );
	bSizer11->Add( checksPassed, 0, wxALL, 4 );
	
	
	bSizer15->Add( bSizer11, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxHORIZONTAL );
	
	pointsText = new wxStaticText( this, wxID_ANY, wxT("Points:"), wxDefaultPosition, wxDefaultSize, 0 );
	pointsText->Wrap( -1 );
	bSizer111->Add( pointsText, 0, wxALL, 4 );
	
	points = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	points->Wrap( -1 );
	bSizer111->Add( points, 0, wxALL, 4 );
	
	
	bSizer15->Add( bSizer111, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer191;
	bSizer191 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticText* m_staticText18;
	m_staticText18 = new wxStaticText( this, wxID_ANY, wxT("Executed:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	bSizer191->Add( m_staticText18, 0, wxALL, 4 );
	
	executions = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	executions->Wrap( -1 );
	bSizer191->Add( executions, 0, wxALL, 4 );
	
	
	bSizer15->Add( bSizer191, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* m_staticText22;
	m_staticText22 = new wxStaticText( this, wxID_ANY, wxT("Test Information:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText22->Wrap( -1 );
	bSizer23->Add( m_staticText22, 0, wxTOP|wxRIGHT|wxLEFT, 4 );
	
	testInfo = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	testInfo->SetFont( wxFont( 9, 76, 90, 90, false, wxT("Monospace") ) );
	
	bSizer23->Add( testInfo, 1, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 4 );
	
	
	bSizer15->Add( bSizer23, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* m_staticText23;
	m_staticText23 = new wxStaticText( this, wxID_ANY, wxT("Preconditions:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	bSizer24->Add( m_staticText23, 0, wxTOP|wxRIGHT|wxLEFT, 4 );
	
	testInputs = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	testInputs->SetFont( wxFont( 9, 76, 90, 90, false, wxT("Monospace") ) );
	
	bSizer24->Add( testInputs, 1, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	
	bSizer19->Add( bSizer24, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer25;
	bSizer25 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* m_staticText231;
	m_staticText231 = new wxStaticText( this, wxID_ANY, wxT("Postconditions:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText231->Wrap( -1 );
	bSizer25->Add( m_staticText231, 0, wxTOP|wxRIGHT|wxLEFT, 4 );
	
	testOutputs = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	testOutputs->SetFont( wxFont( 9, 76, 90, 90, false, wxT("Monospace") ) );
	
	bSizer25->Add( testOutputs, 1, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	
	bSizer19->Add( bSizer25, 1, wxEXPAND, 5 );
	
	
	bSizer15->Add( bSizer19, 3, wxEXPAND, 5 );
	
	halted = new wxStaticText( this, wxID_ANY, wxT("WARNING TEST DID NOT REACH A HALT INSTRUCTION"), wxDefaultPosition, wxDefaultSize, 0 );
	halted->Wrap( -1 );
	halted->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Sans") ) );
	halted->SetForegroundColour( wxColour( 255, 0, 0 ) );
	halted->Hide();
	
	bSizer15->Add( halted, 0, wxALL, 4 );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxHORIZONTAL );
	
	warning = new wxStaticText( this, wxID_ANY, wxT("TEST COMPLETED WITH WARNINGS"), wxDefaultPosition, wxDefaultSize, 0 );
	warning->Wrap( -1 );
	warning->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Sans") ) );
	warning->SetForegroundColour( wxColour( 255, 0, 0 ) );
	warning->Hide();
	
	bSizer21->Add( warning, 0, wxALL|wxALIGN_CENTER_VERTICAL, 4 );
	
	warningButton = new wxButton( this, wxID_ANY, wxT("Warnings"), wxDefaultPosition, wxDefaultSize, 0 );
	warningButton->Hide();
	
	bSizer21->Add( warningButton, 0, wxALL, 4 );
	
	
	bSizer15->Add( bSizer21, 0, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer15 );
	this->Layout();
	
	// Connect Events
	warningButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TestCaseInfoPanelDecl::OnWarnings ), NULL, this );
}

TestCaseInfoPanelDecl::~TestCaseInfoPanelDecl()
{
	// Disconnect Events
	warningButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TestCaseInfoPanelDecl::OnWarnings ), NULL, this );
	
}

CheckInfoPanelDecl::CheckInfoPanelDecl( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer27;
	bSizer27 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticText* m_staticText27;
	m_staticText27 = new wxStaticText( this, wxID_ANY, wxT("Passed?:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText27->Wrap( -1 );
	bSizer27->Add( m_staticText27, 0, wxALL, 4 );
	
	passed = new wxStaticText( this, wxID_ANY, wxT("Passed"), wxDefaultPosition, wxDefaultSize, 0 );
	passed->Wrap( -1 );
	bSizer27->Add( passed, 0, wxALL, 4 );
	
	
	bSizer14->Add( bSizer27, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxHORIZONTAL );
	
	pointsText = new wxStaticText( this, wxID_ANY, wxT("Points:"), wxDefaultPosition, wxDefaultSize, 0 );
	pointsText->Wrap( -1 );
	bSizer111->Add( pointsText, 0, wxALL, 4 );
	
	points = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	points->Wrap( -1 );
	bSizer111->Add( points, 0, wxALL, 4 );
	
	
	bSizer14->Add( bSizer111, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticText* m_staticText29;
	m_staticText29 = new wxStaticText( this, wxID_ANY, wxT("Condition:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText29->Wrap( -1 );
	bSizer28->Add( m_staticText29, 0, wxALL, 4 );
	
	condition = new wxStaticText( this, wxID_ANY, wxT("PC is equal to x3000"), wxDefaultPosition, wxDefaultSize, 0 );
	condition->Wrap( -1 );
	bSizer28->Add( condition, 0, wxALL, 4 );
	
	
	bSizer14->Add( bSizer28, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer241;
	bSizer241 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticText* m_staticText221;
	m_staticText221 = new wxStaticText( this, wxID_ANY, wxT("Expected (right):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText221->Wrap( -1 );
	bSizer241->Add( m_staticText221, 0, wxALL, 4 );
	
	expected = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	expected->Wrap( -1 );
	bSizer241->Add( expected, 0, wxALL, 4 );
	
	
	bSizer14->Add( bSizer241, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2411;
	bSizer2411 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticText* m_staticText2211;
	m_staticText2211 = new wxStaticText( this, wxID_ANY, wxT("Actual (left):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2211->Wrap( -1 );
	bSizer2411->Add( m_staticText2211, 0, wxALL, 4 );
	
	actual = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	actual->Wrap( -1 );
	bSizer2411->Add( actual, 0, wxALL, 4 );
	
	
	bSizer14->Add( bSizer2411, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer14 );
	this->Layout();
}

CheckInfoPanelDecl::~CheckInfoPanelDecl()
{
}

TestReportDialogDecl::TestReportDialogDecl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer25;
	bSizer25 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticText* m_staticText27;
	m_staticText27 = new wxStaticText( this, wxID_ANY, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText27->Wrap( -1 );
	bSizer26->Add( m_staticText27, 0, wxALL, 4 );
	
	name = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	name->Wrap( -1 );
	bSizer26->Add( name, 0, wxALL, 4 );
	
	
	bSizer25->Add( bSizer26, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer27;
	bSizer27 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* m_staticText28;
	m_staticText28 = new wxStaticText( this, wxID_ANY, wxT("Report"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText28->Wrap( -1 );
	bSizer27->Add( m_staticText28, 0, wxTOP|wxRIGHT|wxLEFT, 4 );
	
	report = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	report->SetFont( wxFont( 9, 76, 90, 90, false, wxT("Monospace") ) );
	
	bSizer27->Add( report, 1, wxALL|wxEXPAND, 4 );
	
	
	bSizer25->Add( bSizer27, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer25 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

TestReportDialogDecl::~TestReportDialogDecl()
{
}
