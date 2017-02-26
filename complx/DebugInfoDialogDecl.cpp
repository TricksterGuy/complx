///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  4 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "DebugInfoList.hpp"

#include "DebugInfoDialogDecl.h"

///////////////////////////////////////////////////////////////////////////

DebugInfoDialogDecl::DebugInfoDialogDecl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxSize( 480,320 ), wxSize( 480,320 ) );

    wxBoxSizer* bSizer13;
    bSizer13 = new wxBoxSizer( wxVERTICAL );

    debugpoints = new DebugInfoList( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VIRTUAL|wxLC_VRULES );
    bSizer13->Add( debugpoints, 1, wxALL|wxEXPAND, 5 );

    wxBoxSizer* bSizer3;
    bSizer3 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* bSizer31;
    bSizer31 = new wxBoxSizer( wxHORIZONTAL );

    addBButton = new wxButton( this, wxID_ANY, _("New Breakpoint"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer31->Add( addBButton, 1, wxALL, 5 );

    addWButton = new wxButton( this, wxID_ANY, _("New Watchpoint"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer31->Add( addWButton, 1, wxALL, 5 );

    editButton = new wxButton( this, wxID_ANY, _("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer31->Add( editButton, 1, wxALL, 5 );

    removeButton = new wxButton( this, wxID_ANY, _("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer31->Add( removeButton, 1, wxALL, 5 );

    bSizer3->Add( bSizer31, 1, wxEXPAND, 5 );

    wxBoxSizer* bSizer4;
    bSizer4 = new wxBoxSizer( wxHORIZONTAL );


    bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );


    bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );

    okButton = new wxButton( this, wxID_OK, _("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer4->Add( okButton, 0, wxALL, 5 );

    cancelButton = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer4->Add( cancelButton, 0, wxALL, 5 );

    bSizer3->Add( bSizer4, 1, wxEXPAND, 5 );

    bSizer13->Add( bSizer3, 0, wxEXPAND, 5 );

    this->SetSizer( bSizer13 );
    this->Layout();

    this->Centre( wxBOTH );

    // Connect Events
    addBButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugInfoDialogDecl::OnBreakpoint ), NULL, this );
    addWButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugInfoDialogDecl::OnWatchpoint ), NULL, this );
    editButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugInfoDialogDecl::OnEdit ), NULL, this );
    removeButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugInfoDialogDecl::OnRemove ), NULL, this );
}

DebugInfoDialogDecl::~DebugInfoDialogDecl()
{
    // Disconnect Events
    addBButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugInfoDialogDecl::OnBreakpoint ), NULL, this );
    addWButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugInfoDialogDecl::OnWatchpoint ), NULL, this );
    editButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugInfoDialogDecl::OnEdit ), NULL, this );
    removeButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugInfoDialogDecl::OnRemove ), NULL, this );

}
