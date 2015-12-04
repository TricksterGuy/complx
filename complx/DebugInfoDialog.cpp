#include "DebugInfoDialog.hpp"
#include "DebugInfoList.hpp"

/** GetMemWatchpoints
  */
std::vector<lc3_watchpoint_info> DebugInfoDialog::GetMemWatchpoints() const
{
    return debugpoints->GetMemWatchpoints();
}

/** GetRegWatchpoints
  */
std::vector<lc3_watchpoint_info> DebugInfoDialog::GetRegWatchpoints() const
{
    return debugpoints->GetRegWatchpoints();
}

/** GetBreakpoints
  */
std::vector<lc3_breakpoint_info> DebugInfoDialog::GetBreakpoints() const
{
    return debugpoints->GetBreakpoints();
}

/** OnRemove
  *
  * Called when something is about to be removed.
  */
void DebugInfoDialog::OnRemove(wxCommandEvent& event)
{
    debugpoints->OnRemove(event);
}

/** OnEdit
  *
  * Called when something is about to be edited
  */
void DebugInfoDialog::OnEdit(wxCommandEvent& event)
{
    debugpoints->OnEdit(event);
}

/** OnWatchpoint
  *
  * Called when a watchpoint is about to be created.
  */
void DebugInfoDialog::OnWatchpoint(wxCommandEvent& event)
{
    debugpoints->OnWatchpoint(event);
}

/** @brief OnBreakpoint
  *
  * Called when a breakpoint is about to be created
  */
void DebugInfoDialog::OnBreakpoint(wxCommandEvent& event)
{
    debugpoints->OnBreakpoint(event);
}

