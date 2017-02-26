///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  4 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DebugInfoDialogDecl__
#define __DebugInfoDialogDecl__

#include <wx/intl.h>

class DebugInfoList;

#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class DebugInfoDialogDecl
///////////////////////////////////////////////////////////////////////////////
class DebugInfoDialogDecl : public wxDialog
{
private:

protected:
    DebugInfoList* debugpoints;
    wxButton* addBButton;
    wxButton* addWButton;
    wxButton* editButton;
    wxButton* removeButton;


    wxButton* okButton;
    wxButton* cancelButton;

    // Virtual event handlers, overide them in your derived class
    virtual void OnBreakpoint( wxCommandEvent& event ) = 0;
    virtual void OnWatchpoint( wxCommandEvent& event ) = 0;
    virtual void OnEdit( wxCommandEvent& event ) = 0;
    virtual void OnRemove( wxCommandEvent& event ) = 0;


public:

    DebugInfoDialogDecl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Breakpoints and Watchpoints"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 480,320 ), long style = wxDEFAULT_DIALOG_STYLE );
    ~DebugInfoDialogDecl();

};

#endif //__DebugInfoDialogDecl__
