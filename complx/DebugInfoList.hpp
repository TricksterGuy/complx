#ifndef DEBUG_INFO_LIST_HPP
#define DEBUG_INFO_LIST_HPP

#include <lc3_all.hpp>
#include <vector>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/validate.h>
#include <wx/window.h>
#include <wx/listctrl.h>

enum
{
    DebugInfoName = 0,
    DebugInfoAddress,
    DebugInfoEnabled,
    DebugInfoCondition,
    DebugInfoTimes,
    DebugInfoHits,
};

class DebugInfoList : public wxListCtrl
{
public:
    DebugInfoList(wxWindow *parent, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxLC_ICON, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxListCtrlNameStr);
    void OnBreakpoint(wxCommandEvent& event);
    void OnWatchpoint(wxCommandEvent& event);
    void OnEdit(wxCommandEvent& event);
    void OnRemove(wxCommandEvent& event);
    std::vector<lc3_breakpoint_info> GetBreakpoints() const;
    std::vector<lc3_watchpoint_info> GetRegWatchpoints() const;
    std::vector<lc3_watchpoint_info> GetMemWatchpoints() const;
protected:
    virtual wxString OnGetItemText(long item, long column) const;
    wxString OnGetBreakpointText(long item, long column) const;
    wxString OnGetWatchpointText(long item, long column) const;
    virtual wxListItemAttr* OnGetItemAttr(long item) const;
private:
    void InitList(void);
    std::vector<lc3_breakpoint_info> breakpoints;
    std::vector<lc3_watchpoint_info> reg_watchpoints;
    std::vector<lc3_watchpoint_info> mem_watchpoints;
};

#endif
