#include <wx/msgdlg.h>
#include <wx/bitmap.h>
#include <wx/imaglist.h>
#include <wx/dcmemory.h>
#include <wx/colour.h>
#include <wx/brush.h>
#include <wx/menu.h>
#include <wx/dcclient.h>
#include "MemoryGrid.hpp"
#include "MemoryView.hpp"
#include "GridCellInfoRenderer.hpp"
#include "GridCellBinaryRenderer.hpp"
#include "BreakpointDialog.hpp"
#include <lc3_all.hpp>
#include <wx/settings.h>

extern lc3_state state;
void PrintError(int error);

/** MemoryGrid
  *
  * Constructor
  */
MemoryGrid::MemoryGrid(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style) :
wxGrid(parent, id, pos, size, style), timer(this, MemoryToolTipTimer), tipWindow(NULL)
{
    last_address = 0x3000;
    highlight = true;
    SetUseNativeColLabels();

    wxFont font = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
    SetFont(font);
    SetDefaultRowSize(font.GetPointSize());

    Connect(MemoryMenuBreakpoint, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MemoryGrid::OnBreakpoint));
    Connect(MemoryMenuTemppoint, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MemoryGrid::OnTemppoint));
    Connect(MemoryMenuWatchpoint, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MemoryGrid::OnWatchpoint));
    Connect(MemoryMenuBlackbox, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MemoryGrid::OnBlackbox));
    Connect(MemoryMenuAdvancedpoint, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MemoryGrid::OnAdvancedpoint));
    Connect(MemoryMenuPCHere, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MemoryGrid::OnPCHere));
    Connect(wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler(MemoryGrid::OnContextMenu), NULL, this);
    Connect(wxEVT_GRID_CELL_CHANGED, wxGridEventHandler(MemoryGrid::OnGridChanged), NULL, this);
    Connect(wxEVT_ACTIVATE, wxActivateEventHandler(MemoryGrid::OnActivate), NULL, this);
    GetGridWindow()->Connect(wxEVT_MOTION, wxMouseEventHandler(MemoryGrid::OnMotion), NULL, this);
    Connect(MemoryToolTipTimer, wxEVT_TIMER, wxTimerEventHandler(MemoryGrid::OnShowToolTip), NULL, this);
    ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
}

/** ~MemoryGrid
  *
  * Destructor
  */
 MemoryGrid::~MemoryGrid()
{
    Disconnect(MemoryMenuBreakpoint, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MemoryGrid::OnBreakpoint));
    Disconnect(MemoryMenuTemppoint, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MemoryGrid::OnTemppoint));
    Disconnect(MemoryMenuWatchpoint, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MemoryGrid::OnWatchpoint));
    Disconnect(MemoryMenuBlackbox, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MemoryGrid::OnBlackbox));
    Disconnect(MemoryMenuAdvancedpoint, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MemoryGrid::OnAdvancedpoint));
    Disconnect(MemoryMenuPCHere, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MemoryGrid::OnPCHere));
    Disconnect(wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler(MemoryGrid::OnContextMenu), NULL, this);
    Disconnect(wxEVT_GRID_CELL_CHANGED, wxGridEventHandler(MemoryGrid::OnGridChanged), NULL, this);
    Disconnect(wxEVT_ACTIVATE, wxActivateEventHandler(MemoryGrid::OnActivate), NULL, this);
    GetGridWindow()->Disconnect(wxEVT_MOTION, wxMouseEventHandler(MemoryGrid::OnMotion), NULL, this);
    Disconnect(MemoryToolTipTimer, wxEVT_TIMER, wxTimerEventHandler(MemoryGrid::OnShowToolTip), NULL, this);
}

/** SetView
  *
  * Sets the backend Table the grid is using
  */
void MemoryGrid::SetView(MemoryView* view)
{
    // DO NOT SET TO TRUE.
    // See TODOs in MemoryViewContainer.cpp
    SetTable(view, false, wxGridSelectRows);
    InitGridSizes();
}

/** InitGridSizes
  *
  * Initializes the Grid Sizes
  */
void MemoryGrid::InitGridSizes()
{
    int w, h;
    GetClientSize(&w, &h);
    w -= 24;

    wxClientDC dc(this);
    dc.SetFont(GetFont());
    int binSize = dc.GetTextExtent("0000000000000000 ").GetWidth();
    int addrSize = dc.GetTextExtent("FFFF: ").GetWidth();
    int hexSize = dc.GetTextExtent("xFFFF ").GetWidth();
    int decimalSize = std::max(dc.GetTextExtent("-32768 ").GetWidth(), GetColSize(MemoryDecimal));
    int instrSize = dc.GetTextExtent("ABC 12345678901234567890 ").GetWidth();
    int labelSize = dc.GetTextExtent("1234567890 ").GetWidth();
    // Hi I am a function call that takes 2+ seconds
    //AutoSizeColumn(MemoryBinary);

    w -= binSize;//GetColSize(MemoryBinary);
    w -= addrSize;
    w -= hexSize;
    w -= decimalSize;
    w -= instrSize;
    w -= labelSize;
    w -= 10;
    /// TODO Apparently this can go below zero and cause an assertion failed
    if (w < 0) w = 200;

    SetColSize(MemoryInfo, 22);
    SetColSize(MemoryAddress, addrSize);
    SetColSize(MemoryHexadecimal, hexSize);
    SetColSize(MemoryDecimal, decimalSize);
    SetColSize(MemoryBinary, binSize);
    SetColSize(MemoryLabel, labelSize);
    SetColSize(MemoryInstruction, instrSize);
    SetColSize(MemoryComment, w);

    wxGridCellAttr* info = new wxGridCellAttr; info->SetReadOnly(); info->SetTextColour(*wxBLACK);
    /// TODO abstract out Memory View address->row translation into an object and pass it in here
    info->SetRenderer(new GridCellInfoRenderer(dynamic_cast<MemoryView*>(GetTable())));
    wxGridCellAttr* addr = new wxGridCellAttr; addr->SetReadOnly(); addr->SetTextColour(*wxBLACK);
    wxGridCellAttr* instr = new wxGridCellAttr; instr->SetTextColour(*wxBLACK);
    wxGridCellAttr* hex = new wxGridCellAttr; hex->SetTextColour(*wxBLUE);
    wxGridCellAttr* decimal = new wxGridCellAttr; decimal->SetTextColour(*wxRED);
    wxGridCellAttr* binary = new wxGridCellAttr; binary->SetTextColour(*wxWHITE); binary->SetBackgroundColour(wxTransparentColor);
    // Passing the view here is not needed because we call GetTableValueAsLong which takes care of view<=>address translation
    binary->SetRenderer(new GridCellBinaryRenderer());
    //binary->SetFont(wxFont( 10, 70, 90, wxFONTWEIGHT_BOLD, false, wxT("Courier New")));
    wxGridCellAttr* label = new wxGridCellAttr; label->SetTextColour(wxColour(64, 64, 64));
    wxGridCellAttr* comment = new wxGridCellAttr; comment->SetTextColour(wxColour(128, 128, 255));

    SetColAttr(MemoryInfo, info);
    SetColAttr(MemoryAddress, addr);
    SetColAttr(MemoryHexadecimal, hex);
    SetColAttr(MemoryDecimal, decimal);
    SetColAttr(MemoryBinary, binary);
    SetColAttr(MemoryLabel, label);
    SetColAttr(MemoryInstruction, instr);
    SetColAttr(MemoryComment, comment);
}

/** SelectLocation
  *
  * Shows a location on the grid
  */
void MemoryGrid::SelectLocation(unsigned short location)
{
    MemoryView* view = dynamic_cast<MemoryView*>(GetTable());
    if (view) location = view->AddressToView(location);
    last_address = location;
    // Handles close to edge!
    int n_location = (location + 8 + 1) > 0xFFFF ? location : location + 8 + 1;
    SelectRow(location);
    SetGridCursor(location, 0);
    MakeCellVisible((location < 16) ? location : n_location, 0);
    Refresh();
}

/** OnContextMenu
  *
  * Called when the user right clicks
  */
void MemoryGrid::OnContextMenu(wxGridEvent& event)
{
    wxMenu menu;

    menu.Append(MemoryMenuTemppoint, _("&Mark as One-Time Breakpoint\tF8"));
    menu.Append(MemoryMenuBreakpoint, _("Mark as &Breakpoint\tF9"));
    menu.Append(MemoryMenuWatchpoint, _("Mark as &Watchpoint\tF11"));
    menu.Append(MemoryMenuAdvancedpoint, _("&Advanced Breakpoint...\tF12"));
    menu.Append(MemoryMenuBlackbox, _("Mark as B&lackbox\tF5"));
    menu.Append(MemoryMenuPCHere, _("Move PC Here"));

    PopupMenu(&menu);
}

/** SetBreakpoint
  *
  * Sets a breakpoint
  */
void MemoryGrid::OnBreakpoint(wxCommandEvent& event)
{
    int addr = GetSelectedAddress();
    if (addr == -1) return;

    bool exist = lc3_add_break(state, (unsigned short) addr);
    if (exist) lc3_remove_break(state, (unsigned short) addr);
    Refresh();
}

/** SetWatchpoint
  *
  * Sets a watchpoint
  */
void MemoryGrid::OnWatchpoint(wxCommandEvent& event)
{
    int addr = GetSelectedAddress();
    if (addr == -1) return;

    bool exist = lc3_add_watch(state, false, (unsigned short) addr, "1");
    if (exist) lc3_remove_watch(state, false, (unsigned short) addr);
    Refresh(addr);
}

/** SetBlackbox
  *
  * Sets a blackbox
  */
void MemoryGrid::OnBlackbox(wxCommandEvent& event)
{
    int addr = GetSelectedAddress();
    if (addr == -1) return;

    bool exist = lc3_add_blackbox(state, (unsigned short) addr);
    if (exist) lc3_remove_blackbox(state, (unsigned short) addr);
    Refresh(addr);
}

/** SetTemppoint
  *
  * Sets a temporary breakpoint
  */
void MemoryGrid::OnTemppoint(wxCommandEvent& event)
{
    int addr = GetSelectedAddress();
    if (addr == -1) return;

    bool exist = lc3_add_break(state, (unsigned short) addr, "", "1", 1);
    if (exist) lc3_remove_break(state, (unsigned short) addr);
    Refresh(addr);
}

/** SetAdvancedpoint
  *
  * Sets an Advanced Breakpoint
  */
void MemoryGrid::OnAdvancedpoint(wxCommandEvent& event)
{
    lc3_breakpoint_info info;

    int addr = GetSelectedAddress();
    if (addr == -1) return;

    bool new_bp = state.breakpoints.find(addr) == state.breakpoints.end();
    if (new_bp)
    {
        info.addr = addr;
        info.condition = "1";
        info.enabled = true;
        info.hit_count = 0;
        info.label = "";
        info.max_hits = -1;
    }
    else
    {
        info = state.breakpoints[addr];
    }

    BreakpointDialog* dialog = new BreakpointDialog(this, info, false);
    if (dialog->ShowModal() == wxID_OK)
    {
        int error;
        info = dialog->GetInfo(error);

        if (error)
            PrintError(error);
        else
            state.breakpoints[info.addr] = info;
    }
    delete dialog;

    Refresh();
}

/** PCHere
  *
  * Moves the pc to the current address
  */
void MemoryGrid::OnPCHere(wxCommandEvent& event)
{
    int addr = GetSelectedAddress();
    if (addr == -1) return;

    Refresh(state.pc);
    state.pc = addr;
    Refresh(addr);
}

/** GetSelectedAddress
  *
  * Gets the first selected address
  */
int MemoryGrid::GetSelectedAddress() const
{
    if (!IsSelection()) return -1;

    int row = GetSelectedRows()[0];

    MemoryView* view = dynamic_cast<MemoryView*>(GetTable());
    row = view ? view->ViewToAddress(row) : -1;

    return row;
}

/** OnGridChange
  *
  * Called when a cell changes
  */
void MemoryGrid::OnGridChanged(wxGridEvent& event)
{
    Refresh();
}

/** AcceptsFocusFromKeyboard
  *
  * Do not tab into this control
  */
bool MemoryGrid::AcceptsFocusFromKeyboard() const
{
    return false;
}

/** OnActivate
  *
  *
  */
void MemoryGrid::OnActivate(wxActivateEvent& event)
{
    SelectLocation(last_address);
    event.Skip();
}

void MemoryGrid::OnMotion(wxMouseEvent& event)
{
    int x, y;
    CalcUnscrolledPosition(event.GetX(), event.GetY(), &x, &y);
    int row = YToRow(y);
    int col = XToCol(x);

    if (col == wxNOT_FOUND || row == wxNOT_FOUND)
    {
        timer.Stop();
        event.Skip();
		if (tipWindow != NULL)
		{
			delete tipWindow;
			tipWindow = NULL;
		}
        return;
    }

    if (col == MemoryComment && tipWindow == NULL)
    {
        timer.Start(500, wxTIMER_ONE_SHOT);
    }

    event.Skip();
}

void MemoryGrid::OnShowToolTip(wxTimerEvent& event)
{
    wxPoint p = ScreenToClient(wxGetMousePosition());
    int x, y;
    CalcUnscrolledPosition(p.x, p.y, &x, &y);
    int row = YToRow(y);
    int col = XToCol(x);

    if (col == MemoryComment && tipWindow == NULL)
    {
        MemoryView* view = dynamic_cast<MemoryView*>(GetTable());
        row = view ? view->ViewToAddress(row - 1) : -1;
        if (row == -1)
            return;
        unsigned short address = (unsigned short)(row);
        if (state.comments.find(address) == state.comments.end())
            return;
        const std::string& tooltip = state.comments[address];
        tipWindow = new wxTipWindow(this, tooltip);
        tipWindow->SetTipWindowPtr(&tipWindow);
        tipWindow->SetBoundingRect(wxRect(x + 30, y + 30, 100, 22));
    }
}

/** SetHighlight
  *
  * Sets Instruction Highlighting
  */
void MemoryGrid::SetHighlight(bool highlight)
{
    wxGridCellAttr* binary = new wxGridCellAttr; binary->SetTextColour(*wxWHITE); binary->SetBackgroundColour(wxTransparentColor);
    if (highlight) binary->SetRenderer(new GridCellBinaryRenderer());
    SetColAttr(MemoryBinary, binary);
}

/** SetDisassembleLevel
  *
  * Sets the disassemble level
  */
void MemoryGrid::SetDisassembleLevel(int level)
{
    static_cast<MemoryView*>(GetTable())->SetDisassembleLevel(level);
}

/** SetUnsignedMode
  *
  * Sets the decimal mode
  */
void MemoryGrid::SetUnsignedMode(bool unsigned_mode)
{
    static_cast<MemoryView*>(GetTable())->SetUnsignedMode(unsigned_mode);
}

/** SetFlippedMode
  *
  * Sets the flipped mode
  */
void MemoryGrid::SetFlippedMode(bool flipped_mode)
{
    static_cast<MemoryView*>(GetTable())->SetFlippedMode(flipped_mode);
}

