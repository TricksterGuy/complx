#include "DebugInfoList.hpp"
#include "BreakpointDialog.hpp"
#include "WatchpointDialog.hpp"
#include <wx/msgdlg.h>

extern lc3_state state;
void PrintError(int error);

/** DebugInfoList
  *
  * Constructor
  */
DebugInfoList::DebugInfoList(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxValidator &validator, const wxString &name)
    : wxListCtrl(parent, id, pos, size, style, validator, name)
{
    breakpoints.clear();
    reg_watchpoints.clear();
    mem_watchpoints.clear();

    std::map<unsigned short, lc3_breakpoint_info>::iterator i;
    std::map<unsigned short, lc3_watchpoint_info>::iterator j;

    for (i = state.breakpoints.begin(); i != state.breakpoints.end(); i++)
        breakpoints.push_back(i->second);

    for (j = state.reg_watchpoints.begin(); j != state.reg_watchpoints.end(); j++)
        reg_watchpoints.push_back(j->second);

    for (j = state.mem_watchpoints.begin(); j != state.mem_watchpoints.end(); j++)
        mem_watchpoints.push_back(j->second);

    InitList();
}

/** GetMemWatchpoints
  */
std::vector<lc3_watchpoint_info> DebugInfoList::GetMemWatchpoints() const
{
    return mem_watchpoints;
}

/** GetRegWatchpoints
  */
std::vector<lc3_watchpoint_info> DebugInfoList::GetRegWatchpoints() const
{
    return reg_watchpoints;
}

/** GetBreakpoints
  */
std::vector<lc3_breakpoint_info> DebugInfoList::GetBreakpoints() const
{
    return breakpoints;
}

/** InitList
  *
  * Initializes the listctrl
  */
void DebugInfoList::InitList(void)
{
    ClearAll();
    SetItemCount(breakpoints.size() + reg_watchpoints.size() + mem_watchpoints.size());
    InsertColumn(DebugInfoName, _("Name"), wxLIST_FORMAT_LEFT, -1);
    InsertColumn(DebugInfoAddress, _("Target"), wxLIST_FORMAT_LEFT, -1);
    InsertColumn(DebugInfoEnabled, _("Enabled"), wxLIST_FORMAT_LEFT, -1);
    InsertColumn(DebugInfoCondition, _("Condition"), wxLIST_FORMAT_LEFT, -1);
    InsertColumn(DebugInfoTimes, _("Times"), wxLIST_FORMAT_LEFT, -1);
    InsertColumn(DebugInfoHits, _("Hits"), wxLIST_FORMAT_LEFT, -1);
}

/** OnGetItemAttr
  *
  * Required by wxListCtrl.
  */
wxListItemAttr* DebugInfoList::OnGetItemAttr(long item) const
{
    return NULL;
}

/** OnGetItemText
  *
  * Gets the text for the breakpoints.
  */
wxString DebugInfoList::OnGetItemText(long item, long column) const
{
    if ((unsigned) item < breakpoints.size())
        return OnGetBreakpointText(item, column);
    else
        return OnGetWatchpointText(item - breakpoints.size(), column);
}

/** OnGetBreakpointText
  *
  * Gets the text for the breakpoints.
  */
wxString DebugInfoList::OnGetBreakpointText(long item, long column) const
{
    lc3_breakpoint_info info = breakpoints[item];
    std::string sym = lc3_sym_rev_lookup(state, info.addr);
    wxString ret;

    switch(column)
    {
    case DebugInfoName:
        ret = wxString::FromUTF8(info.label.c_str());
        break;
    case DebugInfoAddress:
        ret = sym.empty() ? wxString::Format(_("x%04X"), info.addr) : wxString::FromUTF8(sym.c_str());
        break;
    case DebugInfoEnabled:
        ret = info.enabled ? _("YES") : _("NO");
        break;
    case DebugInfoCondition:
        ret = wxString::FromUTF8(info.condition.c_str());
        break;
    case DebugInfoTimes:
        ret = wxString::Format(_("%d"), info.max_hits);
        break;
    case DebugInfoHits:
        ret = wxString::Format(_("%d"), info.hit_count);
        break;
    default:
        ret = wxEmptyString;
        break;
    }

    return ret;
}

/** OnGetWatchpointText
  *
  * Gets the text for the watchpoints.
  */
wxString DebugInfoList::OnGetWatchpointText(long item, long column) const
{
    bool is_reg = (unsigned int) item < reg_watchpoints.size();
    lc3_watchpoint_info info = is_reg ? reg_watchpoints[item] : mem_watchpoints[item - reg_watchpoints.size()];
    std::string sym = lc3_sym_rev_lookup(state, info.data);
    wxString ret;

    switch(column)
    {
    case DebugInfoName:
        ret = wxString::FromUTF8(info.label.c_str());
        break;
    case DebugInfoAddress:
        if (is_reg)
            ret = wxString::Format(_("R%d"), info.data);
        else
            ret = sym.empty() ? wxString::Format(_("MEM[x%04X]"), info.data) : wxString::FromUTF8(sym.c_str());
        break;
    case DebugInfoEnabled:
        ret = info.enabled ? _("YES") : _("NO");
        break;
    case DebugInfoCondition:
        ret = wxString::FromUTF8(info.condition.c_str());
        break;
    case DebugInfoTimes:
        ret = wxString::Format(_("%d"), info.max_hits);
        break;
    case DebugInfoHits:
        ret = wxString::Format(_("%d"), info.hit_count);
        break;
    default:
        ret = wxEmptyString;
        break;
    }

    return ret;
}

/** OnRemove
  *
  * Called when something is about to be removed.
  */
void DebugInfoList::OnRemove(wxCommandEvent& event)
{
    long index = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (index == -1) return;

    if ((unsigned)index < breakpoints.size())
        breakpoints.erase(breakpoints.begin() + index);
    else if ((unsigned)(index -= breakpoints.size()) < reg_watchpoints.size())
        reg_watchpoints.erase(reg_watchpoints.begin() + index);
    else if ((unsigned) (index -= reg_watchpoints.size()) < mem_watchpoints.size())
        mem_watchpoints.erase(mem_watchpoints.begin() + index);

    InitList();
}

/** OnEdit
  *
  * Called when something is going to be edited
  */
void DebugInfoList::OnEdit(wxCommandEvent& event)
{
    long index = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (index == -1) return;
    long refresh = index;


    lc3_breakpoint_info bp_info;
    lc3_watchpoint_info wp_info;
    bool is_bp = false;

    if ((unsigned)index < breakpoints.size())
    {
        is_bp = true;
        bp_info = breakpoints[index];
    }
    else if ((unsigned)(index -= breakpoints.size()) < reg_watchpoints.size())
    {
        wp_info = reg_watchpoints[index];
    }
    else if ((unsigned) (index -= reg_watchpoints.size()) < mem_watchpoints.size())
    {
        wp_info = mem_watchpoints[index];
    }

    if (is_bp)
    {
        BreakpointDialog* dialog = new BreakpointDialog(this, bp_info, false);
        if (dialog->ShowModal() == wxID_OK)
        {
            int error;
            bp_info = dialog->GetInfo(error);

            if (error)
                PrintError(error);
            else
                breakpoints[index] = bp_info;

        }
        delete dialog;
    }
    else
    {
        WatchpointDialog* dialog = new WatchpointDialog(this, wp_info, false);
        if (dialog->ShowModal() == wxID_OK)
        {
            int error;
            wp_info = dialog->GetInfo(error);

            if (error)
                PrintError(error);
            else
            {
                if (wp_info.is_reg)
                    reg_watchpoints[index] = wp_info;
                else
                    mem_watchpoints[index] = wp_info;
            }
        }
        delete dialog;
    }

    RefreshItem(refresh);
}

/** OnWatchpoint
  *
  * Called on a new watchpoint
  */
void DebugInfoList::OnWatchpoint(wxCommandEvent& event)
{
    lc3_watchpoint_info info;

    info.is_reg = false;
    info.data = 0x3000;
    info.condition = "1";
    info.enabled = true;
    info.hit_count = 0;
    info.label = "";
    info.max_hits = -1;

    WatchpointDialog* dialog = new WatchpointDialog(this, info, true);
    if (dialog->ShowModal() == wxID_OK)
    {
        int error;
        info = dialog->GetInfo(error);

        if (error)
            PrintError(error);
        else
        {
            bool multiple = false;
            if (info.is_reg)
            {
                for (unsigned int i = 0; i < reg_watchpoints.size(); i++)
                {
                    if (reg_watchpoints[i].data == info.data)
                    {
                        multiple = true;
                        break;
                    }
                }
            }
            else
            {
                for (unsigned int i = 0; i < mem_watchpoints.size(); i++)
                {
                    if (mem_watchpoints[i].data == info.data)
                    {
                        multiple = true;
                        break;
                    }
                }
            }

            if (multiple)
                wxMessageBox(_("ERROR! You can't have two watchpoints refer to the same target"), _("Error"));
            else
            {
                if (info.is_reg)
                    reg_watchpoints.push_back(info);
                else
                    mem_watchpoints.push_back(info);
                InitList();
            }
        }
    }
    delete dialog;
}

/** OnBreakpoint
  *
  * Called when the user wants to add a breakpoint
  */
void DebugInfoList::OnBreakpoint(wxCommandEvent& event)
{
    lc3_breakpoint_info info;

    info.addr = 0x3000;
    info.condition = "1";
    info.enabled = true;
    info.hit_count = 0;
    info.label = "";
    info.max_hits = -1;

    BreakpointDialog* dialog = new BreakpointDialog(this, info, true);
    if (dialog->ShowModal() == wxID_OK)
    {
        int error;
        info = dialog->GetInfo(error);

        if (error)
            PrintError(error);
        else
        {
            bool multiple = false;
            for (unsigned int i = 0; i < breakpoints.size(); i++)
            {
                if (breakpoints[i].addr == info.addr)
                {
                    multiple = true;
                    break;
                }
            }

            if (multiple)
                wxMessageBox(_("ERROR! You can't have two breakpoints refer to the same address"), _("Error"));
            else
            {
                breakpoints.push_back(info);
                InitList();
            }
        }
    }
    delete dialog;
}
