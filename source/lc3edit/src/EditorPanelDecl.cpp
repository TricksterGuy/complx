#include "EditorPanelDecl.hpp"

EditorPanelDecl::EditorPanelDecl( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	editor = new wxScintilla( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxEmptyString );
	editor->SetUseTabs( false );
	editor->SetTabWidth( 4 );
	editor->SetIndent( 4 );
	editor->SetTabIndents( true );
	editor->SetBackSpaceUnIndents( true );
	editor->SetViewEOL( false );
	editor->SetViewWhiteSpace( false );
	editor->SetMarginWidth( 2, 0 );
	editor->SetIndentationGuides( false );
	editor->SetMarginType( 1, wxSCI_MARGIN_SYMBOL );
	editor->SetMarginMask( 1, wxSCI_MASK_FOLDERS );
	editor->SetMarginWidth( 1, 16);
	editor->SetMarginSensitive( 1, true );
	editor->SetProperty( wxT("fold"), wxT("1") );
	editor->SetFoldFlags( wxSCI_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSCI_FOLDFLAG_LINEAFTER_CONTRACTED );
	editor->SetMarginType( 0, wxSCI_MARGIN_NUMBER );
	editor->SetMarginWidth( 0, editor->TextWidth( wxSCI_STYLE_LINENUMBER, wxT("_99999") ) );
	editor->MarkerDefine( wxSCI_MARKNUM_FOLDER, wxSCI_MARK_BOXPLUS );
	editor->MarkerSetBackground( wxSCI_MARKNUM_FOLDER, wxColour( wxT("BLACK") ) );
	editor->MarkerSetForeground( wxSCI_MARKNUM_FOLDER, wxColour( wxT("WHITE") ) );
	editor->MarkerDefine( wxSCI_MARKNUM_FOLDEROPEN, wxSCI_MARK_BOXMINUS );
	editor->MarkerSetBackground( wxSCI_MARKNUM_FOLDEROPEN, wxColour( wxT("BLACK") ) );
	editor->MarkerSetForeground( wxSCI_MARKNUM_FOLDEROPEN, wxColour( wxT("WHITE") ) );
	editor->MarkerDefine( wxSCI_MARKNUM_FOLDERSUB, wxSCI_MARK_EMPTY );
	editor->MarkerDefine( wxSCI_MARKNUM_FOLDEREND, wxSCI_MARK_BOXPLUS );
	editor->MarkerSetBackground( wxSCI_MARKNUM_FOLDEREND, wxColour( wxT("BLACK") ) );
	editor->MarkerSetForeground( wxSCI_MARKNUM_FOLDEREND, wxColour( wxT("WHITE") ) );
	editor->MarkerDefine( wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_BOXMINUS );
	editor->MarkerSetBackground( wxSCI_MARKNUM_FOLDEROPENMID, wxColour( wxT("BLACK") ) );
	editor->MarkerSetForeground( wxSCI_MARKNUM_FOLDEROPENMID, wxColour( wxT("WHITE") ) );
	editor->MarkerDefine( wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_EMPTY );
	editor->MarkerDefine( wxSCI_MARKNUM_FOLDERTAIL, wxSCI_MARK_EMPTY );
	editor->SetSelBackground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
	editor->SetSelForeground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
	bSizer3->Add( editor, 1, wxEXPAND | wxALL, 0 );
	
	
	this->SetSizer( bSizer3 );
	this->Layout();
}

EditorPanelDecl::~EditorPanelDecl()
{
}
