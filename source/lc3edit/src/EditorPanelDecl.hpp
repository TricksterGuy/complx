///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct  8 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __EDITORFRAMEDECL_H__
#define __EDITORFRAMEDECL_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#ifdef __VISUALC__
#include <wx/link_additions.h>
#endif //__VISUALC__
#include <wx/sizer.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/wxscintilla.h>

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

#endif //__EDITORFRAMEDECL_H__
