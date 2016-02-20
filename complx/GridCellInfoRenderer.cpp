#include "GridCellInfoRenderer.hpp"
#include <lc3_all.hpp>

extern wxImageList* infoImages;
extern lc3_state state;

/** Draw
  *
  * Overriden from wxGridCellStringRenderer
  */
void GridCellInfoRenderer::Draw(wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, const wxRect &rect, int item, int column, bool isSelected)
{
    if (column != 0) return;

    wxGridCellStringRenderer::Draw(grid, attr, dc, rect, item, column, isSelected);

    int address = view ? view->ViewToAddress(item) : -1;
    if (address == -1) return;

    bool pc = state.pc == address ? IMAGE_PC : 0;
    int bp = state.breakpoints.find(address) != state.breakpoints.end() ? IMAGE_BREAKPOINT : 0;
    bp = (bp == IMAGE_BREAKPOINT && !state.breakpoints[address].enabled) ? IMAGE_DISABLEDBP : bp;
    int bb = state.blackboxes.find(address) != state.blackboxes.end() ? IMAGE_BLACKBOX : 0;
    bb = (bb == IMAGE_BLACKBOX && !state.blackboxes[address].enabled) ? IMAGE_DISABLEDBB : bb;

    if (pc)
        infoImages->Draw(pc, dc, rect.GetX(), rect.GetY(), wxIMAGELIST_DRAW_TRANSPARENT | (isSelected ? wxIMAGELIST_DRAW_SELECTED : 0));
    if (bp)
        infoImages->Draw(bp, dc, rect.GetX(), rect.GetY(), wxIMAGELIST_DRAW_TRANSPARENT | (isSelected ? wxIMAGELIST_DRAW_SELECTED : 0));
    if (bb)
        infoImages->Draw(bb, dc, rect.GetX(), rect.GetY(), wxIMAGELIST_DRAW_TRANSPARENT | (isSelected ? wxIMAGELIST_DRAW_SELECTED : 0));
}
