///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "MemoryGrid.hpp"

#include "MemoryViewFrameDecl.h"

///////////////////////////////////////////////////////////////////////////

MemoryViewFrameDecl::MemoryViewFrameDecl( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	menu = new wxMenuBar( 0 );
	menuView1 = new wxMenu();
	wxMenuItem* menuViewGoto;
	menuViewGoto = new wxMenuItem( menuView1, ID_GOTO_ADDRESS, wxString( _("Goto Address") ) + wxT('\t') + wxT("Ctrl+G"), _("Moves memory to a specific address."), wxITEM_NORMAL );
	menuView1->Append( menuViewGoto );

	menuViewHideAddresses = new wxMenu();
	wxMenuItem* menuViewHideAddressesItem = new wxMenuItem( menuView1, wxID_ANY, _("Hide Addresses"), wxEmptyString, wxITEM_NORMAL, menuViewHideAddresses );
	menuViewHideAddressesShowAll = new wxMenuItem( menuViewHideAddresses, ID_SHOW_ALL, wxString( _("Show All") ) , _("Shows all memory addresses"), wxITEM_RADIO );
	menuViewHideAddresses->Append( menuViewHideAddressesShowAll );

	menuViewHideAddressesShowOnlyCodeData = new wxMenuItem( menuViewHideAddresses, ID_SHOW_ONLY_CODEDATA, wxString( _("Show Only Code/Data") ) , _("Shows addresses modified when your program was loaded."), wxITEM_RADIO );
	menuViewHideAddresses->Append( menuViewHideAddressesShowOnlyCodeData );

	menuViewHideAddressesCustom = new wxMenuItem( menuViewHideAddresses, ID_CUSTOM, wxString( _("Custom...") ) + wxT('\t') + wxT("Ctrl+H"), _("Allows you to show/hide memory address ranges"), wxITEM_RADIO );
	menuViewHideAddresses->Append( menuViewHideAddressesCustom );

	menuView1->Append( menuViewHideAddressesItem );

	menuViewDisassemble = new wxMenu();
	wxMenuItem* menuViewDisassembleItem = new wxMenuItem( menuView1, wxID_ANY, _("Disassemble"), wxEmptyString, wxITEM_NORMAL, menuViewDisassemble );
	menuViewBasic = new wxMenuItem( menuViewDisassemble, ID_BASIC, wxString( _("Basic") ) , _("Disassembles ignoring symbols"), wxITEM_RADIO );
	menuViewDisassemble->Append( menuViewBasic );

	menuViewNormal = new wxMenuItem( menuViewDisassemble, ID_NORMAL, wxString( _("Normal") ) , _("Disassembles with symbol info."), wxITEM_RADIO );
	menuViewDisassemble->Append( menuViewNormal );

	menuViewHighLevel = new wxMenuItem( menuViewDisassemble, ID_HIGH_LEVEL, wxString( _("High Level") ) , _("Disassembles into C-like statements"), wxITEM_RADIO );
	menuViewDisassemble->Append( menuViewHighLevel );
	menuViewHighLevel->Check( true );

	menuView1->Append( menuViewDisassembleItem );

	menuViewInstructionHighlighting = new wxMenuItem( menuView1, ID_INSTRUCTION_HIGHLIGHTING, wxString( _("Instruction Highlighting") ) , _("Highlights bits of the instruction."), wxITEM_CHECK );
	menuView1->Append( menuViewInstructionHighlighting );
	menuViewInstructionHighlighting->Check( true );

	menuViewMemoryFlip = new wxMenuItem( menuView1, ID_FLIP_MEMORY, wxString( _("Memory Flip") ) , _("Reverses memory such that high addresses come first."), wxITEM_CHECK );
	menuView1->Append( menuViewMemoryFlip );

	menu->Append( menuView1, _("View") );

	this->SetMenuBar( menu );

	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );

	memory = new MemoryGrid( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );

	// Grid
	memory->CreateGrid( 1, 8 );
	memory->EnableEditing( true );
	memory->EnableGridLines( false );
	memory->EnableDragGridSize( false );
	memory->SetMargins( 0, 0 );

	// Columns
	memory->EnableDragColMove( true );
	memory->EnableDragColSize( true );
	memory->SetColLabelSize( 20 );
	memory->SetColLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Rows
	memory->EnableDragRowSize( false );
	memory->SetRowLabelSize( 0 );
	memory->SetRowLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Label Appearance
	memory->SetLabelFont( wxFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	// Cell Defaults
	memory->SetDefaultCellFont( wxFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );
	memory->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_CENTER );
	bSizer13->Add( memory, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer13 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	menuView1->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnGoto ), this, menuViewGoto->GetId());
	menuViewHideAddresses->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnUpdateHideAddresses ), this, menuViewHideAddressesShowAll->GetId());
	menuViewHideAddresses->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnUpdateHideAddresses ), this, menuViewHideAddressesShowOnlyCodeData->GetId());
	menuViewHideAddresses->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnHideAddressesCustom ), this, menuViewHideAddressesCustom->GetId());
	menuViewDisassemble->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnDumbDisassemble ), this, menuViewBasic->GetId());
	menuViewDisassemble->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnNormalDisassemble ), this, menuViewNormal->GetId());
	menuViewDisassemble->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnCDisassemble ), this, menuViewHighLevel->GetId());
	menuView1->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnInstructionHighlight ), this, menuViewInstructionHighlighting->GetId());
	menuView1->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnFlipMemory ), this, menuViewMemoryFlip->GetId());
}

MemoryViewFrameDecl::~MemoryViewFrameDecl()
{
	// Disconnect Events

}
