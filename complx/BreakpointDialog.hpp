#ifndef BREAKPOINT_DIALOG_HPP
#define BREAKPOINT_DIALOG_HPP

#include <wx/window.h>
#include "BreakpointDialogDecl.h"
#include "lc3.hpp"

extern lc3_state state;

class BreakpointDialog : public BreakpointDialogDecl
{
	public:
		BreakpointDialog(wxWindow* parent, lc3_breakpoint_info info, bool addr_change = true);
		~BreakpointDialog();
		lc3_breakpoint_info GetInfo(int& error);
	private:
};


#endif
