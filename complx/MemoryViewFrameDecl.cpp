///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 25 2015)
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
	menuView1 = new wxMenu();
	wxMenuItem* menuViewGoto;
	menuViewGoto = new wxMenuItem( menuView1, ID_GOTO_ADDRESS, wxString( _("Goto Address") ) + wxT('\t') + wxT("Ctrl+G"), _("Moves memory to a specific address."), wxITEM_NORMAL );
	menuView1->Append( menuViewGoto );
	
	menuViewHideAddresses = new wxMenu();
	wxMenuItem* menuViewHideAddressesItem = new wxMenuItem( menuView1, wxID_ANY, _("Hide Addresses"), wxEmptyString, wxITEM_NORMAL, menuViewHideAddresses );
	menuViewHideAddressesShowAll = new wxMenuItem( menuViewHideAddresses, ID_SHOW_ALL, wxString( _("Show All") ) , _("Shows all memory addresses"), wxITEM_RADIO );
	menuViewHideAddresses->Append( menuViewHideAddressesShowAll );
	
	menuViewHideAddressesShowNonZero = new wxMenuItem( menuViewHideAddresses, ID_SHOW_NON_ZERO, wxString( _("Show Non Zero") ) , _("Show all memory addresses that have a value other than 0"), wxITEM_RADIO );
	menuViewHideAddresses->Append( menuViewHideAddressesShowNonZero );
	
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
	this->Connect( menuViewGoto->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnGoto ) );
	this->Connect( menuViewHideAddressesShowAll->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnUpdateHideAddresses ) );
	this->Connect( menuViewHideAddressesShowNonZero->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnUpdateHideAddresses ) );
	this->Connect( menuViewHideAddressesShowOnlyCodeData->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnUpdateHideAddresses ) );
	this->Connect( menuViewHideAddressesCustom->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnHideAddressesCustom ) );
	this->Connect( menuViewBasic->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnDumbDisassemble ) );
	this->Connect( menuViewNormal->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnNormalDisassemble ) );
	this->Connect( menuViewHighLevel->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnCDisassemble ) );
	this->Connect( menuViewInstructionHighlighting->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnInstructionHighlight ) );
	this->Connect( menuViewMemoryFlip->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnFlipMemory ) );
}

MemoryViewFrameDecl::~MemoryViewFrameDecl()
{
	// Disconnect Events
	this->Disconnect( ID_GOTO_ADDRESS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnGoto ) );
	this->Disconnect( ID_SHOW_ALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnUpdateHideAddresses ) );
	this->Disconnect( ID_SHOW_NON_ZERO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnUpdateHideAddresses ) );
	this->Disconnect( ID_SHOW_ONLY_CODEDATA, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnUpdateHideAddresses ) );
	this->Disconnect( ID_CUSTOM, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnHideAddressesCustom ) );
	this->Disconnect( ID_BASIC, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnDumbDisassemble ) );
	this->Disconnect( ID_NORMAL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnNormalDisassemble ) );
	this->Disconnect( ID_HIGH_LEVEL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnCDisassemble ) );
	this->Disconnect( ID_INSTRUCTION_HIGHLIGHTING, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnInstructionHighlight ) );
	this->Disconnect( ID_FLIP_MEMORY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MemoryViewFrameDecl::OnFlipMemory ) );
	
}
