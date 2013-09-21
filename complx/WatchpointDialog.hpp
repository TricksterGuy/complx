#ifndef WATCHPOINT_DIALOG_HPP
#define WATCHPOINT_DIALOG_HPP

#include <wx/window.h>
#include "lc3.hpp"
#include "WatchpointDialogDecl.h"

extern lc3_state state;

class WatchpointDialog : public WatchpointDialogDecl
{
	public:
		WatchpointDialog(wxWindow* parent, lc3_watchpoint_info info, bool target_change);
		~WatchpointDialog();
		lc3_watchpoint_info GetInfo(int& error);
	private:

};

#endif
