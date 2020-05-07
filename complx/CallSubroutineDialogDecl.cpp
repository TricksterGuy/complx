///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "CallSubroutineDialogDecl.h"

///////////////////////////////////////////////////////////////////////////

CallSubroutineDialogDecl::CallSubroutineDialogDecl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	m_staticText8 = new wxStaticText( m_panel1, wxID_ANY, wxT("For debugging purposes only."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	bSizer2->Add( m_staticText8, 0, wxALL|wxEXPAND, 5 );

	m_staticText9 = new wxStaticText( m_panel1, wxID_ANY, wxT("This dialog allows you to simulate a subroutine call."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	bSizer2->Add( m_staticText9, 0, wxALL, 5 );

	m_staticText10 = new wxStaticText( m_panel1, wxID_ANY, wxT("This is to ensure your subroutine works in all cases."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	bSizer2->Add( m_staticText10, 0, wxALL, 5 );

	m_staticText11 = new wxStaticText( m_panel1, wxID_ANY, wxT("This is exactly the same environment used by the pyLC3 autograder."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bSizer2->Add( m_staticText11, 0, wxALL, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText101 = new wxStaticText( m_panel1, wxID_ANY, wxT("I'm using"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText101->Wrap( -1 );
	bSizer5->Add( m_staticText101, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	wxString subroutineModeChoices[] = { wxT("LC3 Calling Convention"), wxT("Pass by Registers") };
	int subroutineModeNChoices = sizeof( subroutineModeChoices ) / sizeof( wxString );
	subroutineMode = new wxChoice( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, subroutineModeNChoices, subroutineModeChoices, 0 );
	subroutineMode->SetSelection( 0 );
	bSizer5->Add( subroutineMode, 0, wxALL, 5 );


	bSizer4->Add( bSizer5, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText1 = new wxStaticText( m_panel1, wxID_ANY, wxT("Subroutine"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer6->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	wxArrayString subroutineChoices;
	subroutine = new wxChoice( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, subroutineChoices, 0 );
	subroutine->SetSelection( 0 );
	subroutine->SetToolTip( wxT("Subroutine to test.") );

	bSizer6->Add( subroutine, 0, wxALL|wxEXPAND, 5 );


	bSizer4->Add( bSizer6, 0, wxEXPAND, 5 );

	callingConventionPanel = new wxPanel( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer71;
	bSizer71 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText2 = new wxStaticText( callingConventionPanel, wxID_ANY, wxT("Stack Start"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer8->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	stack = new wxTextCtrl( callingConventionPanel, wxID_ANY, wxT("xF000"), wxDefaultPosition, wxDefaultSize, 0 );
	stack->SetToolTip( wxT("Usually not recommended to change this.") );

	bSizer8->Add( stack, 0, wxALL|wxALIGN_RIGHT, 5 );

	m_staticText20 = new wxStaticText( callingConventionPanel, wxID_ANY, wxT("R5"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	bSizer8->Add( m_staticText20, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	r5ValueLCC = new wxTextCtrl( callingConventionPanel, wxID_ANY, wxT("xCAFE"), wxDefaultPosition, wxDefaultSize, 0 );
	r5ValueLCC->SetToolTip( wxT("Usually not recommended to change this.") );

	bSizer8->Add( r5ValueLCC, 0, wxALL, 5 );

	m_staticText19 = new wxStaticText( callingConventionPanel, wxID_ANY, wxT("R7"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	bSizer8->Add( m_staticText19, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	r7ValueLCC = new wxTextCtrl( callingConventionPanel, wxID_ANY, wxT("x8000"), wxDefaultPosition, wxDefaultSize, 0 );
	r7ValueLCC->SetToolTip( wxT("Usually not recommended to change this value. A breakpoint will be at this address.") );

	bSizer8->Add( r7ValueLCC, 0, wxALL, 5 );


	bSizer71->Add( bSizer8, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText4 = new wxStaticText( callingConventionPanel, wxID_ANY, wxT("Params"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer9->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	params = new wxTextCtrl( callingConventionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	params->SetToolTip( wxT("Comma separated list of expressions/addresses.") );

	bSizer9->Add( params, 1, wxALL|wxEXPAND, 5 );


	bSizer71->Add( bSizer9, 1, wxEXPAND, 5 );


	callingConventionPanel->SetSizer( bSizer71 );
	callingConventionPanel->Layout();
	bSizer71->Fit( callingConventionPanel );
	bSizer4->Add( callingConventionPanel, 1, wxEXPAND | wxALL, 5 );

	passByRegistersPanel = new wxPanel( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	passByRegistersPanel->Hide();

	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText111 = new wxStaticText( passByRegistersPanel, wxID_ANY, wxT("R0"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText111->Wrap( -1 );
	bSizer12->Add( m_staticText111, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	r0Value = new wxTextCtrl( passByRegistersPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( r0Value, 0, wxALL, 5 );

	m_staticText112 = new wxStaticText( passByRegistersPanel, wxID_ANY, wxT("R1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText112->Wrap( -1 );
	bSizer12->Add( m_staticText112, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	r1Value = new wxTextCtrl( passByRegistersPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( r1Value, 0, wxALL, 5 );

	m_staticText13 = new wxStaticText( passByRegistersPanel, wxID_ANY, wxT("R2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	bSizer12->Add( m_staticText13, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	r2Value = new wxTextCtrl( passByRegistersPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( r2Value, 0, wxALL, 5 );

	m_staticText14 = new wxStaticText( passByRegistersPanel, wxID_ANY, wxT("R3"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	bSizer12->Add( m_staticText14, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	r3Value = new wxTextCtrl( passByRegistersPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( r3Value, 0, wxALL, 5 );


	bSizer11->Add( bSizer12, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText15 = new wxStaticText( passByRegistersPanel, wxID_ANY, wxT("R4"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	bSizer13->Add( m_staticText15, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	r4Value = new wxTextCtrl( passByRegistersPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( r4Value, 0, wxALL, 5 );

	m_staticText16 = new wxStaticText( passByRegistersPanel, wxID_ANY, wxT("R5"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	bSizer13->Add( m_staticText16, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	r5Value = new wxTextCtrl( passByRegistersPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( r5Value, 0, wxALL, 5 );

	m_staticText17 = new wxStaticText( passByRegistersPanel, wxID_ANY, wxT("R6"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	bSizer13->Add( m_staticText17, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	r6Value = new wxTextCtrl( passByRegistersPanel, wxID_ANY, wxT("xF000"), wxDefaultPosition, wxDefaultSize, 0 );
	r6Value->SetToolTip( wxT("Usually not recommended to change this.") );

	bSizer13->Add( r6Value, 0, wxALL, 5 );

	m_staticText18 = new wxStaticText( passByRegistersPanel, wxID_ANY, wxT("R7"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	bSizer13->Add( m_staticText18, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	r7Value = new wxTextCtrl( passByRegistersPanel, wxID_ANY, wxT("x8000"), wxDefaultPosition, wxDefaultSize, 0 );
	r7Value->SetToolTip( wxT("Usually not recommended to change this value. A breakpoint will be at this address.") );

	bSizer13->Add( r7Value, 0, wxALL, 5 );


	bSizer11->Add( bSizer13, 1, wxEXPAND, 5 );


	passByRegistersPanel->SetSizer( bSizer11 );
	passByRegistersPanel->Layout();
	bSizer11->Fit( passByRegistersPanel );
	bSizer4->Add( passByRegistersPanel, 1, wxEXPAND | wxALL, 5 );


	bSizer2->Add( bSizer4, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );

	m_button1 = new wxButton( m_panel1, wxID_OK, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_button1, 0, wxALL, 5 );

	m_button2 = new wxButton( m_panel1, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_button2, 0, wxALL, 5 );


	bSizer2->Add( bSizer7, 0, wxALIGN_RIGHT, 5 );


	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	subroutineMode->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CallSubroutineDialogDecl::OnSubroutineModeChange ), NULL, this );
}

CallSubroutineDialogDecl::~CallSubroutineDialogDecl()
{
	// Disconnect Events
	subroutineMode->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CallSubroutineDialogDecl::OnSubroutineModeChange ), NULL, this );

}
