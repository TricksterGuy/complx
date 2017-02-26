#ifndef GRID_CELL_INFO_RENDERER_HPP
#define GRID_CELL_INFO_RENDERER_HPP

#include <wx/dc.h>
#include <wx/gdicmn.h>
#include <wx/grid.h>
#include <wx/imaglist.h>

#include "MemoryView.hpp"

enum
{
    IMAGE_PC = 1,
    IMAGE_BREAKPOINT,
    IMAGE_DISABLEDBP,
    IMAGE_BLACKBOX,
    IMAGE_DISABLEDBB,
};

extern wxImageList* infoImages;

class GridCellInfoRenderer : public wxGridCellStringRenderer
{
public:
    GridCellInfoRenderer(MemoryView* mview) : view(mview) {}
    void Draw(wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, const wxRect &rect, int row, int col, bool isSelected);
private:
    /// Necessary for determining what address we are at.
    /// Not owned by this class
    MemoryView* view;
};

#endif
