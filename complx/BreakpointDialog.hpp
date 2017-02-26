#ifndef BREAKPOINT_DIALOG_HPP
#define BREAKPOINT_DIALOG_HPP

#include "BreakpointDialogDecl.h"
#include <lc3_all.hpp>
#include <wx/window.h>

class BreakpointDialog : public BreakpointDialogDecl
{
public:
    BreakpointDialog(wxWindow* parent, lc3_breakpoint_info info, bool addr_change = true);
    lc3_breakpoint_info GetInfo(int& error);
};


#endif
