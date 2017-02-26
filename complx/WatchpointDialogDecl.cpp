///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "WatchpointDialogDecl.h"

///////////////////////////////////////////////////////////////////////////

WatchpointDialogDecl::WatchpointDialogDecl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxBoxSizer* bSizer3;
    bSizer3 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer* fgSizer2;
    fgSizer2 = new wxFlexGridSizer( 6, 2, 0, 0 );
    fgSizer2->SetFlexibleDirection( wxBOTH );
    fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_staticText2 = new wxStaticText( this, wxID_ANY, _("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText2->Wrap( -1 );
    fgSizer2->Add( m_staticText2, 0, wxALL, 8 );

    name_text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    name_text->SetMaxLength( 32 );
    name_text->SetValidator( wxTextValidator( wxFILTER_ASCII, &name ) );
    name_text->SetToolTip( _("Name of the watchpoint") );

    fgSizer2->Add( name_text, 0, wxALL|wxEXPAND, 5 );

    m_staticText1 = new wxStaticText( this, wxID_ANY, _("Target:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText1->Wrap( -1 );
    fgSizer2->Add( m_staticText1, 0, wxALL, 8 );

    target_text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    target_text->SetMaxLength( 32 );
    target_text->SetValidator( wxTextValidator( wxFILTER_ASCII, &target ) );
    target_text->SetToolTip( _("What the watchpoint is looking at (Can be a memory address or a register)") );

    fgSizer2->Add( target_text, 0, wxALL|wxEXPAND, 5 );

    m_staticText3 = new wxStaticText( this, wxID_ANY, _("Condition:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText3->Wrap( -1 );
    fgSizer2->Add( m_staticText3, 0, wxALL, 8 );

    condition_text = new wxTextCtrl( this, wxID_ANY, _("1"), wxDefaultPosition, wxDefaultSize, 0 );
    condition_text->SetMaxLength( 128 );
    condition_text->SetValidator( wxTextValidator( wxFILTER_ASCII, &condition ) );
    condition_text->SetToolTip( _("Expression specifying a condition which when true (!0)\n activates the watchpoint.\n(Enter an expression i.e. R0+5 or PC+10 or LABEL[0])\n") );

    fgSizer2->Add( condition_text, 0, wxALL|wxEXPAND, 5 );

    m_staticText4 = new wxStaticText( this, wxID_ANY, _("Times:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText4->Wrap( -1 );
    fgSizer2->Add( m_staticText4, 0, wxALL, 8 );

    times_spin = new wxSpinCtrl( this, wxID_ANY, wxT("-1"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -1, 1000000, 0 );
    times_spin->SetValidator( wxGenericValidator( &times ) );
    times_spin->SetToolTip( _("After the watchpoint has been triggered this many times it will disappear.\nEnter -1 for infinite.") );

    fgSizer2->Add( times_spin, 0, wxALL, 5 );

    m_staticText5 = new wxStaticText( this, wxID_ANY, _("Hits"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText5->Wrap( -1 );
    fgSizer2->Add( m_staticText5, 0, wxALL, 8 );

    hits_spin = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, 1000000, 0 );
    hits_spin->SetValidator( wxGenericValidator( &hits ) );
    hits_spin->Enable( false );
    hits_spin->SetToolTip( _("The number of times the watchpoint was triggered") );

    fgSizer2->Add( hits_spin, 0, wxALL, 5 );

    enable_check = new wxCheckBox( this, wxID_ANY, _("Enabled?"), wxDefaultPosition, wxDefaultSize, 0 );
    enable_check->SetValue(true);
    enable_check->SetValidator( wxGenericValidator( &enabled ) );
    enable_check->SetToolTip( _("Is the watchpoint enabled?") );

    fgSizer2->Add( enable_check, 0, wxALL, 5 );


    fgSizer2->Add( 0, 0, 1, wxEXPAND, 5 );

    bSizer3->Add( fgSizer2, 0, wxEXPAND, 5 );

    wxBoxSizer* bSizer5;
    bSizer5 = new wxBoxSizer( wxHORIZONTAL );


    bSizer5->Add( 0, 0, 1, wxEXPAND, 5 );

    okButton = new wxButton( this, wxID_OK, _("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer5->Add( okButton, 0, wxALL, 5 );

    cancelButton = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer5->Add( cancelButton, 0, wxALL, 5 );

    bSizer3->Add( bSizer5, 0, wxEXPAND, 5 );

    this->SetSizer( bSizer3 );
    this->Layout();

    this->Centre( wxBOTH );
}

WatchpointDialogDecl::~WatchpointDialogDecl()
{
}
