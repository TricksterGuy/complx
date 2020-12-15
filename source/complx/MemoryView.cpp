#include "MemoryView.hpp"

#include "data/MemoryViewBinaryDataRenderer.hpp"
#include "data/MemoryViewDataModel.hpp"
#include "data/MemoryViewInfoDataRenderer.hpp"
#include "util/GuiConstants.hpp"

#include <algorithm>
#include <logging.hpp>

#include <wx/debug.h>
#include <wx/menu.h>
#include <wx/settings.h>
#include <wx/version.h>



MemoryView::MemoryView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, int64_t style, const wxValidator& validator, const wxString& name) :
    wxDataViewCtrl(parent, id, pos, size, style, validator, name)
{
    EventLog l(__func__);

    const int info_column_width = GuiConstants().GetOffset("INFO_COLUMN_WIDTH");
    const int binary_column_width = GetTextExtent("0").GetWidth() * 16 + GuiConstants().GetOffset("BINARY_COLUMN_ADDITION");

    const unsigned int cell_flags = wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE;

    AppendColumn(new wxDataViewColumn("",               new MemoryViewInfoDataRenderer(),   MemoryInfo,     info_column_width,      wxALIGN_NOT,        0));

    AppendTextColumn("Address",         MemoryAddress,          wxDATAVIEW_CELL_INERT,      64,     wxALIGN_RIGHT,  wxDATAVIEW_COL_RESIZABLE);
    AppendTextColumn("Hex",             MemoryHexadecimal,      wxDATAVIEW_CELL_EDITABLE,   44,     wxALIGN_LEFT,   cell_flags);
    AppendTextColumn("Decimal",         MemoryDecimal,          wxDATAVIEW_CELL_EDITABLE,   64,     wxALIGN_LEFT,   cell_flags);

    AppendColumn(new wxDataViewColumn("Binary",         new MemoryViewBinaryDataRenderer(), MemoryBinary,   binary_column_width,    wxALIGN_CENTER,     cell_flags));

    AppendTextColumn("Label",           MemoryLabel,            wxDATAVIEW_CELL_INERT,      -1,     wxALIGN_LEFT,   cell_flags);
    AppendTextColumn("Instruction",     MemoryInstruction,      wxDATAVIEW_CELL_EDITABLE,   128,    wxALIGN_LEFT,   cell_flags);
    AppendTextColumn("Comment",         MemoryComment,          wxDATAVIEW_CELL_INERT,      -1,     wxALIGN_LEFT,   cell_flags);


    Connect(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, wxDataViewEventHandler(MemoryView::OnContextMenu), nullptr, this);
    Connect(MemoryMenuBreakpoint,   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MemoryView::OnBreakpoint));
    Connect(MemoryMenuTemporary,    wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MemoryView::OnTemporaryBreakpoint));
    Connect(MemoryMenuWatchpoint,   wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MemoryView::OnWatchpoint));
    Connect(MemoryMenuBlackbox,     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MemoryView::OnBlackbox));
    Connect(MemoryMenuAdvanced,     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MemoryView::OnAdvancedBreakpoint));
    Connect(MemoryMenuPCHere,       wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MemoryView::OnSetPcHere));
    Connect(MemoryMenuGoto,         wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MemoryView::OnGotoAddress));
    auto font = wxFont(wxFontInfo().Family(wxFONTFAMILY_TELETYPE).FaceName("Courier New"));
    SetFont(font);
}

int MemoryView::GetSelectedAddress() const
{
    if (!HasSelection())
        return -1;

    auto selection = GetSelection();

    const auto* model = GetModel();

    wxASSERT(model);

    const auto* lmodel = dynamic_cast<const wxDataViewListModel*>(model);

    wxASSERT(lmodel);

    return lmodel->GetRow(selection);
}

void MemoryView::ScrollTo(uint16_t address)
{
    VerboseLog("Scrolling Memory to x%04x", address);

    auto* model = GetModel();

    wxASSERT(model);

    auto* vlmodel = dynamic_cast<wxDataViewVirtualListModel*>(model);

    wxASSERT(vlmodel);

#ifdef _WIN32
    /// TODO Delete this section when wx 3.2 is a thing and GetCountPerPage can be used, odd numbered minor versions are experimental releases.
    uint16_t original = address;
#if wxCHECK_VERSION(3, 1, 0)
    // Apparently Windows uses the Generic wxDataViewCtrl whose implementation is to put the VisibleItem at the bottom.
    address += GetCountPerPage() - 1;
#else
    // GetCountPerPage is not available which makes fixing the above issue much easier...
    // There's also no GetRowHeight so approximate it.
    wxSize size = GetClientSize();
    address += size.y / 23 - 1;
#endif
    if (original > address)
        address = original;
#endif

    EnsureVisible(vlmodel->GetItem(address));

#ifdef _WIN32
    // The issue with the above correction is if the PC moves back up then the actual item we want visible will probably not be visible
    // So make another call with the item we want to be visible.
    EnsureVisible(vlmodel->GetItem(original));
#endif
}

void MemoryView::OnContextMenu(wxDataViewEvent& WXUNUSED(event))
{
    EventLog l(__func__);
    wxMenu menu;

    menu.Append(MemoryMenuTemporary,    "Mark as Temporary Breakpoint");
    menu.Append(MemoryMenuBreakpoint,   "Mark as Breakpoint");
    menu.Append(MemoryMenuWatchpoint,   "Mark as Watchpoint");
    menu.Append(MemoryMenuAdvanced,     "Advanced Breakpoint...");
    menu.Append(MemoryMenuBlackbox,     "Mark as Blackbox");
    menu.Append(MemoryMenuPCHere,       "Move PC Here");
    menu.Append(MemoryMenuGoto,         "Goto Address...");

    PopupMenu(&menu);
}

void MemoryView::OnTemporaryBreakpoint(wxCommandEvent& WXUNUSED(event))
{
    EventLog l(__func__);

    wxASSERT(state_ref);
    lc3_state& state = *state_ref;

    int addr = GetSelectedAddress();
    if (addr == -1)
    {
        WarnLog("No address selected");
        return;
    }

    auto address = static_cast<uint16_t>(addr);

    if (!lc3_has_breakpoint(state, address))
    {
        InfoLog("Adding temporary breakpoint at x%04x", address);
        lc3_add_break(state, address, "", "1", 1);
    }
    else
    {
        InfoLog("Removing breakpoint at x%04x", address);
        lc3_remove_break(state, address);
    }
}

void MemoryView::OnBreakpoint(wxCommandEvent& WXUNUSED(event))
{
    EventLog l(__func__);

    wxASSERT(state_ref);
    lc3_state& state = *state_ref;

    int addr = GetSelectedAddress();
    if (addr == -1)
    {
        WarnLog("No address selected");
        return;
    }

    auto address = static_cast<uint16_t>(addr);


    if (!lc3_has_breakpoint(state, address))
    {
        InfoLog("Adding breakpoint at x%04x", address);
        lc3_add_break(state, address);
    }
    else
    {
        InfoLog("Removing breakpoint at x%04x", address);
        lc3_remove_break(state, address);
    }
}

void MemoryView::OnWatchpoint(wxCommandEvent& WXUNUSED(event))
{
    EventLog l(__func__);

    wxASSERT(state_ref);
    lc3_state& state = *state_ref;

    int addr = GetSelectedAddress();
    if (addr == -1)
    {
        WarnLog("No address selected");
        return;
    }

    auto address = static_cast<uint16_t>(addr);

    if (!lc3_has_watch(state, false, address))
    {
        InfoLog("Adding watchpoint at x%04x", address);
        lc3_add_watch(state, false, address, "1");
    }
    else
    {
        InfoLog("Removing watchpoint at x%04x", address);
        lc3_remove_watch(state, false, address);
    }
}

void MemoryView::OnBlackbox(wxCommandEvent& WXUNUSED(event))
{
    EventLog l(__func__);

    wxASSERT(state_ref);
    lc3_state& state = *state_ref;

    int addr = GetSelectedAddress();
    if (addr == -1)
    {
        WarnLog("No address selected");
        return;
    }

    auto address = static_cast<uint16_t>(addr);

    if (!lc3_has_blackbox(state, address))
    {
        InfoLog("Adding blackbox at x%04x", address);
        lc3_add_blackbox(state, address);
    }
    else
    {
        InfoLog("Removing blackbox at x%04x", address);
        lc3_remove_blackbox(state, address);
    }
}

void MemoryView::OnAdvancedBreakpoint(wxCommandEvent& WXUNUSED(event))
{

}

void MemoryView::OnSetPcHere(wxCommandEvent& WXUNUSED(event))
{
    EventLog l(__func__);

    wxASSERT(state_ref);
    lc3_state& state = *state_ref;

    int addr = GetSelectedAddress();
    if (addr == -1)
    {
        WarnLog("No address selected");
        return;
    }

    state.pc = addr;
}

void MemoryView::OnGotoAddress(wxCommandEvent& WXUNUSED(event))
{
    EventLog l(__func__);

}

void MemoryView::UpdateRef(std::reference_wrapper<lc3_state> new_value)
{
    state_ref = new_value;

    auto* model = GetModel();

    if (!model) return;

    auto* mvdmodel = dynamic_cast<MemoryViewDataModel*>(model);

    wxASSERT(mvdmodel);

    mvdmodel->UpdateRef(new_value);
}
