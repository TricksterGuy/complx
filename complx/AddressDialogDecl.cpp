///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  4 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "AddressDialogDecl.h"

///////////////////////////////////////////////////////////////////////////

AddressDialogDecl::AddressDialogDecl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxBoxSizer* bSizer4;
    bSizer4 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* bSizer13;
    bSizer13 = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer* bSizer14;
    bSizer14 = new wxBoxSizer( wxVERTICAL );

    m_staticText11 = new wxStaticText( this, wxID_ANY, _("Address:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText11->Wrap( -1 );
    bSizer14->Add( m_staticText11, 0, wxLEFT|wxTOP, 8 );

    address_text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), 0 );
    address_text->SetValidator( wxTextValidator( wxFILTER_ASCII, &address ) );

    bSizer14->Add( address_text, 0, wxLEFT|wxTOP, 5 );

    bSizer13->Add( bSizer14, 0, wxEXPAND, 5 );

    wxBoxSizer* bSizer15;
    bSizer15 = new wxBoxSizer( wxVERTICAL );

    m_staticText12 = new wxStaticText( this, wxID_ANY, _("Value:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText12->Wrap( -1 );
    bSizer15->Add( m_staticText12, 0, wxLEFT|wxTOP, 8 );

    value_text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), 0 );
    value_text->SetValidator( wxTextValidator( wxFILTER_ASCII, &value ) );

    bSizer15->Add( value_text, 0, wxLEFT|wxTOP, 5 );

    bSizer13->Add( bSizer15, 0, wxEXPAND, 5 );

    bSizer4->Add( bSizer13, 0, wxEXPAND, 5 );


    bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );

    wxBoxSizer* bSizer6;
    bSizer6 = new wxBoxSizer( wxHORIZONTAL );

    okButton = new wxButton( this, wxID_OK, _("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer6->Add( okButton, 0, wxALL, 5 );


    bSizer6->Add( 0, 0, 1, wxEXPAND, 5 );

    cancelButton = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer6->Add( cancelButton, 0, wxALL, 5 );

    bSizer4->Add( bSizer6, 0, wxEXPAND, 5 );

    this->SetSizer( bSizer4 );
    this->Layout();

    this->Centre( wxBOTH );
}

AddressDialogDecl::~AddressDialogDecl()
{
}
