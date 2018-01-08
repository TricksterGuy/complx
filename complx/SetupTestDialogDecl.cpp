///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jan  1 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "SetupTestDialogDecl.h"

///////////////////////////////////////////////////////////////////////////

SetupTestDialogDecl::SetupTestDialogDecl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );
	
	m_panel2 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText11 = new wxStaticText( m_panel2, wxID_ANY, _("Test Case:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bSizer16->Add( m_staticText11, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString testsChoices;
	tests = new wxChoice( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, testsChoices, 0 );
	tests->SetSelection( 0 );
	bSizer16->Add( tests, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_button8 = new wxButton( m_panel2, wxID_ANY, _("Reload XML"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_button8, 0, wxALL, 5 );
	
	
	bSizer15->Add( bSizer16, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_panel2, wxID_ANY, _("Preconditions") ), wxVERTICAL );
	
	preconditions = new wxListBox( sbSizer1->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	sbSizer1->Add( preconditions, 1, wxEXPAND|wxALL, 5 );
	
	
	bSizer19->Add( sbSizer1, 1, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_panel2, wxID_ANY, _("Postconditions") ), wxVERTICAL );
	
	postconditions = new wxListBox( sbSizer2->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	sbSizer2->Add( postconditions, 1, wxEXPAND|wxALL, 5 );
	
	
	bSizer19->Add( sbSizer2, 1, wxEXPAND|wxALL, 5 );
	
	
	bSizer15->Add( bSizer19, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button9 = new wxButton( m_panel2, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer18->Add( m_button9, 0, wxALL, 5 );
	
	m_button10 = new wxButton( m_panel2, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer18->Add( m_button10, 0, wxALL, 5 );
	
	
	bSizer15->Add( bSizer18, 0, wxALIGN_RIGHT, 5 );
	
	
	m_panel2->SetSizer( bSizer15 );
	m_panel2->Layout();
	bSizer15->Fit( m_panel2 );
	bSizer14->Add( m_panel2, 1, wxEXPAND | wxALL, 5 );
	
	
	this->SetSizer( bSizer14 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	tests->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SetupTestDialogDecl::OnChooseTest ), NULL, this );
	m_button8->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SetupTestDialogDecl::OnReloadXml ), NULL, this );
}

SetupTestDialogDecl::~SetupTestDialogDecl()
{
	// Disconnect Events
	tests->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SetupTestDialogDecl::OnChooseTest ), NULL, this );
	m_button8->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SetupTestDialogDecl::OnReloadXml ), NULL, this );
	
}
