///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jan  1 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "AdvancedLoadDialogDecl.h"

///////////////////////////////////////////////////////////////////////////

AdvancedLoadDialogDecl::AdvancedLoadDialogDecl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_panel2 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 9, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( m_panel2, wxID_ANY, _("Assembly File:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	m_staticText1->SetForegroundColour( wxColour( 255, 0, 0 ) );
	
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	assemblyFile = new wxFilePickerCtrl( m_panel2, wxID_ANY, wxEmptyString, _("Select an assembly file"), wxT("*.asm"), wxDefaultPosition, wxDefaultSize, wxFLP_CHANGE_DIR|wxFLP_DEFAULT_STYLE|wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_USE_TEXTCTRL );
	assemblyFile->SetToolTip( _("Assembly File to load.  This is required") );
	
	fgSizer1->Add( assemblyFile, 1, wxEXPAND|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_staticText2 = new wxStaticText( m_panel2, wxID_ANY, _("Test XML File:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	xmlFile = new wxFilePickerCtrl( m_panel2, wxID_ANY, wxEmptyString, _("Select an xml file"), wxT("*.xml"), wxDefaultPosition, wxDefaultSize, wxFLP_CHANGE_DIR|wxFLP_DEFAULT_STYLE|wxFLP_FILE_MUST_EXIST|wxFLP_OPEN|wxFLP_USE_TEXTCTRL );
	xmlFile->SetToolTip( _("Test XML file for Setup Test Dialog functions.") );
	
	fgSizer1->Add( xmlFile, 1, wxEXPAND|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_staticText6 = new wxStaticText( m_panel2, wxID_ANY, _("Console Input:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer1->Add( m_staticText6, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	consoleInput = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	consoleInput->SetToolTip( _("Input present in LC3 console when program is loaded") );
	
	fgSizer1->Add( consoleInput, 1, wxEXPAND|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_staticText0 = new wxStaticText( m_panel2, wxID_ANY, _("Registers?"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText0->Wrap( -1 );
	fgSizer1->Add( m_staticText0, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	wxString regInitializerChoices[] = { _("RANDOMIZE"), _("ZERO"), _("FILL WITH...") };
	int regInitializerNChoices = sizeof( regInitializerChoices ) / sizeof( wxString );
	regInitializer = new wxChoice( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, regInitializerNChoices, regInitializerChoices, 0 );
	regInitializer->SetSelection( 0 );
	regInitializer->SetToolTip( _("Whether to randomize, zero out, or fill with a specific value") );
	
	bSizer5->Add( regInitializer, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	regFillValue = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	regFillValue->Enable( false );
	
	bSizer5->Add( regFillValue, 0, wxALL, 5 );
	
	
	fgSizer1->Add( bSizer5, 1, wxEXPAND, 5 );
	
	m_staticText4 = new wxStaticText( m_panel2, wxID_ANY, _("Memory?"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer1->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer51;
	bSizer51 = new wxBoxSizer( wxHORIZONTAL );
	
	wxString memInitializerChoices[] = { _("RANDOMIZE"), _("ZERO"), _("FILL WITH...") };
	int memInitializerNChoices = sizeof( memInitializerChoices ) / sizeof( wxString );
	memInitializer = new wxChoice( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, memInitializerNChoices, memInitializerChoices, 0 );
	memInitializer->SetSelection( 0 );
	memInitializer->SetToolTip( _("Whether to randomize, zero out, or fill with a specific value") );
	
	bSizer51->Add( memInitializer, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	memFillValue = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	memFillValue->Enable( false );
	
	bSizer51->Add( memFillValue, 0, wxALL, 5 );
	
	
	fgSizer1->Add( bSizer51, 1, wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( m_panel2, wxID_ANY, _("PC"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer1->Add( m_staticText5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	pcValue = new wxTextCtrl( m_panel2, wxID_ANY, _("x3000"), wxDefaultPosition, wxDefaultSize, wxTE_RIGHT );
	fgSizer1->Add( pcValue, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_staticText7 = new wxStaticText( m_panel2, wxID_ANY, _("True Traps?"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer1->Add( m_staticText7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	trueTraps = new wxCheckBox( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	trueTraps->SetToolTip( _("Enable executing of trap handler routines in asssembly instead of C.  (This is automatically determined by your assembly code).") );
	
	fgSizer1->Add( trueTraps, 0, wxEXPAND|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_staticText8 = new wxStaticText( m_panel2, wxID_ANY, _("Interrupts?"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	fgSizer1->Add( m_staticText8, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	interrupts = new wxCheckBox( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	interrupts->SetToolTip( _("Enable processing of lc3 interrupts.  This is automatically configued based on your assembly code.") );
	
	fgSizer1->Add( interrupts, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_staticText81 = new wxStaticText( m_panel2, wxID_ANY, _("Strict Execution"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText81->Wrap( -1 );
	fgSizer1->Add( m_staticText81, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	strictExecution = new wxCheckBox( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	strictExecution->SetValue(true); 
	strictExecution->SetToolTip( _("Enable processing of lc3 interrupts.  This is automatically configued based on your assembly code.") );
	
	fgSizer1->Add( strictExecution, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	
	bSizer7->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button1 = new wxButton( m_panel2, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_button1, 0, wxALL, 5 );
	
	m_button2 = new wxButton( m_panel2, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_button2, 0, wxALL, 5 );
	
	
	bSizer7->Add( bSizer9, 0, wxALIGN_RIGHT, 5 );
	
	
	m_panel2->SetSizer( bSizer7 );
	m_panel2->Layout();
	bSizer7->Fit( m_panel2 );
	bSizer6->Add( m_panel2, 1, wxEXPAND | wxALL, 5 );
	
	
	this->SetSizer( bSizer6 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	regInitializer->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( AdvancedLoadDialogDecl::OnRegChoice ), NULL, this );
	memInitializer->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( AdvancedLoadDialogDecl::OnMemChoice ), NULL, this );
}

AdvancedLoadDialogDecl::~AdvancedLoadDialogDecl()
{
	// Disconnect Events
	regInitializer->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( AdvancedLoadDialogDecl::OnRegChoice ), NULL, this );
	memInitializer->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( AdvancedLoadDialogDecl::OnMemChoice ), NULL, this );
	
}
