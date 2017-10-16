#include "WatchpointDialog.hpp"
#include <wx/string.h>

extern lc3_state state;

/** WatchpointDialog
  *
  * Constructor
  */
WatchpointDialog::WatchpointDialog(wxWindow* parent, lc3_watchpoint_info info, bool target_change) : WatchpointDialogDecl(parent)
{
    name = wxString::FromUTF8(info.label.c_str());
    condition = wxString::FromUTF8(info.condition.c_str());

    if (info.is_reg)
    {
        target = wxString::Format(_("R%d"), info.data);
    }
    else
    {
        std::string sym = lc3_sym_rev_lookup(state, info.data);
        target = sym.empty() ? wxString::Format(_("MEM[x%04X]"), info.data) : wxString::FromUTF8(sym.c_str());
    }

    times = info.max_hits;
    enabled = info.enabled;
    hits = info.hit_count;

    if (!target_change)
        target_text->Disable();
}

/** GetInfo
  *
  * Gets the watchpoint info
  */
lc3_watchpoint_info WatchpointDialog::GetInfo(int& error)
{
    lc3_watchpoint_info info;

    info.label = std::string(name.mb_str());
    info.condition = std::string(condition.mb_str());
    info.max_hits = times;
    info.enabled = enabled;

    if (target[0] == 'R' || target[0] == 'r')
    {
        info.is_reg = true;
        wxString num = target.Mid(1);
        long value;
        error = !num.ToLong(&value, 0);
        info.data = (unsigned short) (value & 0x7);
    }
    else if (target.StartsWith(_("MEM[")) || target.StartsWith(_("mem[")))
    {
        // MEM[0x5000]
        info.is_reg = false;
        wxString num = target.Mid(4, target.Length() - 4 - 1);
        long value;
        error = !num.ToLong(&value, 0);
        info.data = (unsigned short) (value & 0xFFFF);
    }
    else
    {
        // Could be a symbol so calculate.
        info.is_reg = false;
        int addr = 0;
        error = lc3_calculate(state, std::string(target.mb_str()), addr);
        info.data = (unsigned short) (addr & 0xFFFF);
    }

    info.hit_count = hits;
    return info;
}
