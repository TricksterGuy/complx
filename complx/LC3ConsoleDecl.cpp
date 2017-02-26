///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct  8 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "LC3ConsoleDecl.h"

///////////////////////////////////////////////////////////////////////////

LC3ConsoleDecl::LC3ConsoleDecl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer( wxVERTICAL );

    panel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* bSizer3;
    bSizer3 = new wxBoxSizer( wxVERTICAL );

    m_splitter1 = new wxSplitterWindow( panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
    //m_splitter1->SetSashSize( 8 );
    m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( LC3ConsoleDecl::m_splitter1OnIdle ), NULL, this );
    m_splitter1->SetMinimumPaneSize( 32 );

    m_panel2 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* bSizer31;
    bSizer31 = new wxBoxSizer( wxVERTICAL );

    output = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
    output->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, false, wxT("Courier New") ) );

    bSizer31->Add( output, 1, wxALL|wxEXPAND, 0 );


    m_panel2->SetSizer( bSizer31 );
    m_panel2->Layout();
    bSizer31->Fit( m_panel2 );
    m_panel3 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* bSizer4;
    bSizer4 = new wxBoxSizer( wxVERTICAL );

    input_text = new wxTextCtrl( m_panel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_ENTER );
    input_text->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, false, wxT("Courier New") ) );
    input_text->SetMinSize( wxSize( -1,30 ) );

    input_text->SetValidator( wxTextValidator( wxFILTER_NONE, &input ) );

    bSizer4->Add( input_text, 1, wxALL|wxEXPAND, 0 );


    m_panel3->SetSizer( bSizer4 );
    m_panel3->Layout();
    bSizer4->Fit( m_panel3 );
    m_splitter1->SplitHorizontally( m_panel2, m_panel3, 224 );
    bSizer3->Add( m_splitter1, 1, wxEXPAND, 5 );


    panel->SetSizer( bSizer3 );
    panel->Layout();
    bSizer3->Fit( panel );
    bSizer1->Add( panel, 1, wxEXPAND | wxALL, 0 );


    this->SetSizer( bSizer1 );
    this->Layout();

    this->Centre( wxBOTH );

    // Connect Events
    input_text->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( LC3ConsoleDecl::OnText ), NULL, this );
    input_text->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( LC3ConsoleDecl::OnEnterKey ), NULL, this );
}

LC3ConsoleDecl::~LC3ConsoleDecl()
{
    // Disconnect Events
    input_text->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( LC3ConsoleDecl::OnText ), NULL, this );
    input_text->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( LC3ConsoleDecl::OnEnterKey ), NULL, this );

}
