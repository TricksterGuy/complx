#ifndef DEBUG_INFO_DIALOG_HPP
#define DEBUG_INFO_DIALOG_HPP

#include <wx/event.h>
#include <wx/window.h>
#include <vector>
#include "DebugInfoDialogDecl.h"
#include "lc3.hpp"

extern lc3_state state;

class DebugInfoDialog : public DebugInfoDialogDecl
{
	public:
		DebugInfoDialog(wxWindow* parent = NULL);
		~DebugInfoDialog();
		std::vector<lc3_breakpoint_info> GetBreakpoints() const;
		std::vector<lc3_watchpoint_info> GetRegWatchpoints() const;
		std::vector<lc3_watchpoint_info> GetMemWatchpoints() const;
	private:
		void OnBreakpoint(wxCommandEvent& event);
		void OnWatchpoint(wxCommandEvent& event);
		void OnEdit(wxCommandEvent& event);
		void OnRemove(wxCommandEvent& event);

};


#endif
