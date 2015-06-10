///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 25 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __MEMORYVIEWFRAMEDECL_H__
#define __MEMORYVIEWFRAMEDECL_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
class MemoryGrid;

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/grid.h>
#include <wx/sizer.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class MemoryViewFrameDecl
///////////////////////////////////////////////////////////////////////////////
class MemoryViewFrameDecl : public wxFrame 
{
	private:
	
	protected:
		enum
		{
			ID_GOTO_ADDRESS = 1000,
			ID_SHOW_ALL,
			ID_SHOW_NON_ZERO,
			ID_SHOW_ONLY_CODEDATA,
			ID_CUSTOM,
			ID_BASIC,
			ID_NORMAL,
			ID_HIGH_LEVEL,
			ID_INSTRUCTION_HIGHLIGHTING
		};
		
		wxMenuBar* menu;
		wxMenu* menuView1;
		wxMenu* menuViewHideAddresses;
		wxMenuItem* menuViewHideAddressesShowAll;
		wxMenuItem* menuViewHideAddressesShowNonZero;
		wxMenuItem* menuViewHideAddressesShowOnlyCodeData;
		wxMenuItem* menuViewHideAddressesCustom;
		wxMenu* menuViewDisassemble;
		wxMenuItem* menuViewBasic;
		wxMenuItem* menuViewNormal;
		wxMenuItem* menuViewHighLevel;
		wxMenuItem* menuViewInstructionHighlighting;
		MemoryGrid* memory;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnGoto( wxCommandEvent& event ) = 0;
		virtual void OnUpdateHideAddresses( wxCommandEvent& event ) = 0;
		virtual void OnHideAddressesCustom( wxCommandEvent& event ) = 0;
		virtual void OnDumbDisassemble( wxCommandEvent& event ) = 0;
		virtual void OnNormalDisassemble( wxCommandEvent& event ) = 0;
		virtual void OnCDisassemble( wxCommandEvent& event ) = 0;
		virtual void OnInstructionHighlight( wxCommandEvent& event ) = 0;
		
	
	public:
		
		MemoryViewFrameDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Memory View"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 608,352 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		
		~MemoryViewFrameDecl();
	
};

#endif //__MEMORYVIEWFRAMEDECL_H__
