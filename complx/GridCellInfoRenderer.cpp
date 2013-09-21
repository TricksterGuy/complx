#include "GridCellInfoRenderer.hpp"

extern wxImageList* infoImages;

/** GridCellInfoRenderer
  *
  * Constructor
  */
GridCellInfoRenderer::GridCellInfoRenderer()
{

}

/** ~GridCellInfoRenderer
  *
  * Destructor
  */
GridCellInfoRenderer::~GridCellInfoRenderer()
{

}

/** Draw
  *
  * Overriden from wxGridCellStringRenderer
  */
void GridCellInfoRenderer::Draw(wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, const wxRect &rect, int item, int column, bool isSelected)
{
    if (column != 0) return;

    bool pc = state.pc == item ? IMAGE_PC : 0;
    int bp = state.breakpoints.find(item) != state.breakpoints.end() ? IMAGE_BREAKPOINT : 0;
    bp = (bp == IMAGE_BREAKPOINT && !state.breakpoints[item].enabled) ? IMAGE_DISABLEDBP : bp;
    int bb = state.blackboxes.find(item) != state.blackboxes.end() ? IMAGE_BLACKBOX : 0;
    bb = (bb == IMAGE_BLACKBOX && !state.blackboxes[item].enabled) ? IMAGE_DISABLEDBB : bb;

    wxGridCellStringRenderer::Draw(grid, attr, dc, rect, item, column, isSelected);

    if (pc)
        infoImages->Draw(pc, dc, rect.GetX(), rect.GetY(), wxIMAGELIST_DRAW_TRANSPARENT | (isSelected ? wxIMAGELIST_DRAW_SELECTED : 0));
    if (bp)
        infoImages->Draw(bp, dc, rect.GetX(), rect.GetY(), wxIMAGELIST_DRAW_TRANSPARENT | (isSelected ? wxIMAGELIST_DRAW_SELECTED : 0));
    if (bb)
        infoImages->Draw(bb, dc, rect.GetX(), rect.GetY(), wxIMAGELIST_DRAW_TRANSPARENT | (isSelected ? wxIMAGELIST_DRAW_SELECTED : 0));
}
