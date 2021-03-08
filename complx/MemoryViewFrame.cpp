#include "MemoryViewFrame.hpp"
#include "MemoryViewContainer.hpp"
#include <wx/textdlg.h>
#include <wx/msgdlg.h>

void PrintError(int error);

/** InitGridSizes
  *
  * Initializes Column Sizes
  */
void MemoryViewFrame::InitGridSizes(void)
{
    memory->InitGridSizes();
}

/** OnGoto
  *
  * Called when the user wants to goto a certain address
  */
void MemoryViewFrame::OnGoto(wxCommandEvent& event)
{
    ::OnGoto(memory);
}

/** OnUpdateHideAddresses
  *
  * Called when the user wants change the hide memory address setting
  */
void MemoryViewFrame::OnUpdateHideAddresses(wxCommandEvent& event)
{
    if (!memoryView) return;
    int mode;
    if (menuViewHideAddressesShowAll->IsChecked())
        mode = SHOW_ALL;
    else if (menuViewHideAddressesShowOnlyCodeData->IsChecked())
        mode = SHOW_MODIFIED;
    else
        mode = SHOW_ALL;
    ::OnUpdateHideAddresses(memory, memoryView, mode);
}

/** OnHideAddressesCustom
  *
  * Called when the user wants customize what memory addresses are shown
  */
void MemoryViewFrame::OnHideAddressesCustom(wxCommandEvent& event)
{
    if (!memoryView) return;
    ::OnHideAddressesCustom(memory, memoryView);
}

/** OnDumbDisassemble
  *
  * Sets the mode to just straight disassembling
  * Does not take into account symbols.
  */
void MemoryViewFrame::OnDumbDisassemble(wxCommandEvent& event)
{
    ::OnDumbDisassemble(memory);
}

/** OnNormalDisassemble
  *
  * Sets the disassemble level to normal mode.
  */
void MemoryViewFrame::OnNormalDisassemble(wxCommandEvent& event)
{
    ::OnNormalDisassemble(memory);
}

/** OnCDisassemble
  *
  * Sets the disassemble level to C-Like
  */
void MemoryViewFrame::OnCDisassemble(wxCommandEvent& event)
{
    ::OnCDisassemble(memory);
}

/** OnInstructionHighlight
  *
  * Turns on/off Instruction highlighting
  */
void MemoryViewFrame::OnInstructionHighlight(wxCommandEvent& event)
{
    ::OnInstructionHighlight(memory, event.IsChecked());
}

/** OnFlipMemory
  *
  * Turns on/off flipped memory display
  */
void MemoryViewFrame::OnFlipMemory(wxCommandEvent& event)
{
    ::OnFlipMemory(memory, event.IsChecked());
}
