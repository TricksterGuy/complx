#include <wx/msgdlg.h>
#include <wx/bitmap.h>
#include <wx/imaglist.h>
#include <wx/dcmemory.h>
#include <wx/colour.h>
#include <wx/brush.h>
#include <wx/menu.h>
#include "MemoryGrid.hpp"
#include "MemoryView.hpp"
#include "GridCellInfoRenderer.hpp"
#include "GridCellBinaryRenderer.hpp"
#include "BreakpointDialog.hpp"

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
    SetDefaultRowSize(12);

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

    // Hi I am a function call that takes 2+ seconds
    //AutoSizeColumn(MemoryBinary);

    //printf("%d\n", GetColSize(MemoryBinary));
    w -= 140;//GetColSize(MemoryBinary);

    SetColSize(MemoryInfo, 22);
    SetColSize(MemoryAddress, 8 * w / 75 - 8);
    SetColSize(MemoryHexadecimal, 8 * w / 75);
    SetColSize(MemoryDecimal, 10 * w / 75);
    SetColSize(MemoryBinary, 139);
    SetColSize(MemoryLabel, 18 * w / 75);
    SetColSize(MemoryInstruction, 31 * w / 75 + 9);

    wxGridCellAttr* info = new wxGridCellAttr; info->SetReadOnly(); info->SetTextColour(*wxBLACK);
    info->SetRenderer(new GridCellInfoRenderer());
    wxGridCellAttr* addr = new wxGridCellAttr; addr->SetReadOnly(); addr->SetTextColour(*wxBLACK);
    wxGridCellAttr* instr = new wxGridCellAttr; instr->SetTextColour(*wxBLACK);
    wxGridCellAttr* hex = new wxGridCellAttr; hex->SetTextColour(*wxBLUE);
    wxGridCellAttr* decimal = new wxGridCellAttr; decimal->SetTextColour(*wxRED);
    wxGridCellAttr* binary = new wxGridCellAttr; binary->SetTextColour(*wxWHITE); binary->SetBackgroundColour(wxTransparentColor);
    binary->SetRenderer(new GridCellBinaryRenderer());
    //binary->SetFont(wxFont( 10, 70, 90, wxFONTWEIGHT_BOLD, false, wxT("Courier New")));
    wxGridCellAttr* label = new wxGridCellAttr; label->SetTextColour(wxColour(64, 64, 64));

    SetColAttr(MemoryInfo, info);
    SetColAttr(MemoryAddress, addr);
    SetColAttr(MemoryHexadecimal, hex);
    SetColAttr(MemoryDecimal, decimal);
    SetColAttr(MemoryBinary, binary);
    SetColAttr(MemoryLabel, label);
    SetColAttr(MemoryInstruction, instr);
}

/** SelectLocation
  *
  * Shows a location on the grid
  */
void MemoryGrid::SelectLocation(unsigned short location)
{
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
    int addr = -1;
    addr = GetSelectedRow();
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
    int addr = -1;
    addr = GetSelectedRow();
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
    int addr = -1;
    addr = GetSelectedRow();
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
    int addr = -1;
    addr = GetSelectedRow();
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

    int addr = -1;
    addr = GetSelectedRow();
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
    int addr = -1;
    addr = GetSelectedRow();
    if (addr == -1) return;

    Refresh(state.pc);
    state.pc = addr;
    Refresh(addr);
}

/** GetSelectedRow
  *
  * Gets the first selected row
  */
int MemoryGrid::GetSelectedRow() const
{
    if (!IsSelection()) return -1;

    return GetSelectedRows()[0];
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

    if (col == MemoryInstruction && tipWindow == NULL)
    {
        toolTipLastRow = row;
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

    if (col == MemoryInstruction && tipWindow == NULL)
    {
        unsigned short address = (unsigned short)(row-1);
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
