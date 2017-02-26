///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 30 2013)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "CallStackDialogDecl.h"

///////////////////////////////////////////////////////////////////////////

CallStackDialogDecl::CallStackDialogDecl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxBoxSizer* bSizer3;
    bSizer3 = new wxBoxSizer( wxVERTICAL );

    callList = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE );
    callList->Append( wxT("MAIN") );
    callList->Append( wxT("BRANDONACCI(2)") );
    callList->Append( wxT("BRANDONACCI(1)") );
    callList->Append( wxT("BRANDONACCI(0)") );
    callList->SetValidator( wxGenericValidator( &frame ) );

    bSizer3->Add( callList, 1, wxEXPAND|wxALL, 4 );

    wxBoxSizer* bSizer4;
    bSizer4 = new wxBoxSizer( wxHORIZONTAL );


    bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );

    rewindButton = new wxButton( this, wxID_ANY, wxT("Rewind To Call"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer4->Add( rewindButton, 0, wxALL, 4 );

    viewStackButton = new wxButton( this, wxID_ANY, wxT("View Stack Frame"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer4->Add( viewStackButton, 0, wxALL, 4 );

    closeButton = new wxButton( this, wxID_OK, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer4->Add( closeButton, 0, wxALL, 4 );


    bSizer3->Add( bSizer4, 0, wxEXPAND, 4 );


    this->SetSizer( bSizer3 );
    this->Layout();

    this->Centre( wxBOTH );

    // Connect Events
    rewindButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CallStackDialogDecl::OnRewindToCall ), NULL, this );
    viewStackButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CallStackDialogDecl::OnViewStackFrame ), NULL, this );
}

CallStackDialogDecl::~CallStackDialogDecl()
{
    // Disconnect Events
    rewindButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CallStackDialogDecl::OnRewindToCall ), NULL, this );
    viewStackButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CallStackDialogDecl::OnViewStackFrame ), NULL, this );

}
