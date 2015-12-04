#ifndef WATCHPOINT_DIALOG_HPP
#define WATCHPOINT_DIALOG_HPP

#include "WatchpointDialogDecl.h"
#include <lc3_all.hpp>
#include <wx/window.h>

class WatchpointDialog : public WatchpointDialogDecl
{
	public:
		WatchpointDialog(wxWindow* parent, lc3_watchpoint_info info, bool target_change);
		lc3_watchpoint_info GetInfo(int& error);
};

#endif
