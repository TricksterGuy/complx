///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 25 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
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
	
	m_staticText11 = new wxStaticText( m_panel1, wxID_ANY, wxT("This is exactly the same environment used by lc3test."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bSizer2->Add( m_staticText11, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( m_panel1, wxID_ANY, wxT("Subroutine"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	wxArrayString subroutineChoices;
	subroutine = new wxChoice( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, subroutineChoices, 0 );
	subroutine->SetSelection( 0 );
	subroutine->SetToolTip( wxT("Subroutine to test.") );
	
	fgSizer1->Add( subroutine, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( m_panel1, wxID_ANY, wxT("Stack Symbol"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	stack = new wxTextCtrl( m_panel1, wxID_ANY, wxT("STACK"), wxDefaultPosition, wxDefaultSize, 0 );
	stack->SetToolTip( wxT("Symbol that contains stacks starting address") );
	
	fgSizer1->Add( stack, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	m_staticText4 = new wxStaticText( m_panel1, wxID_ANY, wxT("Params"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer1->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	params = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	params->SetToolTip( wxT("Comma separated list of expressions/addresses.") );
	
	fgSizer1->Add( params, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( m_panel1, wxID_ANY, wxT("Randomize Registers"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer1->Add( m_staticText5, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	randomRegisters = new wxCheckBox( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	randomRegisters->SetToolTip( wxT("Randomize all register values except R6 and R") );
	
	fgSizer1->Add( randomRegisters, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	m_staticText6 = new wxStaticText( m_panel1, wxID_ANY, wxT("Randomize Memory"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer1->Add( m_staticText6, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	randomMemory = new wxCheckBox( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	randomMemory->SetToolTip( wxT("Randomize memory before loading program.") );
	
	fgSizer1->Add( randomMemory, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	
	bSizer2->Add( fgSizer1, 0, wxEXPAND, 5 );
	
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
}

CallSubroutineDialogDecl::~CallSubroutineDialogDecl()
{
}
