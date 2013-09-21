///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "MemoryGrid.hpp"

#include "MemoryViewFrameDecl.h"

///////////////////////////////////////////////////////////////////////////

MemoryViewFrameDecl::MemoryViewFrameDecl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	menu = new wxMenuBar( 0 );
	menuView = new wxMenu();
	wxMenuItem* menuViewGoto;
	menuViewGoto = new wxMenuItem( menuView, ID_GOTO_ADDRESS, wxString( _("Goto Address") ) + wxT('\t') + wxT("Ctrl+G"), _("Shows a specific address"), wxITEM_NORMAL );
	menuView->Append( menuViewGoto );
	
	menu->Append( menuView, _("View") ); 
	
	this->SetMenuBar( menu );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	memory = new MemoryGrid( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	// Grid
	memory->CreateGrid( 5, 5 );
	memory->EnableEditing( true );
	memory->EnableGridLines( false );
	memory->EnableDragGridSize( false );
	memory->SetMargins( 0, 0 );
	
	// Columns
	memory->EnableDragColMove( true );
	memory->EnableDragColSize( true );
	memory->SetColLabelSize( 20 );
	memory->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	memory->EnableDragRowSize( false );
	memory->SetRowLabelSize( 0 );
	memory->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	memory->SetLabelFont( wxFont( 10, 70, 90, 90, false, wxT("Courier New") ) );
	
	// Cell Defaults
	memory->SetDefaultCellFont( wxFont( 10, 70, 90, 90, false, wxT("Courier New") ) );
	memory->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_CENTRE );
	bSizer13->Add( memory, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer13 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( ID_GOTO_ADDRESS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnGoto ) );
}

MemoryViewFrameDecl::~MemoryViewFrameDecl()
{
	// Disconnect Events
	this->Disconnect( ID_GOTO_ADDRESS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnGoto ) );
	
}
