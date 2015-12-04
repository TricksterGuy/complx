#include <wx/string.h>
#include "BreakpointDialog.hpp"

extern lc3_state state;

/** BreakpointDialog
  *
  * Constructor
  */
BreakpointDialog::BreakpointDialog(wxWindow* parent, lc3_breakpoint_info info, bool addr_change) : BreakpointDialogDecl(parent)
{
    name = wxString::FromUTF8(info.label.c_str());
    condition = wxString::FromUTF8(info.condition.c_str());

    std::string sym = lc3_sym_rev_lookup(state, info.addr);
    address = sym.empty() ? wxString::Format(_("0x%04X"), info.addr) : wxString::FromUTF8(sym.c_str());

    times = info.max_hits;
    enabled = info.enabled;
    hits = info.hit_count;

    if (!addr_change) address_text->Disable();
}

/** GetInfo
  *
  * Returns the breakpoint the user formed.
  */
lc3_breakpoint_info BreakpointDialog::GetInfo(int& error)
{
    lc3_breakpoint_info info;

    info.label = std::string(name.mb_str());
    info.condition = std::string(condition.mb_str());
    info.max_hits = times;
    info.enabled = enabled;
    int addr = 0;
    error = lc3_calculate(state, std::string(address.mb_str()), addr);
    info.addr = addr;
    info.hit_count = hits;
    return info;
}
