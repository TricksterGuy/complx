///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct  8 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __LC3EDITFRAMEDECL_H__
#define __LC3EDITFRAMEDECL_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/wxFlatNotebook/wxFlatNotebook.h>
#ifdef __VISUALC__
#include <wx/link_additions.h>
#endif //__VISUALC__
#include <wx/sizer.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/wxscintilla.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class LC3EditFrameDecl
///////////////////////////////////////////////////////////////////////////////
class LC3EditFrameDecl : public wxFrame
{
	private:

	protected:
		wxMenuBar* m_menubar1;
		wxMenu* file;
		wxMenu* edit;
		wxMenu* view;
		wxMenu* run;
		wxMenu* help;
		wxFlatNotebook* fileNotebook;
		wxStatusBar* statusBar;

	public:

		LC3EditFrameDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("LC3 EDIT"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 640,480 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~LC3EditFrameDecl();

};

///////////////////////////////////////////////////////////////////////////////
/// Class EditorPanelDecl
///////////////////////////////////////////////////////////////////////////////
class EditorPanelDecl : public wxPanel
{
	private:

	protected:
		wxScintilla* editor;

	public:

		EditorPanelDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~EditorPanelDecl();

};

#endif //__LC3EDITFRAMEDECL_H__
