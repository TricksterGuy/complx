///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "COLORLCDGUI.h"

///////////////////////////////////////////////////////////////////////////

COLORLCDGUI::COLORLCDGUI( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	displayPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	displayPanel->SetMinSize( wxSize( 120,120 ) );
	
	bSizer1->Add( displayPanel, 1, wxEXPAND | wxALL, 0 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	displayPanel->Connect( wxEVT_PAINT, wxPaintEventHandler( COLORLCDGUI::OnPaint ), NULL, this );
}

COLORLCDGUI::~COLORLCDGUI()
{
	// Disconnect Events
	displayPanel->Disconnect( wxEVT_PAINT, wxPaintEventHandler( COLORLCDGUI::OnPaint ), NULL, this );
}
