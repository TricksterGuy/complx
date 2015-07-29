#include "MemoryViewContainer.hpp"

#include <vector>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>

extern lc3_state state;
extern std::vector<ViewRange> modified_addresses;
void PrintError(int error);

/** OnGoto
  *
  * Called when the user wants to goto a certain address
  */
void OnGoto(MemoryGrid* memory)
{
    wxString wxaddress = wxGetTextFromUser(_("Input Address"), _("Goto Address"));
    if (wxaddress.IsEmpty()) return;
    int addr;
    int error = lc3_calculate(state, wxaddress.ToStdString(), addr);

    if (error)
    {
        PrintError(error);
        return;
    }

    memory->SelectLocation((unsigned short) addr > 0xFFF0 ? 0xFFFF : 0);
    memory->SelectLocation((unsigned short) addr);
    memory->Refresh();
}

/** OnUpdateHideAddresses
  *
  * Called when the user wants change the hide memory address setting
  */
void OnUpdateHideAddresses(MemoryGrid* memory, MemoryView* memoryView, int mode)
{
    if (!memoryView) return;
    if (mode == SHOW_ALL)
        memoryView->ShowAllAddresses();
    else if (mode == SHOW_MODIFIED)
    {
        memoryView->ShowAllAddresses();
        memoryView->SetDefaultVisibility(ViewAction::HIDE);
        memoryView->ModifyAddresses(modified_addresses);
    }
    else if (mode == SHOW_NONZERO)
    {
        std::vector<ViewRange> ranges;
        int start = 0;
        for (int current = 1; current <= 0xFFFF; current++)
        {
            if (state.mem[current] != 0 && state.mem[current - 1] == 0)
                start = current;
            else if (state.mem[current] == 0 && state.mem[current - 1] != 0)
                ranges.emplace_back(start, current - 1);
        }
        if (state.mem[0xFFFF] != 0)
            ranges.emplace_back(start, 0xFFFF);
        memoryView->ShowAllAddresses();
        memoryView->SetDefaultVisibility(ViewAction::HIDE);
        memoryView->ModifyAddresses(ranges);
    }
    ///TODO learn the interface for updating a grid's dimensions via wxGridTableMessage
    // This is inefficient, but well...
    memory->SetView(memoryView);
    memory->SelectLocation((unsigned short) state.pc > 0xFFF0 ? 0xFFFF : 0);
    memory->SelectLocation((unsigned short) state.pc);
    memory->ForceRefresh();
}

/** OnGoto
  *
  * Called when the user wants customize what memory addresses are shown
  */
void OnHideAddressesCustom(MemoryGrid* memory, MemoryView* memoryView)
{
    ///TODO come up with a better dialog for this.
    std::string rangesStr = wxGetTextFromUser(
        "Enter a list of ranges in the format start-end,start2-end2,start3-end3\n"
        "Ending address for range is inclusive\n"
        "Example x3000-x3010,xE000-xF000",
        "Custom Hide Addresses").ToStdString();
		if (rangesStr.empty()) return;
		std::vector<std::string> parsed_ranges;
		tokenize(rangesStr, parsed_ranges, ",");
		std::vector<ViewRange> ranges;
		for (const auto& parsed_range : parsed_ranges)
    {
        int start, end;
        std::vector<std::string> start_end;
        tokenize(parsed_range, start_end, "-");
        if (start_end.size() != 2)
        {
            wxMessageBox(wxString::Format("%s is not a valid range", parsed_range), "Error");
            return;
        }
        if (lc3_calculate(state, start_end[0], start) ||
            lc3_calculate(state, start_end[1], end))
        {
            wxMessageBox(wxString::Format("%s does not contain valid addresses or malformed", parsed_range), "Error");
            return;
        }
        ranges.emplace_back(start, end);
    }
    memoryView->ShowAllAddresses();
    memoryView->SetDefaultVisibility(ViewAction::HIDE);
    memoryView->ModifyAddresses(ranges);
    ///TODO learn the interface for updating a grid's dimensions via wxGridTableMessage
    // This is inefficient, but well...
    memory->SetView(memoryView);
    memory->SelectLocation((unsigned short) state.pc > 0xFFF0 ? 0xFFFF : 0);
    memory->SelectLocation((unsigned short) state.pc);
    memory->ForceRefresh();
}

/** OnDumbDisassemble
  *
  * Sets the mode to just straight disassembling
  * Does not take into account symbols.
  */
void OnDumbDisassemble(MemoryGrid* memory)
{
    memory->SetDisassembleLevel(0);
    memory->Refresh();
}

/** OnNormalDisassemble
  *
  * Sets the disassemble level to normal mode.
  */
void OnNormalDisassemble(MemoryGrid* memory)
{
    memory->SetDisassembleLevel(1);
    memory->Refresh();
}

/** OnCDisassemble
  *
  * Sets the disassemble level to C-Like
  */
void OnCDisassemble(MemoryGrid* memory)
{
    memory->SetDisassembleLevel(2);
    memory->Refresh();
}

/** OnInstructionHighlight
  *
  * Turns on/off Instruction highlighting
  */
void OnInstructionHighlight(MemoryGrid* memory, bool set)
{
    memory->SetHighlight(set);
    memory->Refresh();
}
