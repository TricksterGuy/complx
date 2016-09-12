///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __LC3EDITFRAMEDECL_H__
#define __LC3EDITFRAMEDECL_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/wxFlatNotebook/wxFlatNotebook.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/statusbr.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/docview.h>
#include <wx/docmdi.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class LC3EditFrameDecl
///////////////////////////////////////////////////////////////////////////////
class LC3EditFrameDecl : public wxDocMDIParentFrame
{
	private:

	protected:
		wxPanel* m_panel1;
		wxFlatNotebook* fileNotebook;
		wxStatusBar* statusBar;
		wxMenuBar* m_menubar1;
		wxMenu* file;
		wxMenu* edit;
		wxMenu* view;
		wxMenu* run;
		wxMenu* help;

	public:

		LC3EditFrameDecl( wxDocManager* manager, wxFrame* parent, wxWindowID id = wxID_ANY, const wxString& title = _("LC3-Edit"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 640,480 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~LC3EditFrameDecl();

};

#endif //__LC3EDITFRAMEDECL_H__
