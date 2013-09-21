///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 11 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "RunForDialogDecl.h"

///////////////////////////////////////////////////////////////////////////

RunForDialogDecl::RunForDialogDecl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );


	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );

	m_staticText12 = new wxStaticText( this, wxID_ANY, _("Run for how many instructions?"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	bSizer15->Add( m_staticText12, 0, wxLEFT|wxTOP, 8 );

	m_staticText3 = new wxStaticText( this, wxID_ANY, _("(Note enter a negative value to rewind)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer15->Add( m_staticText3, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );

	value_text = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -2147483647, 214783646, 1 );
	value_text->SetValidator( wxGenericValidator(&value ) );
	bSizer7->Add( value_text, 0, wxALL|wxALIGN_RIGHT, 5 );


	bSizer15->Add( bSizer7, 1, wxEXPAND, 5 );


	bSizer4->Add( bSizer15, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );


	bSizer6->Add( 0, 0, 1, wxEXPAND, 5 );

	okButton = new wxButton( this, wxID_OK, _("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( okButton, 0, wxALL, 5 );

	cancelButton = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( cancelButton, 0, wxALL, 5 );


	bSizer4->Add( bSizer6, 0, wxEXPAND, 5 );


	this->SetSizer( bSizer4 );
	this->Layout();

	this->Centre( wxBOTH );
}

RunForDialogDecl::~RunForDialogDecl()
{
}
